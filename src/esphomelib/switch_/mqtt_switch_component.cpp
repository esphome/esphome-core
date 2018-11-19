#include "esphomelib/defines.h"

#ifdef USE_SWITCH

#include "esphomelib/switch_/mqtt_switch_component.h"

#include <utility>

#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.mqtt";

MQTTSwitchComponent::MQTTSwitchComponent(switch_::Switch *switch_)
    : MQTTComponent(), switch_(switch_) {

}

void MQTTSwitchComponent::setup() {
  this->subscribe(this->get_command_topic(), [this](const std::string &topic, const std::string &payload) {
    switch (parse_on_off(payload.c_str())) {
      case PARSE_ON:
        this->switch_->turn_on();
        break;
      case PARSE_OFF:
        this->switch_->turn_off();
        break;
      case PARSE_TOGGLE:
        this->switch_->toggle();
        break;
      case PARSE_NONE:
      default:
        ESP_LOGW(TAG, "'%s': Received unknown status payload: %s", this->friendly_name().c_str(), payload.c_str());
        this->status_momentary_warning("state", 5000);
        break;
    }
  });
  this->switch_->add_on_state_callback([this](bool enabled){
    this->defer([this, enabled]() {
      this->publish_state(enabled);
    });
  });
}
void MQTTSwitchComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "MQTT switch '%s': ", this->switch_->get_name().c_str());
  if (!this->switch_->get_icon().empty()) {
    ESP_LOGCONFIG(TAG, "  Icon: '%s'", this->switch_->get_icon().c_str());
  }
  if (this->switch_->optimistic()) {
    ESP_LOGCONFIG(TAG, "  Optimistic: YES");
  }
  LOG_MQTT_COMPONENT(true, true);
}

std::string MQTTSwitchComponent::component_type() const {
  return "switch";
}
void MQTTSwitchComponent::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (!this->switch_->get_icon().empty())
    root["icon"] = this->switch_->get_icon();
  if (this->switch_->optimistic())
    root["optimistic"] = true;
}
void MQTTSwitchComponent::send_initial_state() {
  this->publish_state(this->switch_->state);
}
bool MQTTSwitchComponent::is_internal() {
  return this->switch_->is_internal();
}
std::string MQTTSwitchComponent::friendly_name() const {
  return this->switch_->get_name();
}
void MQTTSwitchComponent::publish_state(bool state) {
  const char *state_s = state ? "ON" : "OFF";
  this->send_message(this->get_state_topic(), state_s);
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_SWITCH
