#ifndef ESPHOME_UPTIME_SENSOR_H
#define ESPHOME_UPTIME_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_UPTIME_SENSOR

#include "esphome/sensor/sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class UptimeSensor : public PollingSensorComponent {
 public:
  explicit UptimeSensor(const std::string &name, uint32_t update_interval = 60000);

  void update() override;

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;
  std::string unique_id() override;

  float get_setup_priority() const override;

 protected:
  uint64_t uptime_{0};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_UPTIME_SENSOR

#endif  // ESPHOME_UPTIME_SENSOR_H
