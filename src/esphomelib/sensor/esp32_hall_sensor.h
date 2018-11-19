#ifndef ESPHOMELIB_SENSOR_ESP32_HALL_SENSOR_H
#define ESPHOMELIB_SENSOR_ESP32_HALL_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_ESP32_HALL_SENSOR

#include "esphomelib/sensor/sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

class ESP32HallSensor : public PollingSensorComponent {
 public:
  explicit ESP32HallSensor(const std::string &name, uint32_t update_interval = 15000);

  void update() override;

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;
  std::string unique_id() override;
};

}  // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif  // USE_ESP32_HALL_SENSOR

#endif  // ESPHOMELIB_SENSOR_ESP32_HALL_SENSOR_H
