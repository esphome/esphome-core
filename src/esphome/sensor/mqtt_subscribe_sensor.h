#ifndef ESPHOME_SENSOR_MQTT_SUBSCRIBE_SENSOR_H
#define ESPHOME_SENSOR_MQTT_SUBSCRIBE_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_MQTT_SUBSCRIBE_SENSOR

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class MQTTSubscribeSensor : public Sensor, public Component {
 public:
  MQTTSubscribeSensor(const std::string &name, std::string topic);

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void set_qos(uint8_t qos);

 protected:
  std::string topic_;
  uint8_t qos_{0};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_SUBSCRIBE_SENSOR

#endif  // ESPHOME_SENSOR_MQTT_SUBSCRIBE_SENSOR_H
