#ifndef ESPHOME_LIGHT_NEO_PIXEL_BUS_LIGHT_OUTPUT_H
#define ESPHOME_LIGHT_NEO_PIXEL_BUS_LIGHT_OUTPUT_H

#include "esphome/defines.h"

#ifdef USE_NEO_PIXEL_BUS_LIGHT

#ifdef ARDUINO_ESP8266_RELEASE_2_3_0
#error The NeoPixelBus library requires at least arduino_core_version 2.4.x
#endif

#include "esphome/helpers.h"
#include "esphome/light/light_state.h"
#include "esphome/light/addressable_light.h"
#include "esphome/power_supply_component.h"
#include "NeoPixelBus.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

enum class ESPNeoPixelOrder {
  GBWR = 0b11000110,
  GBRW = 0b10000111,
  GBR = 0b10000111,
  GWBR = 0b11001001,
  GRBW = 0b01001011,
  GRB = 0b01001011,
  GWRB = 0b10001101,
  GRWB = 0b01001110,
  BGWR = 0b11010010,
  BGRW = 0b10010011,
  BGR = 0b10010011,
  WGBR = 0b11011000,
  RGBW = 0b00011011,
  RGB = 0b00011011,
  WGRB = 0b10011100,
  RGWB = 0b00011110,
  BWGR = 0b11100001,
  BRGW = 0b01100011,
  BRG = 0b01100011,
  WBGR = 0b11100100,
  RBGW = 0b00100111,
  RBG = 0b00100111,
  WRGB = 0b01101100,
  RWGB = 0b00101101,
  BWRG = 0b10110001,
  BRWG = 0b01110010,
  WBRG = 0b10110100,
  RBWG = 0b00110110,
  WRBG = 0b01111000,
  RWBG = 0b00111001,
};

/** This component implements support for many types of addressable LED lights.
 *
 * To do this, it uses the NeoPixelBus library. The API for setting up the different
 * types of lights NeoPixelBus supports is intentionally kept as close to NeoPixelBus defaults
 * as possible. To use NeoPixelBus lights with ESPHome, first set up the component using
 * the helper in Application, then add the LEDs using the `add_leds` helper functions.
 *
 * These add_leds helpers can, however, only be called once on a NeoPixelBusLightOutputComponent.
 */
template<typename T_METHOD, typename T_COLOR_FEATURE>
class NeoPixelBusLightOutputBase : public Component, public AddressableLight {
 public:
#ifdef USE_OUTPUT
  void set_power_supply(PowerSupplyComponent *power_supply);
#endif

  NeoPixelBus<T_COLOR_FEATURE, T_METHOD> *get_controller() const;

  void clear_effect_data() override;

  /// Add some LEDS, can only be called once.
  void add_leds(uint16_t count_pixels, uint8_t pin);
  void add_leds(uint16_t count_pixels, uint8_t pin_clock, uint8_t pin_data);
  void add_leds(uint16_t count_pixels);
  void add_leds(NeoPixelBus<T_COLOR_FEATURE, T_METHOD> *controller);

  // ========== INTERNAL METHODS ==========
  void setup() override;

  void dump_config() override;

  void loop() override;

  float get_setup_priority() const override;

  int32_t size() const override;

  void set_pixel_order(ESPNeoPixelOrder order);

 protected:
  NeoPixelBus<T_COLOR_FEATURE, T_METHOD> *controller_{nullptr};
  uint8_t *effect_data_{nullptr};
  uint8_t rgb_offsets_[4]{0, 1, 2, 3};
#ifdef USE_OUTPUT
  PowerSupplyComponent *power_supply_{nullptr};
  bool has_requested_high_power_{false};
#endif

  static const char *TAG;  // NOLINT
};

template<typename T_METHOD, typename T_COLOR_FEATURE = NeoRgbFeature>
class NeoPixelRGBLightOutput : public NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE> {
 public:
  inline ESPColorView operator[](int32_t index) const override;

  LightTraits get_traits() override;
};

template<typename T_METHOD, typename T_COLOR_FEATURE = NeoRgbwFeature>
class NeoPixelRGBWLightOutput : public NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE> {
 public:
  inline ESPColorView operator[](int32_t index) const override;

  LightTraits get_traits() override;
};

}  // namespace light

ESPHOME_NAMESPACE_END

#include "esphome/light/neo_pixel_bus_light_output.tcc"

#endif  // USE_NEO_PIXEL_BUS_LIGHT

#endif  // ESPHOME_LIGHT_NEO_PIXEL_BUS_LIGHT_OUTPUT_H
