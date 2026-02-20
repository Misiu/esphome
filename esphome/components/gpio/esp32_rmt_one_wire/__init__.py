from esphome import pins
import esphome.codegen as cg
from esphome.components import esp32, esp32_rmt
from esphome.components.esp32 import include_builtin_idf_component
from esphome.components.one_wire import OneWireBus
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PIN

from .. import gpio_ns

DEPENDENCIES = ["esp32"]

ESP32RMTOneWireBus = gpio_ns.class_("ESP32RMTOneWireBus", OneWireBus, cg.Component)

CONFIG_SCHEMA = cv.All(
    esp32.only_on_variant(
        unsupported=list(esp32_rmt.VARIANTS_NO_RMT),
        msg_prefix="ESP32 RMT 1-wire",
    ),
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(ESP32RMTOneWireBus),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ).extend(cv.COMPONENT_SCHEMA),
)


async def to_code(config):
    # Re-enable ESP-IDF's RMT driver (excluded by default to save compile time)
    include_builtin_idf_component("esp_driver_rmt")

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    pin = await cg.gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_pin(pin))
