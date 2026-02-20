#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/one_wire/one_wire.h"

#ifdef USE_ESP32
#include <soc/soc_caps.h>
#if SOC_RMT_SUPPORTED
#include <driver/rmt_tx.h>
#include <driver/rmt_rx.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#endif  // SOC_RMT_SUPPORTED
#endif  // USE_ESP32

namespace esphome {
namespace gpio {

class GPIOOneWireBus : public one_wire::OneWireBus, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::BUS; }

  void set_pin(InternalGPIOPin *pin) {
    this->t_pin_ = pin;
#if !(defined(USE_ESP32) && SOC_RMT_SUPPORTED)
    this->pin_ = pin->to_isr();
#endif
  }

  void write8(uint8_t val) override;
  void write64(uint64_t val) override;
  uint8_t read8() override;
  uint64_t read64() override;

 protected:
  InternalGPIOPin *t_pin_{nullptr};

  // ROM search state (shared by both implementations)
  uint8_t last_discrepancy_{0};
  bool last_device_flag_{false};
  uint64_t address_{0};

  int reset_int() override;
  void reset_search() override;
  uint64_t search_int() override;
  bool read_bit_();
  void write_bit_(bool bit);

#if defined(USE_ESP32) && SOC_RMT_SUPPORTED
  rmt_channel_handle_t tx_channel_{nullptr};
  rmt_channel_handle_t rx_channel_{nullptr};
  rmt_encoder_handle_t tx_bytes_encoder_{nullptr};
  rmt_encoder_handle_t tx_copy_encoder_{nullptr};
  rmt_symbol_word_t *rx_symbols_buf_{nullptr};
  QueueHandle_t receive_queue_{nullptr};

  void destroy_();
#else
  ISRInternalGPIOPin pin_;
  bool read_bit_(uint32_t *t);
#endif
};

}  // namespace gpio
}  // namespace esphome
