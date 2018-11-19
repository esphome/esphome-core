#include "esphomelib/defines.h"

#ifdef USE_FAN

#include "esphomelib/fan/mqtt_fan_component.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace fan {

static const char *TAG = "fan.mqtt";

MQTTFanComponent::MQTTFanComponent(FanState *state)
    : MQTTComponent(), state_(state) {

}

FanState *MQTTFanComponent::get_state() const {
  return this->state_;
}
std::string MQTTFanComponent::component_type() const {
  return "fan";
}
void MQTTFanComponent::setup() {
  this->subscribe(this->get_command_topic(), [this](const std::string &topic, const std::string &payload) {
    auto val = parse_on_off(payload.c_str());
    switch (val) {
      case PARSE_ON:
        ESP_LOGD(TAG, "'%s' Turning Fan ON.", this->friendly_name().c_str());
        this->state_->turn_on().perform();
        break;
      case PARSE_OFF:
        ESP_LOGD(TAG, "'%s' Turning Fan OFF.", this->friendly_name().c_str());
        this->state_->turn_off().perform();
        break;
      case PARSE_TOGGLE:
        ESP_LOGD(TAG, "'%s' Toggling Fan.", this->friendly_name().c_str());
        this->state_->toggle().perform();
        break;
      case PARSE_NONE:
      default:
        ESP_LOGW(TAG, "Unknown state payload %s", payload.c_str());
        this->status_momentary_warning("state", 5000);
        break;
    }
  });

  if (this->state_->get_traits().supports_oscillation()) {
    this->subscribe(this->get_oscillation_command_topic(), [this](const std::string &topic, const std::string &payload) {
      auto val = parse_on_off(payload.c_str(), "oscillate_on", "oscillate_off");
      switch (val) {
        case PARSE_ON:
          ESP_LOGD(TAG, "'%s': Setting oscillating ON", this->friendly_name().c_str());
          this->state_->make_call().set_oscillating(true).perform();
          break;
        case PARSE_OFF:
          ESP_LOGD(TAG, "'%s': Setting oscillating OFF", this->friendly_name().c_str());
          this->state_->make_call().set_oscillating(false).perform();
          break;
        case PARSE_TOGGLE:
          this->state_->make_call().set_oscillating(!this->state_->oscillating).perform();
          break;
        case PARSE_NONE:
          ESP_LOGW(TAG, "Unknown Oscillation Payload %s", payload.c_str());
          this->status_momentary_warning("oscillation", 5000);
          break;
      }
    });
  }

  if (this->state_->get_traits().supports_speed()) {
    this->subscribe(this->get_speed_command_topic(), [this](const std::string &topic, const std::string &payload) {
      this->state_->make_call().set_speed(payload.c_str()).perform();
    });
  }

  auto f = std::bind(&MQTTFanComponent::publish_state, this);
  this->state_->add_on_state_callback([this, f]() {
    this->defer("send", f);
  });
}
void MQTTFanComponent::set_custom_oscillation_command_topic(const std::string &topic) {
  this->custom_oscillation_command_topic_ = topic;
}
void MQTTFanComponent::set_custom_oscillation_state_topic(const std::string &topic) {
  this->custom_oscillation_state_topic_ = topic;
}
void MQTTFanComponent::set_custom_speed_command_topic(const std::string &topic) {
  this->custom_speed_command_topic_ = topic;
}
void MQTTFanComponent::set_custom_speed_state_topic(const std::string &topic) {
  this->custom_speed_state_topic_ = topic;
}
const std::string MQTTFanComponent::get_oscillation_command_topic() const {
  if (this->custom_oscillation_command_topic_.empty())
    return this->get_default_topic_for("oscillation/command");
  return this->custom_oscillation_command_topic_;
}
const std::string MQTTFanComponent::get_oscillation_state_topic() const {
  if (this->custom_oscillation_state_topic_.empty())
    return this->get_default_topic_for("oscillation/state");
  return this->custom_oscillation_state_topic_;
}
const std::string MQTTFanComponent::get_speed_command_topic() const {
  if (this->custom_speed_command_topic_.empty())
    return this->get_default_topic_for("speed/command");
  return this->custom_speed_command_topic_;
}
const std::string MQTTFanComponent::get_speed_state_topic() const {
  if (this->custom_speed_state_topic_.empty())
    return this->get_default_topic_for("speed/state");
  return this->custom_speed_state_topic_;
}
void MQTTFanComponent::send_initial_state() {
  this->publish_state();
}
std::string MQTTFanComponent::friendly_name() const {
  return this->state_->get_name();
}
void MQTTFanComponent::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (this->state_->get_traits().supports_oscillation()) {
    root["oscillation_command_topic"] = this->get_oscillation_command_topic();
    root["oscillation_state_topic"] = this->get_oscillation_state_topic();
  }
  if (this->state_->get_traits().supports_speed()) {
    root["speed_command_topic"] = this->get_speed_command_topic();
    root["speed_state_topic"] = this->get_speed_state_topic();
  }
}
bool MQTTFanComponent::is_internal() {
  return this->state_->is_internal();
}
void MQTTFanComponent::publish_state() {
  const char *state_s = this->state_->state ? "ON" : "OFF";
  ESP_LOGD(TAG, "'%s' Sending state %s.", this->state_->get_name().c_str(), state_s);
  this->send_message(this->get_state_topic(), state_s);
  if (this->state_->get_traits().supports_oscillation())
    this->send_message(this->get_oscillation_state_topic(),
                       this->state_->oscillating ? "oscillate_on" : "oscillate_off");
  if (this->state_->get_traits().supports_speed()) {
    const char *payload;
    switch (this->state_->speed) {
      case FAN_SPEED_LOW: {
        payload = "low";
        break;
      }
      case FAN_SPEED_MEDIUM: {
        payload = "medium";
        break;
      }
      default:
      case FAN_SPEED_HIGH: {
        payload = "high";
        break;
      }
    }
    this->send_message(this->get_speed_state_topic(), payload);
  }
}

} // namespace fan

ESPHOMELIB_NAMESPACE_END

#endif //USE_FAN
