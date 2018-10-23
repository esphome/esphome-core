#ifndef ESPHOMELIB_SENSOR_MQTT_SUBSCRIBE_SENSOR_H
#define ESPHOMELIB_SENSOR_MQTT_SUBSCRIBE_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_MQTT_SUBSCRIBE_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/sensor/sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

class MQTTSubscribeSensor : public Sensor, public Component {
 public:
  MQTTSubscribeSensor(const std::string &name, std::string topic);

  void setup() override;
  float get_setup_priority() const override;

  void set_qos(uint8_t qos);

 protected:
  std::string topic_;
  uint8_t qos_{0};
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_MQTT_SUBSCRIBE_SENSOR

#endif //ESPHOMELIB_SENSOR_MQTT_SUBSCRIBE_SENSOR_H
