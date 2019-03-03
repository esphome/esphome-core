#ifndef ESPHOME_SENSOR_ESP32_HALL_SENSOR_H
#define ESPHOME_SENSOR_ESP32_HALL_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_ESP32_HALL_SENSOR

#include "esphome/sensor/sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class ESP32HallSensor : public PollingSensorComponent {
 public:
  explicit ESP32HallSensor(const std::string &name, uint32_t update_interval = 60000);

  void dump_config() override;

  void update() override;

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;
  std::string unique_id() override;
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_ESP32_HALL_SENSOR

#endif  // ESPHOME_SENSOR_ESP32_HALL_SENSOR_H
