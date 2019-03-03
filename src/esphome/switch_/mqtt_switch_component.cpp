#include "esphome/defines.h"

#ifdef USE_MQTT_SWITCH

#include "esphome/switch_/mqtt_switch_component.h"

#include <utility>

#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.mqtt";

MQTTSwitchComponent::MQTTSwitchComponent(switch_::Switch *a_switch) : MQTTComponent(), switch_(a_switch) {}

void MQTTSwitchComponent::setup() {
  this->subscribe(this->get_command_topic_(), [this](const std::string &topic, const std::string &payload) {
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
  this->switch_->add_on_state_callback(
      [this](bool enabled) { this->defer("send", [this, enabled]() { this->publish_state(enabled); }); });
}
void MQTTSwitchComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "MQTT Switch '%s': ", this->switch_->get_name().c_str());
  LOG_MQTT_COMPONENT(true, true);
}

std::string MQTTSwitchComponent::component_type() const { return "switch"; }
void MQTTSwitchComponent::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (!this->switch_->get_icon().empty())
    root["icon"] = this->switch_->get_icon();
  if (this->switch_->assumed_state())
    root["optimistic"] = true;
}
bool MQTTSwitchComponent::send_initial_state() { return this->publish_state(this->switch_->state); }
bool MQTTSwitchComponent::is_internal() { return this->switch_->is_internal(); }
std::string MQTTSwitchComponent::friendly_name() const { return this->switch_->get_name(); }
bool MQTTSwitchComponent::publish_state(bool state) {
  const char *state_s = state ? "ON" : "OFF";
  return this->publish(this->get_state_topic_(), state_s);
}

}  // namespace switch_

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_SWITCH
