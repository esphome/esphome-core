#include "esphome/defines.h"

#ifdef USE_NEO_PIXEL_BUS_LIGHT

#include "esphome/light/neo_pixel_bus_light_output.h"
#include "esphome/helpers.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

template<typename T_METHOD, typename T_COLOR_FEATURE>
const char* NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::TAG = "light.neo_pixel_bus";

template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::schedule_show() {
  this->next_show_ = true;
}

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
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::set_correction(float red,
                                                                           float green,
                                                                           float blue,
                                                                           float white) {
  this->correction_.set_max_brightness(ESPColor(
      uint8_t(roundf(red * 255.0f)),
      uint8_t(roundf(green * 255.0f)),
      uint8_t(roundf(blue * 255.0f)),
      uint8_t(roundf(white * 255.0f))
  ));
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::clear_effect_data() {
  for (int i = 0; i < this->size(); i++)
    this->effect_data_[i] = 0;
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::setup_state(LightState *state) {
  this->correction_.calculate_gamma_table(state->get_gamma_correct());
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::add_leds(uint16_t count_pixels, uint8_t pin) {
  this->add_leds(new NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(count_pixels, pin));
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::add_leds(uint16_t count_pixels,
                                                                     uint8_t pin_clock,
                                                                     uint8_t pin_data) {
  this->add_leds(new NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(count_pixels, pin_clock, pin_data));
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::add_leds(uint16_t count_pixels) {
  this->add_leds(new NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(count_pixels));
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::add_leds(NeoPixelBus<T_COLOR_FEATURE, T_METHOD> *controller) {
  this->controller_ = controller;
  this->controller_->Begin();
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::write_state(LightState *state) {
  LightColorValues value = state->get_current_values();
  uint8_t max_brightness = roundf(value.get_brightness() * value.get_state() * 255.0f);
  this->correction_.set_local_brightness(max_brightness);

  if (this->is_effect_active())
    return;

  auto val = state->get_current_values();
  // don't use LightState helper, gamma correction+brightness is handled by ESPColorView
  ESPColor color = ESPColor(
      uint8_t(roundf(val.get_red()   * 255.0f)),
      uint8_t(roundf(val.get_green() * 255.0f)),
      uint8_t(roundf(val.get_blue()  * 255.0f)),
      uint8_t(roundf(val.get_white() * 255.0f))
  );

  for (int i = 0; i < this->size(); i++) {
    (*this)[i] = color;
  }

  this->schedule_show();
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::setup() {
  ESP_LOGCONFIG(TAG, "Setting up NeoPixelBus light...");
  for (int i = 0; i < this->size(); i++) {
    (*this)[i] = ESPColor(0, 0, 0, 0);
  }

  this->effect_data_ = new uint8_t[this->size()];
  this->controller_->Begin();
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::dump_config() {
  ESP_LOGCONFIG(TAG, "NeoPixelBus light:");
  ESP_LOGCONFIG(TAG, "  Num LEDs: %u", this->controller_->PixelCount());
}
template<typename T_METHOD, typename T_COLOR_FEATURE>
void NeoPixelBusLightOutputBase<T_METHOD, T_COLOR_FEATURE>::loop() {
  if (!this->next_show_ && !this->is_effect_active())
    return;

  this->next_show_ = false;
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

  ESP_LOGVV(TAG, "Writing RGB values to bus...");
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
  uint8_t order_ = static_cast<uint8_t>(order);
  this->rgb_offsets_[0] = (order_ >> 6) & 0b11;
  this->rgb_offsets_[1] = (order_ >> 4) & 0b11;
  this->rgb_offsets_[2] = (order_ >> 2) & 0b11;
  this->rgb_offsets_[3] = (order_ >> 0) & 0b11;
}

template<typename T_METHOD, typename T_COLOR_FEATURE>
ESPColorView NeoPixelRGBLightOutput<T_METHOD, T_COLOR_FEATURE>::operator[](int32_t index) const {
  uint8_t *base = this->controller_->Pixels() + 3ULL * index;
  return ESPColorView(
      base + this->rgb_offsets_[0],
      base + this->rgb_offsets_[1],
      base + this->rgb_offsets_[2],
      nullptr,
      this->effect_data_ + index,
      &this->correction_
  );
}

template<typename T_METHOD, typename T_COLOR_FEATURE>
ESPColorView NeoPixelRGBWLightOutput<T_METHOD, T_COLOR_FEATURE>::operator[](int32_t index) const {
  uint8_t *base = this->controller_->Pixels() + 4ULL * index;
  return ESPColorView(
      base + this->rgb_offsets_[0],
      base + this->rgb_offsets_[1],
      base + this->rgb_offsets_[2],
      base + this->rgb_offsets_[3],
      this->effect_data_ + index,
      &this->correction_
  );
}

template<typename T_METHOD, typename T_COLOR_FEATURE>
LightTraits NeoPixelRGBLightOutput<T_METHOD, T_COLOR_FEATURE>::get_traits() {
  return {true, true, false, false};
}

template<typename T_METHOD, typename T_COLOR_FEATURE>
LightTraits NeoPixelRGBWLightOutput<T_METHOD, T_COLOR_FEATURE>::get_traits() {
  return {true, true, true, false};
}

} // namespace light

ESPHOME_NAMESPACE_END

#endif //USE_NEO_PIXEL_BUS_LIGHT
