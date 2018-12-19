#ifndef ESPHOMELIB_MQTT_TEXT_SENSOR_H
#define ESPHOMELIB_MQTT_TEXT_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_TEXT_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/text_sensor/text_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

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

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEXT_SENSOR

#endif //ESPHOMELIB_MQTT_TEXT_SENSOR_H
