//
//  esp32_touch_binary_sensor.h
//  esphomelib
//
//  Created by Otto Winter on 11.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_BINARY_SENSOR_ESP32_TOUCH_BINARY_SENSOR_H
#define ESPHOMELIB_BINARY_SENSOR_ESP32_TOUCH_BINARY_SENSOR_H

#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/defines.h"

#ifdef USE_ESP32_TOUCH_BINARY_SENSOR

namespace esphomelib {

namespace binary_sensor {

class ESP32TouchBinarySensor;

class ESP32TouchComponent : public Component {
 public:
  explicit ESP32TouchComponent();

  void setup() override;
  void loop() override;

  ESP32TouchBinarySensor *make_touch_pad(const std::string &name, touch_pad_t touch_pad, uint16_t threshold);

  void set_setup_mode(bool setup_mode);
  void set_iir_filter(uint32_t iir_filter);

  float get_setup_priority() const override;

 protected:
  inline bool iir_filter_enabled_() const;

  uint16_t sleep_cycle_{4096};
  uint16_t meas_cycle_{65535};
  touch_low_volt_t low_voltage_reference_{TOUCH_LVOLT_0V5};
  touch_high_volt_t high_voltage_reference_{TOUCH_HVOLT_2V7};
  touch_volt_atten_t voltage_attenuation_{TOUCH_HVOLT_ATTEN_0V};
  std::vector<ESP32TouchBinarySensor *> children_;
  bool setup_mode_{false};
  uint32_t iir_filter_{0};
};

class ESP32TouchBinarySensor : public BinarySensor {
 public:
  ESP32TouchBinarySensor(const std::string &name, touch_pad_t touch_pad, uint16_t threshold);

  touch_pad_t get_touch_pad() const;
  uint16_t get_threshold() const;

 protected:
  touch_pad_t touch_pad_;
  uint16_t threshold_;
};

} // namespace binary_sensor

} // namespace esphomelib

#endif //USE_ESP32_TOUCH_BINARY_SENSOR

#endif //ESPHOMELIB_BINARY_SENSOR_ESP32_TOUCH_BINARY_SENSOR_H
