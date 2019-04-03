#include "esphome/defines.h"

#ifdef USE_NEO_PIXEL_BUS_LIGHT

#include "esphome/light/neo_pixel_bus_light_output.h"
#include "esphome/helpers.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

template<typename T_METHOD, typename T_COLOR_FEATURE>
const char *NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::TAG = "light.neo_pixel_bus";  // NOLINT

#ifdef USE_OUTPUT
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::set_power_supply(PowerSupplyComponent *power_supply) {
  this->power_supply_ = power_supply;
}
#endif

template<typename T_METHOD, typename T_COLOR_FEATURE>
NeoPixelBus<T_COLOR_FEATURE, T_METHOD> *NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::get_controller() const {
  return this->controller_;
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::clear_effect_data() {
  for (int i = 0; i < this->size(); i++)
    this->effect_data_[i] = 0;
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::add_leds(uint16_t count_pixels, uint8_t pin) {
  this->add_leds(new NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(count_pixels, pin));
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::add_leds(uint16_t count_pixels, uint8_t pin_clock,
                                                                     uint8_t pin_data) {
  this->add_leds(new NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(count_pixels, pin_clock, pin_data));
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::add_leds(uint16_t count_pixels) {
  this->add_leds(new NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(count_pixels));
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::add_leds(
    NeoPixelBus<T_COLOR_FEATURE, T_METHOD> *controller) {
  this->controller_ = controller;
  this->controller_->Begin();
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::setup() {
  for (int i = 0; i < this->size(); i++) {
    (*this)[i] = ESPColor(0, 0, 0, 0);
  }

  this->effect_data_ = new uint8_t[this->size()];
  this->controller_->Begin();
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::dump_config() {}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::loop() {
  if (!this->should_show_())
    return;

  this->mark_shown_();
  this->controller_->Dirty();

#ifdef USE_OUTPUT
  if (this->power_supply_ != nullptr) {
    bool is_light_on = false;
    for (int i = 0; i < this->size(); i++) {
      if ((*this)[i].get().is_on()) {
        is_light_on = true;
        break;
      }
    }

    if (is_light_on && !this->has_requested_high_power_) {
      this->power_supply_->request_high_power();
      this->has_requested_high_power_ = true;
    }
    if (!is_light_on && this->has_requested_high_power_) {
      this->power_supply_->unrequest_high_power();
      this->has_requested_high_power_ = false;
    }
  }
#endif

  this->controller_->Show();
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
float NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
int32_t NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::size() const {
  return this->controller_->PixelCount();
}

template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::set_pixel_order(ESPNeoPixelOrder order) {
  uint8_t u_order = static_cast<uint8_t>(order);
  this->rgb_offsets_[0] = (u_order >> 6) & 0b11;
  this->rgb_offsets_[1] = (u_order >> 4) & 0b11;
  this->rgb_offsets_[2] = (u_order >> 2) & 0b11;
  this->rgb_offsets_[3] = (u_order >> 0) & 0b11;
}

template<typename T_METHOD, typename T_COLOR_FEATURE>
ESPColorView NeoPixelRGBLightOutput<T_METHOD, T_COLOR_FEATURE>::operator[](int32_t index) const {
  uint8_t *base = this->controller_->Pixels() + 3ULL * index;
  return ESPColorView(base + this->rgb_offsets_[0], base + this->rgb_offsets_[1], base + this->rgb_offsets_[2], nullptr,
                      this->effect_data_ + index, &this->correction_);
}

template<typename T_METHOD, typename T_COLOR_FEATURE>
ESPColorView NeoPixelRGBWLightOutput<T_METHOD, T_COLOR_FEATURE>::operator[](int32_t index) const {
  uint8_t *base = this->controller_->Pixels() + 4ULL * index;
  return ESPColorView(base + this->rgb_offsets_[0], base + this->rgb_offsets_[1], base + this->rgb_offsets_[2],
                      base + this->rgb_offsets_[3], this->effect_data_ + index, &this->correction_);
}

template<typename T_METHOD, typename T_COLOR_FEATURE>
LightTraits NeoPixelRGBLightOutput<T_METHOD, T_COLOR_FEATURE>::get_traits() {
  return {true, true, false, false};
}

template<typename T_METHOD, typename T_COLOR_FEATURE>
LightTraits NeoPixelRGBWLightOutput<T_METHOD, T_COLOR_FEATURE>::get_traits() {
  return {true, true, true, false};
}

}  // namespace light

ESPHOME_NAMESPACE_END

#endif  // USE_NEO_PIXEL_BUS_LIGHT
