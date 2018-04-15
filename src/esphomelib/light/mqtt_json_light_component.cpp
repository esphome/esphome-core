//
// Created by Otto Winter on 28.11.17.
//

#include "esphomelib/light/mqtt_json_light_component.h"

#include "esphomelib/log.h"

#ifdef USE_LIGHT

namespace esphomelib {

namespace light {

static const char *TAG = "light.mqtt_json";

std::string MQTTJSONLightComponent::component_type() const {
  return "light";
}

void MQTTJSONLightComponent::setup() {
  ESP_LOGD(TAG, "Setting up MQTT light...");

  LightColorValues recovered_values;
  recovered_values.load_from_preferences(this->state_->get_name());
  this->state_->set_immediately(recovered_values);

  this->subscribe_json(this->get_command_topic(), [&](JsonObject &root) {
    this->state_->parse_json(root);
  });

  this->state_->add_send_callback([&]() {
    this->next_send_ = true;
  });
}

MQTTJSONLightComponent::MQTTJSONLightComponent(LightState *state)
    : MQTTComponent(), state_(state) {
  assert(state != nullptr);
}

void MQTTJSONLightComponent::send_light_values() {
  LightColorValues remote_values = this->state_->get_remote_values();
  remote_values.save_to_preferences(this->state_->get_name());
  this->send_json_message(this->get_state_topic(), [&](JsonBuffer &buffer, JsonObject &root) {
    this->state_->dump_json(buffer, root);
  });
}
LightState *MQTTJSONLightComponent::get_state() const {
  return this->state_;
}
void MQTTJSONLightComponent::loop() {
  if (this->next_send_) {
    this->next_send_ = false;
    this->send_light_values();
  }
}
std::string MQTTJSONLightComponent::friendly_name() const {
  return this->state_->get_name();
}
void MQTTJSONLightComponent::send_discovery(JsonBuffer &buffer, JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (this->state_->get_traits().supports_brightness())
    root["brightness"] = true;
  if (this->state_->get_traits().supports_rgb())
    root["rgb"] = true;
  root["flash"] = true;
  if (this->state_->get_traits().has_rgb_white_value())
    root["white_value"] = true;
  if (this->state_->supports_effects()) {
    root["effect"] = true;
    JsonArray &effect_list = root.createNestedArray("effect_list");
    for (const LightEffect::Entry &entry : light_effect_entries) {
      if (!this->state_->get_traits().supports_traits(entry.requirements))
        continue;
      effect_list.add(entry.name);
    }
  }
  config.platform = "mqtt_json";
}

} // namespace light

} // namespace esphomelib

#endif //USE_LIGHT
