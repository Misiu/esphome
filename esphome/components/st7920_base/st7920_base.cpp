#include "st7920_base.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace st7920_base {

static const char *TAG = "st7920";

static const uint8_t BLACK = 0;
static const uint8_t WHITE = 1;

static const uint8_t ST7920_SETCOLADDR = 0x15;
static const uint8_t ST7920_SETROWADDR = 0x75;
static const uint8_t ST7920_SETCONTRAST = 0x81;
static const uint8_t ST7920_SETCURRENT = 0x84;

static const uint8_t ST7920_SETREMAP = 0xA0;
static const uint8_t ST7920_SETSTARTLINE = 0xA1;
static const uint8_t ST7920_SETOFFSET = 0xA2;
static const uint8_t ST7920_NORMALDISPLAY = 0xA4;
static const uint8_t ST7920_DISPLAYALLON = 0xA5;
static const uint8_t ST7920_DISPLAYALLOFF = 0xA6;
static const uint8_t ST7920_INVERTDISPLAY = 0xA7;
static const uint8_t ST7920_SETMULTIPLEX = 0xA8;
static const uint8_t ST7920_MASTERCONFIG = 0xAD;
static const uint8_t ST7920_DISPLAYOFF = 0xAE;
static const uint8_t ST7920_DISPLAYON = 0xAF;

static const uint8_t ST7920_SETPRECHARGECOMPENABLE = 0xB0;
static const uint8_t ST7920_SETPHASELEN = 0xB1;
static const uint8_t ST7920_SETROWPERIOD = 0xB2;
static const uint8_t ST7920_SETCLOCK = 0xB3;
static const uint8_t ST7920_SETPRECHARGECOMP = 0xB4;
static const uint8_t ST7920_SETGRAYTABLE = 0xB8;
static const uint8_t ST7920_SETPRECHARGEVOLTAGE = 0xBC;
static const uint8_t ST7920_SETVCOMLEVEL = 0xBE;
static const uint8_t ST7920_SETVSL = 0xBF;

static const uint8_t ST7920_GFXACCEL = 0x23;
static const uint8_t ST7920_DRAWRECT = 0x24;
static const uint8_t ST7920_COPY = 0x25;

void ST7920::setup() {
  this->init_internal_(this->get_buffer_length_());

  this->command(ST7920_DISPLAYOFF);   /* display off */
  this->command(ST7920_SETCLOCK);     /* set osc division */
  this->command(0xF1);                 /* 145 */
  this->command(ST7920_SETMULTIPLEX); /* multiplex ratio */
  this->command(0x3f);                 /* duty = 1/64 */
  this->command(ST7920_SETOFFSET);    /* set display offset --- */
  this->command(0x4C);                 /* 76 */
  this->command(ST7920_SETSTARTLINE); /*set start line */
  this->command(0x00);                 /* ------ */
  this->command(ST7920_MASTERCONFIG); /*Set Master Config DC/DC Converter*/
  this->command(0x02);
  this->command(ST7920_SETREMAP); /* set segment remap------ */
  this->command(0x56);
  this->command(ST7920_SETCURRENT + 0x2); /* Set Full Current Range */
  this->command(ST7920_SETGRAYTABLE);
  this->command(0x01);
  this->command(0x11);
  this->command(0x22);
  this->command(0x32);
  this->command(0x43);
  this->command(0x54);
  this->command(0x65);
  this->command(0x76);
  this->command(ST7920_SETCONTRAST); /* set contrast current */
  this->command(0x7F);                // max!
  this->command(ST7920_SETROWPERIOD);
  this->command(0x51);
  this->command(ST7920_SETPHASELEN);
  this->command(0x55);
  this->command(ST7920_SETPRECHARGECOMP);
  this->command(0x02);
  this->command(ST7920_SETPRECHARGECOMPENABLE);
  this->command(0x28);
  this->command(ST7920_SETVCOMLEVEL);  // Set High Voltage Level of COM Pin
  this->command(0x1C);                  //?
  this->command(ST7920_SETVSL);        // set Low Voltage Level of SEG Pin
  this->command(0x0D | 0x02);
  this->command(ST7920_NORMALDISPLAY); /* set display mode */
  this->command(ST7920_DISPLAYON);     /* display ON */
}
void ST7920::display() {
  this->command(ST7920_SETCOLADDR); /* set column address */
  this->command(0x00);               /* set column start address */
  this->command(0x3F);               /* set column end address */
  this->command(ST7920_SETROWADDR); /* set row address */
  this->command(0x00);               /* set row start address */
  this->command(0x3F);               /* set row end address */

  this->write_display_data();
}
void ST7920::update() {
  this->do_update_();
  this->display();
}
int ST7920::get_height_internal() {
  switch (this->model_) {
    case ST7920_MODEL_128_32:
      return 32;
    case ST7920_MODEL_128_64:
      return 64;
    default:
      return 0;
  }
}
int ST7920::get_width_internal() {
  switch (this->model_) {
    case ST7920_MODEL_128_32:
    case ST7920_MODEL_128_64:
      return 128;
    default:
      return 0;
  }
}
size_t ST7920::get_buffer_length_() {
  return size_t(this->get_width_internal()) * size_t(this->get_height_internal()) / 8u;
}

void HOT ST7920::draw_absolute_pixel_internal(int x, int y, int color) {
  if (x >= this->get_width_internal() || x < 0 || y >= this->get_height_internal() || y < 0)
    return;

  uint16_t pos = x + (y / 8) * this->get_width_internal();
  uint8_t subpos = y % 8;
  if (color) {
    this->buffer_[pos] |= (1 << subpos);
  } else {
    this->buffer_[pos] &= ~(1 << subpos);
  }
}
void ST7920::fill(int color) {
  uint8_t fill = color ? 0xFF : 0x00;
  for (uint32_t i = 0; i < this->get_buffer_length_(); i++)
    this->buffer_[i] = fill;
}
void ST7920::init_reset_() {
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(true);
    delay(1);
    // Trigger Reset
    this->reset_pin_->digital_write(false);
    delay(10);
    // Wake up
    this->reset_pin_->digital_write(true);
  }
}
const char *ST7920::model_str_() {
  switch (this->model_) {
    case ST7920_MODEL_128_32:
      return "ST7920 128x32";
    case ST7920_MODEL_128_64:
      return "ST7920 128x64";
    default:
      return "Unknown";
  }
}

}  // namespace st7920_base
}  // namespace esphome
