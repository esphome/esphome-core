//
// Created by Otto Winter on 28.11.17.
//

#include "light_color_values.h"
#include <esphomelib/helpers.h>
#include <esphomelib/component.h>
#include <esp_log.h>
#include <sstream>
#include <iomanip>
#include <esphomelib/espmath.h>

namespace esphomelib {

namespace light {

static const char *TAG = "light_color_values";

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

void LightColorValues::load_from_preferences(Preferences *preferences) {
  if (preferences == nullptr)
    return;
  this->set_state(preferences->getFloat("state", this->get_state()));
  this->set_brightness(preferences->getFloat("brightness", this->get_brightness()));
  this->set_red(preferences->getFloat("red", this->get_red()));
  this->set_green(preferences->getFloat("green", this->get_green()));
  this->set_blue(preferences->getFloat("blue", this->get_blue()));
  this->set_white(preferences->getFloat("white", this->get_white()));
}

void LightColorValues::save_to_preferences(Preferences *preferences) const {
  if (preferences == nullptr)
    return;
  preferences->putFloat("state", this->get_state());
  preferences->putFloat("brightness", this->get_brightness());
  preferences->putFloat("red", this->get_red());
  preferences->putFloat("green", this->get_green());
  preferences->putFloat("blue", this->get_blue());
  preferences->putFloat("white", this->get_white());
}

void LightColorValues::parse_json(const JsonObject &root) {
  ESP_LOGV(TAG, "Parsing light color values JSON.");
  if (root.containsKey("state")) {
    if (strcasecmp(root["state"], "ON") == 0) {
      this->set_state(1.0f);
      ESP_LOGV(TAG, "    state=true");
    } else if (strcasecmp(root["state"], "OFF") == 0) {
      this->set_state(0.0f);
      ESP_LOGV(TAG, "    state=false");
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
    } else if (color.containsKey("x") && color.containsKey("y")) {
      auto x = float(color["x"]);
      auto y = float(color["y"]);
      this->from_xyz(x, y, this->get_brightness());
    }
  }

  if (root.containsKey("white_value")) {
    this->white_ = float(root["white_value"]) / 255.0f;
    ESP_LOGV(TAG, "    white_value=%.2f", this->get_white());
  }
}

void LightColorValues::from_xyz(float v_x, float v_y, float brightness) {
  ESP_LOGV(TAG, "From XYZ (x=%.2f, y=%.2f, z=%.2f)", v_x, v_y, brightness);
  this->set_brightness(brightness);
  if (this->get_brightness() == 0.0f) {
    this->set_red(0.0f);
    this->set_green(0.0f);
    this->set_blue(0.0f);
  }
  float Y = this->get_brightness();
  if (v_y == 0.0)
    v_y += 0.00000000001;

  float X = (Y / v_y) * v_x;
  float Z = (Y / v_y) * (1 - v_x - v_y);

  float r = X * 1.656492f - Y * 0.354851f - Z * 0.255038f;
  float g = -X * 0.707196f + Y * 1.655397f + Z * 0.036152f;
  float b = X * 0.051713f - Y * 0.121364f + Z * 1.011530f;

  r = (r <= 0.0031308f) ? (12.92f * r) : ((1.055f) * powf(r, (1.0f / 2.4f)) - 0.055f);
  g = (g <= 0.0031308f) ? (12.92f * g) : ((1.055f) * powf(g, (1.0f / 2.4f)) - 0.055f);
  b = (b <= 0.0031308f) ? (12.92f * b) : ((1.055f) * powf(b, (1.0f / 2.4f)) - 0.055f);

  if (r < 0.0f) r = 0.0f;
  if (g < 0.0f) g = 0.0f;
  if (b < 0.0f) b = 0.0f;

  float max_comp = fmaxf(r, fmaxf(g, b));
  if (max_comp > 1.0f) {
    this->set_red(r / max_comp);
    this->set_green(g / max_comp);
    this->set_blue(b / max_comp);
  }
  ESP_LOGV(TAG, "    r=%.2f, g=%.2f, b=%.2f", this->get_red(), this->get_green(), this->get_blue());
}

void LightColorValues::normalize_color(const LightTraits &traits) {
  if (traits.supports_rgb()) {
    float max_value = fmaxf(this->get_red(), fmaxf(this->get_green(), this->get_blue()));
    if (traits.supports_rgbw()) {
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
  if (traits.supports_brightness())
    root["brightness"] = uint8_t(this->get_brightness() * 255);
  if (traits.supports_rgb()) {
    JsonObject &color = root.createNestedObject("color");
    color["r"] = uint8_t(this->get_red() * 255);
    color["g"] = uint8_t(this->get_green() * 255);
    color["b"] = uint8_t(this->get_blue() * 255);
  }
  if (traits.supports_rgbw())
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
std::string LightColorValues::to_string() {
  std::ostringstream os;
  os << std::setprecision(2) << *this;
  return os.str();
}
std::ostream &operator<<(std::ostream &os, const LightColorValues &values) {
  os << "state: " << values.state_ << " brightness: " << values.brightness_ << " red: " << values.red_ << " green: "
     << values.green_ << " blue: " << values.blue_ << " white: " << values.white_;
  return os;
}

} // namespace light

} // namespace esphomelib
