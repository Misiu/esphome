#pragma once

#ifdef USE_ESP32

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/one_wire/one_wire.h"

#include <driver/rmt_tx.h>
#include <driver/rmt_rx.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

namespace esphome {
namespace esp32_rmt_one_wire {

class ESP32RMTOneWireBus : public one_wire::OneWireBus, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::BUS; }

  void set_pin(InternalGPIOPin *pin) { this->pin_ = pin; }

  void write8(uint8_t val) override;
  void write64(uint64_t val) override;
  uint8_t read8() override;
  uint64_t read64() override;

 protected:
  InternalGPIOPin *pin_{nullptr};

  rmt_channel_handle_t tx_channel_{nullptr};
  rmt_channel_handle_t rx_channel_{nullptr};
  rmt_encoder_handle_t tx_bytes_encoder_{nullptr};
  rmt_encoder_handle_t tx_copy_encoder_{nullptr};
  rmt_symbol_word_t *rx_symbols_buf_{nullptr};
  QueueHandle_t receive_queue_{nullptr};

  // ROM search state
  uint8_t last_discrepancy_{0};
  bool last_device_flag_{false};
  uint64_t address_{0};

  int reset_int() override;
  void reset_search() override;
  uint64_t search_int() override;
  bool read_bit_();
  void write_bit_(bool bit);
};

}  // namespace esp32_rmt_one_wire
}  // namespace esphome

#endif  // USE_ESP32
