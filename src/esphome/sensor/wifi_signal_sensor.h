#ifndef ESPHOME_SENSOR_WIFI_SIGNAL_SENSOR_H
#define ESPHOME_SENSOR_WIFI_SIGNAL_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_WIFI_SIGNAL_SENSOR

#include "esphome/sensor/sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class WiFiSignalSensor : public PollingSensorComponent {
 public:
  explicit WiFiSignalSensor(const std::string &name, uint32_t update_interval = 60000);

  void update() override;
  void dump_config() override;

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;
  std::string unique_id() override;
  float get_setup_priority() const override;
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_WIFI_SIGNAL_SENSOR

#endif  // ESPHOME_SENSOR_WIFI_SIGNAL_SENSOR_H
