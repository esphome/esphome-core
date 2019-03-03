#ifndef ESPHOME_MQTT_TEXT_SENSOR_H
#define ESPHOME_MQTT_TEXT_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_MQTT_TEXT_SENSOR

#include "esphome/component.h"
#include "esphome/mqtt/mqtt_component.h"
#include "esphome/text_sensor/text_sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace text_sensor {

class MQTTTextSensor : public mqtt::MQTTComponent {
 public:
  explicit MQTTTextSensor(TextSensor *sensor);

  void send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) override;

  void setup() override;

  void dump_config() override;

  bool publish_state(const std::string &value);

  bool send_initial_state() override;

  bool is_internal() override;

 protected:
  std::string component_type() const override;

  std::string friendly_name() const override;

  TextSensor *sensor_;
};

}  // namespace text_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_TEXT_SENSOR

#endif  // ESPHOME_MQTT_TEXT_SENSOR_H
