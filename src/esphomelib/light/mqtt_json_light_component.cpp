//
// Created by Otto Winter on 28.11.17.
//

#include "esphomelib/light/mqtt_json_light_component.h"

#include "esphomelib/log.h"

namespace esphomelib {

namespace light {

static const char *TAG = "light::mqtt_json";

std::string MQTTJSONLightComponent::component_type() const {
  return "light";
}

void MQTTJSONLightComponent::setup() {
  assert(this->state_ != nullptr);
  ESP_LOGD(TAG, "Setting up MQTT light...");

  this->send_discovery([&](JsonBuffer &buffer, JsonObject &root) {
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
  }, true, true, "mqtt_json");

  LightColorValues recovered_values;
  recovered_values.load_from_preferences(this->friendly_name_);
  this->state_->set_immediately(recovered_values);

  this->subscribe_json(this->get_command_topic(), [&](JsonObject &root) {
    this->parse_light_json(root);
  });

  this->state_->add_send_callback([&]() {
    this->next_send_ = true;
  });
}

void MQTTJSONLightComponent::parse_light_json(const JsonObject &root) {
  assert_setup(this);
  ESP_LOGV(TAG, "Interpreting light JSON.");
  LightColorValues v = this->state_->get_remote_values(); // use remote values for fallback
  v.parse_json(root);
  v.normalize_color(this->state_->get_traits());

  if (root.containsKey("flash")) {
    auto length = uint32_t(float(root["flash"]) * 1000);
    ESP_LOGD(TAG, "Starting flash with length=%u ms", length);
    this->state_->start_flash(v, length);

    // Flashes can occur during effects, so don't stop the current effect.
  } else if (root.containsKey("transition")) {
    auto length = uint32_t(float(root["transition"]) * 1000);
    ESP_LOGD(TAG, "Starting transition with length=%u ms", length);
    this->state_->start_transition(v, length);

    // Stop the current effect if transitioning to off.
    if (v.get_state() == 0.0f)
      this->state_->stop_effect();
  } else if (root.containsKey("effect")) {
    const char *effect = root["effect"];
    ESP_LOGD(TAG, "Starting effect '%s'", effect);
    this->state_->start_effect(effect);
  } else {
    uint32_t length = this->default_transition_length_;
    ESP_LOGD(TAG, "Starting default transition with length=%u ms", length);
    this->state_->start_transition(v, length);

    // Stop the current effect if requesting turn_off.
    if (v.get_state() == 0.0f)
      this->state_->stop_effect();
  }
}

MQTTJSONLightComponent::MQTTJSONLightComponent(std::string friendly_name)
    : MQTTComponent(std::move(friendly_name)), state_(nullptr), default_transition_length_(1000),
      next_send_(true) {

}

void MQTTJSONLightComponent::send_light_values() {
  assert_setup(this);
  LightColorValues remote_values = this->state_->get_remote_values();
  remote_values.save_to_preferences(this->friendly_name_);
  this->send_json_message(this->get_state_topic(), [&](JsonBuffer &buffer, JsonObject &root) {
    assert(this->state_ != nullptr);
    if (this->state_->supports_effects())
      root["effect"] = this->state_->get_effect_name();
    remote_values.dump_json(root, this->state_->get_traits());
  });
}

void MQTTJSONLightComponent::set_state(LightState *state) {
  this->state_ = state;
}

void MQTTJSONLightComponent::set_default_transition_length(uint32_t default_transition_length) {
  this->default_transition_length_ = default_transition_length;
}
uint32_t MQTTJSONLightComponent::get_default_transition_length() const {
  return this->default_transition_length_;
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

} // namespace light

} // namespace esphomelib
