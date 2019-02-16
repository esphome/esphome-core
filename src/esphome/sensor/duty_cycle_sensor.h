#ifndef ESPHOME_DUTY_CYCLE_SENSOR_H
#define ESPHOME_DUTY_CYCLE_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_DUTY_CYCLE_SENSOR

#include "esphome/sensor/sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class DutyCycleSensor : public PollingSensorComponent {
 public:
  DutyCycleSensor(const std::string &name, GPIOPin *pin, uint32_t update_interval = 60000);

  void setup() override;
  float get_setup_priority() const override;
  void dump_config() override;
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
};

extern DutyCycleSensor *duty_cycle_sensors;

} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_DUTY_CYCLE_SENSOR

#endif //ESPHOME_DUTY_CYCLE_SENSOR_H
