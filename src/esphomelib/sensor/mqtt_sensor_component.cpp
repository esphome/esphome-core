//
// Created by Otto Winter on 26.11.17.
//

#include "esphomelib/sensor/mqtt_sensor_component.h"

#include "esphomelib/espmath.h"
#include "esphomelib/log.h"
#include "esphomelib/component.h"

#ifdef USE_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.mqtt";

MQTTSensorComponent::MQTTSensorComponent(Sensor *sensor)
    : MQTTComponent(), sensor_(sensor) {

}

void MQTTSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MQTT Sensor '%s'...", this->sensor_->get_name().c_str());
  if (this->get_expire_after() > 0) {
    ESP_LOGCONFIG(TAG, "    Expire After: %us", this->get_expire_after() / 1000);
  }
  ESP_LOGCONFIG(TAG, "    Unit of Measurement: '%s'", this->sensor_->get_unit_of_measurement().c_str());
  ESP_LOGCONFIG(TAG, "    Accuracy Decimals: %i", this->sensor_->get_accuracy_decimals());
  ESP_LOGCONFIG(TAG, "    Icon: '%s'", this->sensor_->get_icon().c_str());
  if (!this->sensor_->unique_id().empty()) {
    ESP_LOGCONFIG(TAG, "    Unique ID: '%s'", this->sensor_->unique_id().c_str());
  }

  this->sensor_->add_on_value_callback([this](float value) {
    this->publish_state(value);
  });
}

std::string MQTTSensorComponent::component_type() const {
  return "sensor";
}

uint32_t MQTTSensorComponent::get_expire_after() const {
  if (this->expire_after_.has_value()) {
    return *this->expire_after_;
  } else {
    uint32_t interval = this->sensor_->update_interval();
    Filter *filter = this->sensor_->filter_list_;
    while (filter != nullptr) {
      interval = filter->expected_interval(interval);
      filter = filter->next_;
    }
    return interval * 3;
  }
}
void MQTTSensorComponent::set_expire_after(uint32_t expire_after) {
  this->expire_after_ = expire_after;
}
void MQTTSensorComponent::disable_expire_after() {
  this->expire_after_ = 0;
}
std::string MQTTSensorComponent::friendly_name() const {
  return this->sensor_->get_name();
}
void MQTTSensorComponent::send_discovery(JsonBuffer &buffer, JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (!this->sensor_->get_unit_of_measurement().empty())
    root["unit_of_measurement"] = this->sensor_->get_unit_of_measurement();

  if (this->get_expire_after() > 0)
    root["expire_after"] = this->get_expire_after() / 1000;

  if (!this->sensor_->get_icon().empty())
    root["icon"] = this->sensor_->get_icon();

  if (!this->sensor_->unique_id().empty())
    root["unique_id"] = this->sensor_->unique_id();

  config.command_topic = false;
}
void MQTTSensorComponent::send_initial_state() {
  this->publish_state(this->sensor_->value);
}
bool MQTTSensorComponent::is_internal() {
  return this->sensor_->is_internal();
}
void MQTTSensorComponent::publish_state(float value) {
  int8_t accuracy = this->sensor_->get_accuracy_decimals();
  ESP_LOGD(TAG, "'%s': Pushing out value %f with %d decimals of accuracy",
           this->sensor_->get_name().c_str(), value, accuracy);
  this->send_message(this->get_state_topic(), value_accuracy_to_string(value, accuracy));
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_SENSOR
