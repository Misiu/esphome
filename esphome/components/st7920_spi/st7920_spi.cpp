#include "st7920_spi.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace st7920_spi {

static const char *TAG = "st7920_spi";

void SPIST7920::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SPI ST7920...");
  this->spi_setup();
  this->dc_pin_->setup();  // OUTPUT
  this->cs_->setup();      // OUTPUT

  this->init_reset_();
  delay(500);  // NOLINT
  ST7920::setup();
}
void SPIST7920::dump_config() {
  LOG_DISPLAY("", "SPI ST7920", this);
  ESP_LOGCONFIG(TAG, "  Model: %s", this->model_str_());
  LOG_PIN("  CS Pin: ", this->cs_);
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  ESP_LOGCONFIG(TAG, "  External VCC: %s", YESNO(this->external_vcc_));
  LOG_UPDATE_INTERVAL(this);
}
void SPIST7920::command(uint8_t value) {
  this->cs_->digital_write(true);
  this->dc_pin_->digital_write(false);
  delay(1);
  this->enable();
  this->cs_->digital_write(false);
  this->write_byte(value);
  this->cs_->digital_write(true);
  this->disable();
}
void HOT SPIST7920::write_display_data() {
  this->cs_->digital_write(true);
  this->dc_pin_->digital_write(true);
  this->cs_->digital_write(false);
  delay(1);
  this->enable();
  for (uint16_t x = 0; x < this->get_width_internal(); x += 2) {
    for (uint16_t y = 0; y < this->get_height_internal(); y += 8) {  // we write 8 pixels at once
      uint8_t left8 = this->buffer_[y * 16 + x];
      uint8_t right8 = this->buffer_[y * 16 + x + 1];
      for (uint8_t p = 0; p < 8; p++) {
        uint8_t d = 0;
        if (left8 & (1 << p))
          d |= 0xF0;
        if (right8 & (1 << p))
          d |= 0x0F;
        this->write_byte(d);
      }
    }
  }
  this->cs_->digital_write(true);
  this->disable();
}

}  // namespace st7920_spi
}  // namespace esphome
