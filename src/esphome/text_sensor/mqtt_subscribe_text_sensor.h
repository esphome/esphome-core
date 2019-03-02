#ifndef ESPHOME_MQTT_SUBSCRIBE_TEXT_SENSOR_H
#define ESPHOME_MQTT_SUBSCRIBE_TEXT_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_MQTT_SUBSCRIBE_TEXT_SENSOR

#include "esphome/component.h"
#include "esphome/text_sensor/text_sensor.h"
#include "esphome/mqtt/mqtt_client_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace text_sensor {

class MQTTSubscribeTextSensor : public TextSensor, public Component {
 public:
  MQTTSubscribeTextSensor(const std::string &name, std::string topic);

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void set_qos(uint8_t qos);

 protected:
  std::string topic_;
  uint8_t qos_{0};
};

}  // namespace text_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_SUBSCRIBE_TEXT_SENSOR

#endif  // ESPHOME_MQTT_SUBSCRIBE_TEXT_SENSOR_H
