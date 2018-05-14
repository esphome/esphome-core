//
// Created by Otto Winter on 28.11.17.
//

#include "esphomelib/light/light_color_values.h"

#include <sstream>
#include <iomanip>

#include "esphomelib/helpers.h"
#include "esphomelib/component.h"
#include "esphomelib/log.h"
#include "esphomelib/espmath.h"
#include "esphomelib/esppreferences.h"

#ifdef USE_LIGHT

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

static const char *TAG = "light.light_color_values";

float LightColorValues::get_state() const {
  return this->state_;
}

void LightColorValues::set_state(float state) {
  this->state_ = clamp(0.0f, 1.0f, state);
}

float LightColorValues::get_brightness() const {
  return this->brightness_;
}

void LightColorValues::set_brightness(float brightness) {
  this->brightness_ = clamp(0.0f, 1.0f, brightness);
}

float LightColorValues::get_red() const {
  return this->red_;
}

void LightColorValues::set_red(float red) {
  this->red_ = clamp(0.0f, 1.0f, red);
}

float LightColorValues::get_green() const {
  return this->green_;
}

void LightColorValues::set_green(float green) {
  this->green_ = clamp(0.0f, 1.0f, green);
}

float LightColorValues::get_blue() const {
  return this->blue_;
}

void LightColorValues::set_blue(float blue) {
  this->blue_ = clamp(0.0f, 1.0f, blue);
}

float LightColorValues::get_white() const {
  return white_;
}

void LightColorValues::set_white(float white) {
  this->white_ = clamp(0.0f, 1.0f, white);
}

LightColorValues::LightColorValues()
    : state_(0.0f), brightness_(1.0f), red_(1.0f), green_(1.0f), blue_(1.0f), white_(1.0f) {

}

LightColorValues LightColorValues::lerp(const LightColorValues &start, const LightColorValues &end,
                                        float completion) {
  LightColorValues v;
  v.set_state(esphomelib::lerp(start.get_state(), end.get_state(), completion));
  v.set_brightness(esphomelib::lerp(start.get_brightness(), end.get_brightness(), completion));
  v.set_red(esphomelib::lerp(start.get_red(), end.get_red(), completion));
  v.set_green(esphomelib::lerp(start.get_green(), end.get_green(), completion));
  v.set_blue(esphomelib::lerp(start.get_blue(), end.get_blue(), completion));
  v.set_white(esphomelib::lerp(start.get_white(), end.get_white(), completion));

  return v;
}

LightColorValues::LightColorValues(float state, float brightness, float red, float green, float blue,
                                   float white) {
  this->set_state(state);
  this->set_brightness(brightness);
  this->set_red(red);
  this->set_green(green);
  this->set_blue(blue);
  this->set_white(white);
}

void LightColorValues::load_from_preferences(const std::string &friendly_name) {
  this->set_state(global_preferences.get_float(friendly_name, "state", this->get_state()));
  this->set_brightness(global_preferences.get_float(friendly_name, "brightness", this->get_brightness()));
  this->set_red(global_preferences.get_float(friendly_name, "red", this->get_red()));
  this->set_green(global_preferences.get_float(friendly_name, "green", this->get_green()));
  this->set_blue(global_preferences.get_float(friendly_name, "blue", this->get_blue()));
  this->set_white(global_preferences.get_float(friendly_name, "white", this->get_white()));
}

void LightColorValues::save_to_preferences(const std::string &friendly_name) const {
  global_preferences.put_float(friendly_name, "state", this->get_state());
  global_preferences.put_float(friendly_name, "brightness", this->get_brightness());
  global_preferences.put_float(friendly_name, "red", this->get_red());
  global_preferences.put_float(friendly_name, "green", this->get_green());
  global_preferences.put_float(friendly_name, "blue", this->get_blue());
  global_preferences.put_float(friendly_name, "white", this->get_white());
}

void LightColorValues::parse_json(const JsonObject &root) {
  ESP_LOGV(TAG, "Parsing light color values JSON.");
  if (root.containsKey("state")) {
    auto val = parse_on_off(root["state"]);
    if (val.defined) {
      this->set_state(val.value ? 1.0 : 0.0);
      ESP_LOGV(TAG, "    state=%s", val.value ? "ON" : "OFF");
    }
  }

  if (root.containsKey("brightness")) {
    this->set_brightness(float(root["brightness"]) / 255.0f);
    ESP_LOGV(TAG, "    brightness=%.2f", this->get_brightness());
  }

  if (root.containsKey("color")) {
    JsonObject &color = root["color"];
    if (color.containsKey("r") && color.containsKey("g") && color.containsKey("b")) {
      this->set_red(float(color["r"]) / 255.0f);
      this->set_green(float(color["g"]) / 255.0f);
      this->set_blue(float(color["b"]) / 255.0f);
      ESP_LOGV(TAG, "    r=%.2f, g=%.2f, b=%.2f", this->get_red(), this->get_green(), this->get_blue());
    }
  }

  if (root.containsKey("white_value")) {
    this->white_ = float(root["white_value"]) / 255.0f;
    ESP_LOGV(TAG, "    white_value=%.2f", this->get_white());
  }
}

void LightColorValues::normalize_color(const LightTraits &traits) {
  if (traits.has_rgb()) {
    float max_value = fmaxf(this->get_red(), fmaxf(this->get_green(), this->get_blue()));
    if (traits.has_rgb_white_value()) {
      max_value = fmaxf(max_value, this->get_white());
      this->set_white(this->get_white() / max_value);
    }
    this->set_red(this->get_red() / max_value);
    this->set_green(this->get_green() / max_value);
    this->set_blue(this->get_blue() / max_value);
  }
}

void LightColorValues::dump_json(JsonObject &root, const LightTraits &traits) const {
  root["state"] = (this->get_state() != 0.0f) ? "ON" : "OFF";
  if (traits.has_brightness())
    root["brightness"] = uint8_t(this->get_brightness() * 255);
  if (traits.has_rgb()) {
    JsonObject &color = root.createNestedObject("color");
    color["r"] = uint8_t(this->get_red() * 255);
    color["g"] = uint8_t(this->get_green() * 255);
    color["b"] = uint8_t(this->get_blue() * 255);
  }
  if (traits.has_rgb_white_value())
    root["white_value"] = uint8_t(this->get_white() * 255);
}

bool LightColorValues::operator==(const LightColorValues &rhs) const {
  return state_ == rhs.state_ &&
      brightness_ == rhs.brightness_ &&
      red_ == rhs.red_ &&
      green_ == rhs.green_ &&
      blue_ == rhs.blue_ &&
      white_ == rhs.white_;
}

bool LightColorValues::operator!=(const LightColorValues &rhs) const {
  return !(rhs == *this);
}
void LightColorValues::as_rgbw(float *red, float *green, float *blue, float *white) const {
  this->as_rgb(red, green, blue);
  *white = this->state_ * this->brightness_ * this->white_;
}
void LightColorValues::as_rgb(float *red, float *green, float *blue) const {
  *red = this->state_ * this->brightness_ * this->red_;
  *green = this->state_ * this->brightness_ * this->green_;
  *blue = this->state_ * this->brightness_ * this->blue_;
}
void LightColorValues::as_brightness(float *brightness) const {
  *brightness = this->state_ * this->brightness_;
}
void LightColorValues::as_binary(bool *binary) const {
  *binary = this->state_ == 1.0f;
}

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT
