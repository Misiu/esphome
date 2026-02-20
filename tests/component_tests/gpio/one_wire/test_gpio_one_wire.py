"""Tests for the GPIO 1-wire bus component.

Covers:
- ESP32 with RMT hardware: esp_driver_rmt IDF component must be included
- ESP32-C2 (no RMT hardware): esp_driver_rmt must stay excluded
- Non-ESP32 (ESP8266): no esp32 config data; GPIO bit-bang path used
- Config validates correctly for each platform
"""

from __future__ import annotations

from collections.abc import Callable
from pathlib import Path

HERE = Path(__file__).parent


def test_gpio_one_wire_esp32_idf_registers_bus(
    generate_main: Callable[[str | Path], str],
) -> None:
    """ESP32 IDF: GPIOOneWireBus must be instantiated and registered as a component."""
    main_cpp = generate_main(HERE / "test_gpio_one_wire_esp32_idf.yaml")

    assert "new gpio::GPIOOneWireBus();" in main_cpp
    assert 'set_component_source(LOG_STR("gpio.one_wire"))' in main_cpp


def test_gpio_one_wire_esp32_idf_includes_rmt_driver(
    generate_main: Callable[[str | Path], str],
) -> None:
    """ESP32 IDF: esp_driver_rmt must be removed from the exclusion list so it gets compiled.

    SOC_RMT_SUPPORTED is true for all ESP32 variants except C2 and C61.  The
    Python to_code() path calls include_builtin_idf_component("esp_driver_rmt")
    for those variants, which removes it from KEY_EXCLUDE_COMPONENTS.
    """
    generate_main(HERE / "test_gpio_one_wire_esp32_idf.yaml")

    # Check that the RMT driver was un-excluded (i.e. it will be compiled)
    from esphome.components.esp32 import KEY_ESP32, KEY_EXCLUDE_COMPONENTS
    from esphome.core import CORE

    excluded = CORE.data.get(KEY_ESP32, {}).get(KEY_EXCLUDE_COMPONENTS, set())
    assert "esp_driver_rmt" not in excluded, (
        "esp_driver_rmt should be included (removed from exclusions) on ESP32 IDF with RMT support"
    )


def test_gpio_one_wire_esp32c2_excludes_rmt_driver(
    generate_main: Callable[[str | Path], str],
) -> None:
    """ESP32-C2 (no RMT hardware): esp_driver_rmt must remain excluded.

    The ESP32-C2 variant is in VARIANTS_NO_RMT.  to_code() must NOT call
    include_builtin_idf_component() for it, so the RMT driver stays excluded
    and the GPIO bit-bang code path is compiled instead.
    """
    generate_main(HERE / "test_gpio_one_wire_esp32c2_idf.yaml")

    from esphome.components.esp32 import KEY_ESP32, KEY_EXCLUDE_COMPONENTS
    from esphome.core import CORE

    excluded = CORE.data.get(KEY_ESP32, {}).get(KEY_EXCLUDE_COMPONENTS, set())
    assert "esp_driver_rmt" in excluded, (
        "esp_driver_rmt should remain excluded on ESP32-C2 (no RMT hardware)"
    )


def test_gpio_one_wire_esp32c2_config_valid(
    generate_main: Callable[[str | Path], str],
) -> None:
    """ESP32-C2: Configuration must be accepted without errors (no validation failure).

    The C2 variant uses GPIO bit-bang at runtime (with a warning), but the
    YAML configuration itself is perfectly valid.
    """
    main_cpp = generate_main(HERE / "test_gpio_one_wire_esp32c2_idf.yaml")

    assert "new gpio::GPIOOneWireBus();" in main_cpp


def test_gpio_one_wire_esp8266_registers_bus(
    generate_main: Callable[[str | Path], str],
) -> None:
    """ESP8266: GPIOOneWireBus must be instantiated using the GPIO bit-bang path."""
    main_cpp = generate_main(HERE / "test_gpio_one_wire_esp8266.yaml")

    assert "new gpio::GPIOOneWireBus();" in main_cpp
    assert 'set_component_source(LOG_STR("gpio.one_wire"))' in main_cpp


def test_gpio_one_wire_esp8266_no_rmt_data(
    generate_main: Callable[[str | Path], str],
) -> None:
    """ESP8266: No ESP32-specific RMT data should be present in CORE after generation."""
    generate_main(HERE / "test_gpio_one_wire_esp8266.yaml")

    from esphome.components.esp32 import KEY_ESP32
    from esphome.core import CORE

    # On ESP8266 there is no ESP32 platform data at all
    assert KEY_ESP32 not in CORE.data, (
        "ESP32 platform data should not be present when targeting ESP8266"
    )
