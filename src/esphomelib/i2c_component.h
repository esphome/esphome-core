//
//  i2c_component.h
//  esphomelib
//
//  Created by Otto Winter on 05.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_I2C_COMPONENT_H
#define ESPHOMELIB_I2C_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/defines.h"

#ifdef USE_I2C

namespace esphomelib {

class I2CComponent : public Component {
 public:
  I2CComponent(uint8_t sda_pin, uint8_t scl_pin, bool scan = false);

  uint8_t get_sda_pin() const;
  void set_sda_pin(uint8_t sda_pin);
  uint8_t get_scl_pin() const;
  void set_scl_pin(uint8_t scl_pin);
  void set_scan(bool scan);

  void setup() override;
  void loop() override;
  float get_setup_priority() const override;

#ifdef ARDUINO_ARCH_ESP32
  float get_frequency() const;
  void set_frequency(float frequency);
#endif

 protected:
  uint8_t sda_pin_;
  uint8_t scl_pin_;
  bool scan_;
#ifdef ARDUINO_ARCH_ESP32
  float frequency_{1000.0f};
#endif
};

} // namespace esphomelib

#endif

#endif //ESPHOMELIB_I2C_COMPONENT_H
