#include "esphomelib/defines.h"

#ifdef USE_TEXT_SENSOR

#include "esphomelib/text_sensor/mqtt_text_sensor.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

static const char *TAG = "text_sensor.mqtt";

MQTTTextSensor::MQTTTextSensor(TextSensor *sensor) : MQTTComponent(), sensor_(sensor) {
}
void MQTTTextSensor::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (!this->sensor_->get_icon().empty())
    root["icon"] = this->sensor_->get_icon();

  if (!this->sensor_->unique_id().empty())
    root["unique_id"] = this->sensor_->unique_id();

  config.command_topic = false;
}
void MQTTTextSensor::setup() {
  auto f = std::bind(&MQTTTextSensor::publish_state, this, std::placeholders::_1);
  this->sensor_->add_on_state_callback(f);
}

void MQTTTextSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "MQTT Text Sensor '%s':", this->sensor_->get_name().c_str());
  if (!this->sensor_->get_icon().empty()) {
    ESP_LOGCONFIG(TAG, "  Icon: '%s'", this->sensor_->get_icon().c_str());
  }
  if (!this->sensor_->unique_id().empty()) {
    ESP_LOGCONFIG(TAG, "  Unique ID: '%s'", this->sensor_->unique_id().c_str());
  }
  LOG_MQTT_COMPONENT(true, false);
}

void MQTTTextSensor::publish_state(const std::string &value) {
  this->send_message(this->get_state_topic(), value);
}
void MQTTTextSensor::send_initial_state() {
  if (this->sensor_->has_state())
    this->publish_state(this->sensor_->state);
}
bool MQTTTextSensor::is_internal() {
  return this->sensor_->is_internal();
}
std::string MQTTTextSensor::component_type() const {
  return "sensor";
}
std::string MQTTTextSensor::friendly_name() const {
  return this->sensor_->get_name();
}

}  // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif  // USE_TEXT_SENSOR
