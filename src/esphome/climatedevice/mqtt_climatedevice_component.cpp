#include "esphome/defines.h"

#ifdef USE_MQTT_CLIMATEDEVICE

#include "esphome/climatedevice/mqtt_climatedevice_component.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace climatedevice {

static const char *TAG = "climatedevice.mqtt";

MQTTClimateDeviceComponent::MQTTClimateDeviceComponent(ClimateDevice *device) : MQTTComponent(), device_(device) {}

ClimateDevice *MQTTClimateDeviceComponent::get_device() const { return this->device_; }
std::string MQTTClimateDeviceComponent::component_type() const { return "climatedevice"; }
void MQTTClimateDeviceComponent::setup() {
  this->subscribe(this->get_mode_command_topic(), [this](const std::string &topic, const std::string &payload) {
    auto traits = this->device_->get_traits();
    if (strcasecmp(payload.c_str(), "off") == 0) {
      this->device_->make_call().set_mode(CLIMATEDEVICE_MODE_OFF).perform();
    } else if (traits.supports_auto_mode() && strcasecmp(payload.c_str(), "auto") == 0) {
      this->device_->make_call().set_mode(CLIMATEDEVICE_MODE_AUTO).perform();
    } else if (traits.supports_cool_mode() && strcasecmp(payload.c_str(), "cool") == 0) {
      this->device_->make_call().set_mode(CLIMATEDEVICE_MODE_COOL).perform();
    } else if (traits.supports_heat_mode() && strcasecmp(payload.c_str(), "heat") == 0) {
      this->device_->make_call().set_mode(CLIMATEDEVICE_MODE_HEAT).perform();
    } else {
      ESP_LOGW(TAG, "Unknown mode payload %s", payload.c_str());
      this->status_momentary_warning("mode", 5000);
    }
  });

  this->subscribe(this->get_target_temperature_state_topic(),
                  [this](const std::string &topic, const std::string &payload) {
                    auto val = parse_float(payload);
                    if (!val.has_value()) {
                      ESP_LOGW(TAG, "Can't convert '%s' to number!", payload.c_str());
                      return;
                    }
                    this->device_->make_call().set_target_temperature(*val).perform();
                  });

  auto f = std::bind(&MQTTClimateDeviceComponent::publish_state, this);
  this->device_->add_on_publish_state_callback([this, f]() { this->defer("send", f); });
}
void MQTTClimateDeviceComponent::set_custom_current_temperature_state_topic(const std::string &topic) {
  this->custom_current_temperature_state_topic_ = topic;
}
void MQTTClimateDeviceComponent::set_custom_mode_state_topic(const std::string &topic) {
  this->custom_mode_state_topic_ = topic;
}
void MQTTClimateDeviceComponent::set_custom_mode_command_topic(const std::string &topic) {
  this->custom_mode_command_topic_ = topic;
}
void MQTTClimateDeviceComponent::set_custom_target_temperature_state_topic(const std::string &topic) {
  this->custom_target_temperature_state_topic_ = topic;
}
void MQTTClimateDeviceComponent::set_custom_targer_temperature_command_topic(const std::string &topic) {
  this->custom_target_temperature_command_topic_ = topic;
}
const std::string MQTTClimateDeviceComponent::get_current_temperature_state_topic() const {
  if (this->custom_current_temperature_state_topic_.empty())
    return this->get_default_topic_for_("current_temperature/state");
  return this->custom_current_temperature_state_topic_;
}
const std::string MQTTClimateDeviceComponent::get_mode_state_topic() const {
  if (this->custom_mode_state_topic_.empty())
    return this->get_default_topic_for_("mode/state");
  return this->custom_mode_state_topic_;
}
const std::string MQTTClimateDeviceComponent::get_mode_command_topic() const {
  if (this->custom_mode_command_topic_.empty())
    return this->get_default_topic_for_("mode/command");
  return this->custom_mode_command_topic_;
}
const std::string MQTTClimateDeviceComponent::get_target_temperature_state_topic() const {
  if (this->custom_target_temperature_state_topic_.empty())
    return this->get_default_topic_for_("target_temperature/state");
  return this->custom_target_temperature_state_topic_;
}
const std::string MQTTClimateDeviceComponent::get_target_temperature_command_topic() const {
  if (this->custom_target_temperature_command_topic_.empty())
    return this->get_default_topic_for_("target_temperature/command");
  return this->custom_target_temperature_command_topic_;
}
bool MQTTClimateDeviceComponent::send_initial_state() { return this->publish_state(); }
std::string MQTTClimateDeviceComponent::friendly_name() const { return this->device_->get_name(); }
void MQTTClimateDeviceComponent::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  JsonArray &modes = root.createNestedArray("modes");
  auto traits = this->device_->get_traits();
  if (traits.supports_auto_mode()) {
    modes.add("auto");
  }
  if (traits.supports_cool_mode()) {
    modes.add("cool");
  }
  if (traits.supports_heat_mode()) {
    modes.add("heat");
  }
  modes.add("off");
  if (traits.supports_current_temperature()) {
    root["current_temperature_topic"] = this->get_current_temperature_state_topic();
  }
  root["mode_state_topic"] = this->get_mode_state_topic();
  root["mode_command_topic"] = this->get_mode_command_topic();
  root["temperature_state_topic"] = this->get_target_temperature_state_topic();
  root["temperature_command_topic"] = this->get_target_temperature_command_topic();
  if (!isnan(this->device_->get_target_temperature_initial())) {
    root["initial"] = this->device_->get_target_temperature_initial();
  }
  if (!isnan(traits.get_min_target_temperature())) {
    root["min_temp"] = traits.get_min_target_temperature();
  }
  if (!isnan(traits.get_max_target_temperature())) {
    root["max_temp"] = traits.get_max_target_temperature();
  }
  if (!isnan(this->device_->get_target_temperature_step())) {
    root["temp_step"] = this->device_->get_target_temperature_step();
  }
}
bool MQTTClimateDeviceComponent::is_internal() { return this->device_->is_internal(); }
bool MQTTClimateDeviceComponent::publish_state() {
  const char *mode_s = "off";
  switch (this->device_->state.mode) {
    case CLIMATEDEVICE_MODE_AUTO: {
      mode_s = "auto";
      break;
    }
    case CLIMATEDEVICE_MODE_COOL: {
      mode_s = "cool";
      break;
    }
    case CLIMATEDEVICE_MODE_HEAT: {
      mode_s = "heat";
      break;
    }
    default: {}
  }
  ESP_LOGD(TAG, "'%s' Sending mode %s.", this->device_->get_name().c_str(), mode_s);
  bool failed = !this->publish(this->get_mode_state_topic(), mode_s);
  if (this->device_->get_traits().supports_current_temperature()) {
    int8_t accuracy = this->device_->get_current_temperature_accuracy_decimals();
    float value = this->device_->current_temperature;
    bool success =
        this->publish(this->get_current_temperature_state_topic(), value_accuracy_to_string(value, accuracy));
    failed = failed || !success;
  }
  {
    int8_t accuracy = this->device_->get_target_temperature_accuracy_decimals();
    float value = this->device_->state.target_temperature;
    bool success = this->publish(this->get_target_temperature_state_topic(), value_accuracy_to_string(value, accuracy));
    failed = failed || !success;
  }

  return !failed;
}

}  // namespace climatedevice

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_CLIMATEDEVICE
