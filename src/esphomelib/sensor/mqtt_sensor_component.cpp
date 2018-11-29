#include "esphomelib/defines.h"

#ifdef USE_SENSOR

#include "esphomelib/sensor/mqtt_sensor_component.h"

#include "esphomelib/espmath.h"
#include "esphomelib/log.h"
#include "esphomelib/component.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.mqtt";

MQTTSensorComponent::MQTTSensorComponent(Sensor *sensor)
    : MQTTComponent(), sensor_(sensor) {

}

void MQTTSensorComponent::setup() {
  this->sensor_->add_on_state_callback([this](float state) {
    this->publish_state(state);
  });
}

void MQTTSensorComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "MQTT Sensor '%s':", this->sensor_->get_name().c_str());
  if (this->get_expire_after() > 0) {
    ESP_LOGCONFIG(TAG, "  Expire After: %us", this->get_expire_after() / 1000);
  }
  ESP_LOGCONFIG(TAG, "  Unit of Measurement: '%s'", this->sensor_->get_unit_of_measurement().c_str());
  ESP_LOGCONFIG(TAG, "  Accuracy Decimals: %d", this->sensor_->get_accuracy_decimals());
  if (!this->sensor_->get_icon().empty()) {
    ESP_LOGCONFIG(TAG, "  Icon: '%s'", this->sensor_->get_icon().c_str());
  }
  if (!this->sensor_->unique_id().empty()) {
    ESP_LOGCONFIG(TAG, "  Unique ID: '%s'", this->sensor_->unique_id().c_str());
  }
  LOG_MQTT_COMPONENT(true, false)
}

std::string MQTTSensorComponent::component_type() const {
  return "sensor";
}

uint32_t MQTTSensorComponent::get_expire_after() const {
  if (this->expire_after_.has_value()) {
    return *this->expire_after_;
  } else {
    return this->sensor_->calculate_expected_filter_update_interval() * 3;
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
void MQTTSensorComponent::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (!this->sensor_->get_unit_of_measurement().empty())
    root["unit_of_measurement"] = this->sensor_->get_unit_of_measurement();

  if (this->get_expire_after() > 0)
    root["expire_after"] = this->get_expire_after() / 1000;

  if (!this->sensor_->get_icon().empty())
    root["icon"] = this->sensor_->get_icon();

  config.command_topic = false;
}
bool MQTTSensorComponent::send_initial_state() {
  if (this->sensor_->has_state()) {
    return this->publish_state(this->sensor_->state);
  } else {
    return true;
  }
}
bool MQTTSensorComponent::is_internal() {
  return this->sensor_->is_internal();
}
bool MQTTSensorComponent::publish_state(float value) {
  int8_t accuracy = this->sensor_->get_accuracy_decimals();
  return this->publish(this->get_state_topic(), value_accuracy_to_string(value, accuracy));
}
std::string MQTTSensorComponent::unique_id() {
  return this->sensor_->unique_id();
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_SENSOR
