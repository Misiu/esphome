from esphome import pins
import esphome.codegen as cg
from esphome.components.one_wire import OneWireBus
from esphome.config_helpers import filter_source_files_from_platform
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PIN, PlatformFramework
from esphome.core import CORE

from .. import gpio_ns

CODEOWNERS = ["@ssieb"]

GPIOOneWireBus = gpio_ns.class_("GPIOOneWireBus", OneWireBus, cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(GPIOOneWireBus),
        cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    pin = await cg.gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_pin(pin))

    if CORE.is_esp32:
        from esphome.components import esp32_rmt
        from esphome.components.esp32 import (
            get_esp32_variant,
            include_builtin_idf_component,
        )

        # Include the RMT driver for all ESP32 variants that have RMT hardware.
        # Variants without RMT (C2, C61) fall back to GPIO bit-banging at runtime.
        if get_esp32_variant() not in esp32_rmt.VARIANTS_NO_RMT:
            # Re-enable ESP-IDF's RMT driver (excluded by default to save compile time)
            include_builtin_idf_component("esp_driver_rmt")


FILTER_SOURCE_FILES = filter_source_files_from_platform(
    {
        "gpio_one_wire_rmt.cpp": {
            PlatformFramework.ESP32_ARDUINO,
            PlatformFramework.ESP32_IDF,
        },
        "gpio_one_wire.cpp": {
            PlatformFramework.ESP32_ARDUINO,
            PlatformFramework.ESP32_IDF,
            PlatformFramework.ESP8266_ARDUINO,
            PlatformFramework.BK72XX_ARDUINO,
            PlatformFramework.RTL87XX_ARDUINO,
            PlatformFramework.LN882X_ARDUINO,
            PlatformFramework.RP2040_ARDUINO,
        },
    }
)
