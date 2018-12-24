#ifndef ESPHOMELIB_SENSOR_WIFI_SIGNAL_SENSOR_H
#define ESPHOMELIB_SENSOR_WIFI_SIGNAL_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_WIFI_SIGNAL_SENSOR

#include "esphomelib/sensor/sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

class WiFiSignalSensor : public PollingSensorComponent {
 public:
  explicit WiFiSignalSensor(const std::string &name, uint32_t update_interval = 15000);

  void update() override;
  void dump_config() override;

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;
  std::string unique_id() override;
  float get_setup_priority() const override;
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_WIFI_SIGNAL_SENSOR

#endif //ESPHOMELIB_SENSOR_WIFI_SIGNAL_SENSOR_H
