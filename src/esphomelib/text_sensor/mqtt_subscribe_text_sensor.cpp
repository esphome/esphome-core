#include "esphomelib/defines.h"

#ifdef USE_MQTT_SUBSCRIBE_TEXT_SENSOR

#include "esphomelib/text_sensor/mqtt_subscribe_text_sensor.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

static const char *TAG = "text_sensor.mqtt_subscribe";

MQTTSubscribeTextSensor::MQTTSubscribeTextSensor(const std::string &name, std::string topic)
    : TextSensor(name), topic_(std::move(topic)) {

}
void MQTTSubscribeTextSensor::setup() {
  mqtt::global_mqtt_client->subscribe(this->topic_, [this](std::string payload) {
    this->publish_state(payload);
  }, this->qos_);
}
float MQTTSubscribeTextSensor::get_setup_priority() const {
  return setup_priority::MQTT_CLIENT - 1.0f;
}
void MQTTSubscribeTextSensor::set_qos(uint8_t qos) {
  this->qos_ = qos;
}
void MQTTSubscribeTextSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "MQTT Subscribe Text Sensor '%s':", this->name_.c_str());
  ESP_LOGCONFIG(TAG, "  Topic: %s", this->topic_.c_str());
}

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_MQTT_SUBSCRIBE_TEXT_SENSOR
