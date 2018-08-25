//
//  duty_cycle_sensor.h
//  esphomelib
//
//  Created by Otto Winter on 09.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_DUTY_CYCLE_SENSOR_H
#define ESPHOMELIB_DUTY_CYCLE_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_DUTY_CYCLE_SENSOR

#include "esphomelib/sensor/sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

class DutyCycleSensor : public PollingSensorComponent {
 public:
  DutyCycleSensor(const std::string &name, GPIOPin *pin, uint32_t update_interval = 15000);

  void setup() override;
  void update() override;
  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

  void on_interrupt();

  static void gpio_intr();

 protected:
  GPIOPin *pin_;
  volatile uint32_t last_interrupt_{0};
  volatile uint32_t on_time_{0};
  volatile bool last_level_{false};
  DutyCycleSensor *next_;
  // Store the pin number locally, due to IRAM_ATTR
  uint8_t pin_number_;
  bool pin_inverted_;
};

extern DutyCycleSensor *duty_cycle_sensors;

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_DUTY_CYCLE_SENSOR

#endif //ESPHOMELIB_DUTY_CYCLE_SENSOR_H
