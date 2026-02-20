# ESP32 RMT 1-Wire Bus Component

Hardware-accelerated 1-Wire bus driver for ESP32 using the RMT (Remote Control Transceiver) peripheral.
Unlike the GPIO bit-bang implementation, this driver uses the ESP32's dedicated RMT hardware,
which provides more reliable timing and frees the CPU during bus operations.

## Supported platforms

- ESP32 (all variants **except** ESP32-C2 and ESP32-C61, which have no RMT peripheral)
- Requires ESP-IDF framework

## Usage

### As a built-in component

```yaml
esp32_rmt_one_wire:
  id: my_bus
  pin: GPIO38

sensor:
  - platform: dallas_temp
    one_wire_id: my_bus
    address: 0xc80661d4465cae28
    name: "Temperature"
    resolution: 11
    update_interval: 10s
```

### As an external component

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/Misiu/esphome
      ref: one_wire
    components: [esp32_rmt_one_wire]
    refresh: 1h

esp32_rmt_one_wire:
  id: my_bus
  pin: GPIO38

sensor:
  - platform: dallas_temp
    one_wire_id: my_bus
    address: 0xc80661d4465cae28
    name: "Temperature"
    resolution: 11
    update_interval: 10s
```

## Configuration variables

| Variable | Type     | Required | Description                            |
|----------|----------|----------|----------------------------------------|
| `id`     | ID       | No       | Identifier for this bus instance       |
| `pin`    | GPIO pin | **Yes**  | GPIO pin connected to the 1-Wire bus   |

## Notes

- An external 4.7 kΩ pull-up resistor to VCC is recommended for reliable operation,
  especially on longer bus runs. The internal pull-up is enabled automatically but
  may not be sufficient for all setups.
- Multiple sensors on the same bus are supported via the `address` or `index` options
  on the `dallas_temp` platform.
