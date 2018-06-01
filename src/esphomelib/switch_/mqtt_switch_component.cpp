//
// Created by Otto Winter on 02.12.17.
//

#include "esphomelib/switch_/mqtt_switch_component.h"

#include <utility>

#include "esphomelib/log.h"

#ifdef USE_SWITCH

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.mqtt";

using esphomelib::binary_sensor::binary_callback_t;

MQTTSwitchComponent::MQTTSwitchComponent(switch_::Switch *switch_)
    : MQTTBinarySensorComponent(switch_), switch_(switch_) {
  assert(this->switch_ != nullptr);
}

void MQTTSwitchComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MQTT switch '%s'", this->switch_->get_name().c_str());
  ESP_LOGCONFIG(TAG, "    Icon: '%s'", this->switch_->get_icon().c_str());
  if (this->switch_->optimistic()) {
    ESP_LOGCONFIG(TAG, "    Optimistic: YES");
  }

  this->subscribe(this->get_command_topic(), [&](const std::string &payload) {
    if (strcasecmp(payload.c_str(), this->get_payload_on().c_str()) == 0)
      this->turn_on();
    else if (strcasecmp(payload.c_str(), this->get_payload_off().c_str()) == 0)
      this->turn_off();
  });
  this->switch_->add_on_state_callback([this](bool enabled){
    this->publish_state(enabled);
  });

  this->publish_state(this->switch_->value);
}

std::string MQTTSwitchComponent::component_type() const {
  return "switch";
}
void MQTTSwitchComponent::turn_on() {
  ESP_LOGD(TAG, "'%s' Turning ON.", this->friendly_name().c_str());
  this->switch_->write_state(true);
}
void MQTTSwitchComponent::turn_off() {
  ESP_LOGD(TAG, "'%s' Turning OFF.", this->friendly_name().c_str());
  this->switch_->write_state(false);
}
void MQTTSwitchComponent::send_discovery(JsonBuffer &buffer, JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (!this->switch_->get_icon().empty())
    root["icon"] = this->switch_->get_icon();
  if (this->get_payload_on() != "ON")
    root["payload_on"] = this->get_payload_on();
  if (this->get_payload_off() != "OFF")
    root["payload_off"] = this->get_payload_off();
  if (this->switch_->optimistic())
    root["optimistic"] = true;
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_SWITCH
