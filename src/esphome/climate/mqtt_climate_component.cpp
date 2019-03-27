#include "esphome/defines.h"

#ifdef USE_MQTT_CLIMATE

#include "esphome/climate/mqtt_climate_component.h"
#include "esphome/climate/climate_device.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace climate {

static const char *TAG = "climate.mqtt";

void MQTTClimateComponent::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  auto traits = this->device_->get_traits();
  // current_temperature_topic
  if (traits.get_supports_current_temperature()) {
    root["current_temperature_topic"] = this->get_current_temperature_state_topic();
  }
  // mode_command_topic
  root["mode_command_topic"] = this->get_mode_command_topic();
  // mode_state_topic
  root["mode_state_topic"] = this->get_mode_state_topic();
  // modes
  JsonArray &modes = root.createNestedArray("modes");
  modes.add("off");
  if (traits.supports_mode(CLIMATE_MODE_AUTO))
    modes.add("auto");
  if (traits.supports_mode(CLIMATE_MODE_HEAT))
    modes.add("heat");
  if (traits.supports_mode(CLIMATE_MODE_COOL))
    modes.add("cool");

  if (traits.get_supports_two_point_target_temperature()) {
    // TODO: Implement two point target temperature support for MQTT platform in HA
    // https://github.com/home-assistant/home-assistant/blob/dev/homeassistant/components/climate/__init__.py#L233
  } else {
    // temperature_command_topic
    root["temperature_command_topic"] = this->get_target_temperature_command_topic();
    // temperature_state_topic
    root["temperature_state_topic"] = this->get_target_temperature_state_topic();
  }

  // min_temp
  root["min_temp"] = traits.get_visual_min_temperature();
  // max_temp
  root["max_temp"] = traits.get_visual_max_temperature();
  // temp_step
  root["temp_step"] = traits.get_visual_temperature_step();

  // TODO: Implement precision support for HA MQTT climate
  // https://github.com/home-assistant/home-assistant/blob/dev/homeassistant/components/climate/__init__.py#L204

  // TODO: away mode
}
void MQTTClimateComponent::setup() {
  this->subscribe(this->get_mode_command_topic(), [this](const std::string &topic, const std::string &payload) {
    auto call = this->device_->make_call();
    call.set_mode(payload);
    call.perform();
  });

  this->subscribe(this->get_target_temperature_command_topic(),
                  [this](const std::string &topic, const std::string &payload) {
                    auto val = parse_float(payload);
                    if (!val.has_value()) {
                      ESP_LOGW(TAG, "Can't convert '%s' to number!", payload.c_str());
                      return;
                    }
                    auto call = this->device_->make_call();
                    call.set_target_temperature(*val);
                    call.perform();
                  });

  // TODO: two point target temperature support
  this->device_->add_on_state_callback([this]() { this->publish_state_(); });

  // TODO: awy mode
}
MQTTClimateComponent::MQTTClimateComponent(ClimateDevice *device) : device_(device) {}
bool MQTTClimateComponent::send_initial_state() { return this->publish_state_(); }
bool MQTTClimateComponent::is_internal() { return this->device_->is_internal(); }
std::string MQTTClimateComponent::component_type() const { return "climate"; }
std::string MQTTClimateComponent::friendly_name() const { return this->device_->get_name(); }
bool MQTTClimateComponent::publish_state_() {
  auto traits = this->device_->get_traits();
  // mode
  const char *mode_s = climate_mode_to_string(this->device_->mode);
  bool failed = !this->publish(this->get_mode_state_topic(), mode_s);
  // TODO: HA rounds this value itself, we probably don't need to round it ourselves
  // https://github.com/home-assistant/home-assistant/blob/dev/homeassistant/components/climate/__init__.py#L215
  int8_t accuracy = traits.get_temperature_accuracy_decimals();
  if (traits.get_supports_current_temperature()) {
    std::string payload = value_accuracy_to_string(this->device_->current_temperature, accuracy);
    bool success = this->publish(this->get_current_temperature_state_topic(), payload);
    failed = failed || !success;
  }
  if (traits.get_supports_two_point_target_temperature()) {
    // TODO: Add two point target temp support to HA MQTT climate platform
  } else {
    std::string payload = value_accuracy_to_string(this->device_->target_temperature, accuracy);
    bool success = this->publish(this->get_target_temperature_state_topic(), payload);
    failed = failed || !success;
  }

  // TODO: away mode

  return !failed;
}
void MQTTClimateComponent::set_custom_current_temperature_state_topic(const std::string &custom_current_temperature_state_topic) {
  custom_current_temperature_state_topic_ = custom_current_temperature_state_topic;
}
void MQTTClimateComponent::set_custom_mode_state_topic(const std::string &custom_mode_state_topic) {
  custom_mode_state_topic_ = custom_mode_state_topic;
}
void MQTTClimateComponent::set_custom_mode_command_topic(const std::string &custom_mode_command_topic) {
  custom_mode_command_topic_ = custom_mode_command_topic;
}
void MQTTClimateComponent::set_custom_target_temperature_state_topic(const std::string &custom_target_temperature_state_topic) {
  custom_target_temperature_state_topic_ = custom_target_temperature_state_topic;
}
void MQTTClimateComponent::set_custom_target_temperature_command_topic(const std::string &custom_target_temperature_command_topic) {
  custom_target_temperature_command_topic_ = custom_target_temperature_command_topic;
}
const std::string MQTTClimateComponent::get_current_temperature_state_topic() const {
  if (this->custom_current_temperature_state_topic_.empty())
    return this->get_default_topic_for_("current_temperature/state");
  return this->custom_current_temperature_state_topic_;
}
const std::string MQTTClimateComponent::get_mode_state_topic() const {
  if (this->custom_mode_state_topic_.empty())
    return this->get_default_topic_for_("mode/state");
  return this->custom_mode_state_topic_;
}
const std::string MQTTClimateComponent::get_mode_command_topic() const {
  if (this->custom_mode_command_topic_.empty())
    return this->get_default_topic_for_("mode/command");
  return this->custom_mode_command_topic_;
}
const std::string MQTTClimateComponent::get_target_temperature_state_topic() const {
  if (this->custom_target_temperature_state_topic_.empty())
    return this->get_default_topic_for_("target_temperature/state");
  return this->custom_target_temperature_state_topic_;
}
const std::string MQTTClimateComponent::get_target_temperature_command_topic() const {
  if (this->custom_target_temperature_command_topic_.empty())
    return this->get_default_topic_for_("target_temperature/command");
  return this->custom_target_temperature_command_topic_;
}

}  // namespace climate

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_CLIMATE
