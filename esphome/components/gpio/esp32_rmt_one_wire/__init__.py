from esphome import pins
import esphome.codegen as cg
from esphome.components.one_wire import OneWireBus
from esphome.components.esp32_rmt import VARIANTS_NO_RMT
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PIN

CODEOWNERS = ["@ssieb"]
DEPENDENCIES = ["esp32"]

esp32_rmt_one_wire_ns = cg.esphome_ns.namespace("esp32_rmt_one_wire")
ESP32RMTOneWireBus = esp32_rmt_one_wire_ns.class_("ESP32RMTOneWireBus", OneWireBus, cg.Component)


def _validate(config):
    from esphome.components.esp32 import get_esp32_variant

    variant = get_esp32_variant()
    if variant in VARIANTS_NO_RMT:
        raise cv.Invalid(
            f"esp32_rmt_one_wire is not available on {variant} (no RMT hardware)"
        )
    return config


CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(ESP32RMTOneWireBus),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ).extend(cv.COMPONENT_SCHEMA),
    cv.only_on_esp32,
    _validate,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    pin = await cg.gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_pin(pin))
