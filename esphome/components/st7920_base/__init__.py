import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display
from esphome.const import CONF_EXTERNAL_VCC, CONF_LAMBDA, CONF_MODEL, CONF_RESET_PIN
from esphome.core import coroutine

st7920_base_ns = cg.esphome_ns.namespace('st7920_base')
ST7920 = st7920_base_ns.class_('ST7920', cg.PollingComponent, display.DisplayBuffer)
ST7920Model = st7920_base_ns.enum('ST7920Model')

MODELS = {
    'ST7920_128X32': ST7920Model.ST7920_MODEL_128_32,
    'ST7920_128X64': ST7920Model.ST7920_MODEL_128_64,
}

ST7920_MODEL = cv.enum(MODELS, upper=True, space="_")

ST7920_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend({
    cv.Required(CONF_MODEL): ST7920_MODEL,
    cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
    cv.Optional(CONF_EXTERNAL_VCC): cv.boolean,
}).extend(cv.polling_component_schema('1s'))


@coroutine
def setup_ssd1036(var, config):
    yield cg.register_component(var, config)
    yield display.register_display(var, config)

    cg.add(var.set_model(config[CONF_MODEL]))
    if CONF_RESET_PIN in config:
        reset = yield cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(reset))
    if CONF_EXTERNAL_VCC in config:
        cg.add(var.set_external_vcc(config[CONF_EXTERNAL_VCC]))
    if CONF_LAMBDA in config:
        lambda_ = yield cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayBufferRef, 'it')], return_type=cg.void)
        cg.add(var.set_writer(lambda_))
