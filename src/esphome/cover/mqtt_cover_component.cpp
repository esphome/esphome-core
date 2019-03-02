#include "esphome/defines.h"

#ifdef USE_MQTT_COVER

#include "esphome/cover/mqtt_cover_component.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace cover {

static const char *TAG = "cover.mqtt";

MQTTCoverComponent::MQTTCoverComponent(Cover *cover) : cover_(cover) {}
void MQTTCoverComponent::setup() {
  this->cover_->add_on_publish_state_callback([this](CoverState state) { this->publish_state(state); });
  this->subscribe(this->get_command_topic_(), [this](const std::string &topic, const std::string &payload) {
    if (strcasecmp(payload.c_str(), "OPEN") == 0) {
      ESP_LOGD(TAG, "'%s': Opening cover...", this->friendly_name().c_str());
      this->cover_->open();
    } else if (strcasecmp(payload.c_str(), "CLOSE") == 0) {
      ESP_LOGD(TAG, "'%s': Closing cover...", this->friendly_name().c_str());
      this->cover_->close();
    } else if (strcasecmp(payload.c_str(), "STOP") == 0) {
      ESP_LOGD(TAG, "'%s': Stopping cover...", this->friendly_name().c_str());
      this->cover_->stop();
    } else {
      ESP_LOGW(TAG, "'%s': Received unknown payload '%s'...", this->friendly_name().c_str(), payload.c_str());
    }
  });
}

void MQTTCoverComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "MQTT cover '%s':", this->cover_->get_name().c_str());
  LOG_MQTT_COMPONENT(true, true)
}
void MQTTCoverComponent::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (this->cover_->assumed_state())
    root["optimistic"] = true;
}

std::string MQTTCoverComponent::component_type() const { return "cover"; }
std::string MQTTCoverComponent::friendly_name() const { return this->cover_->get_name(); }
bool MQTTCoverComponent::send_initial_state() {
  if (this->cover_->has_state()) {
    return this->publish_state(this->cover_->state);
  } else {
    return true;
  }
}
bool MQTTCoverComponent::is_internal() { return this->cover_->is_internal(); }
bool MQTTCoverComponent::publish_state(cover::CoverState state) {
  const char *state_s;
  switch (state) {
    case COVER_OPEN:
      state_s = "open";
      break;
    case COVER_CLOSED:
      state_s = "closed";
      break;
    default: {
      ESP_LOGW(TAG, "Unknown cover state.");
      return true;
    }
  }
  ESP_LOGD(TAG, "'%s': Sending state %s", this->friendly_name().c_str(), state_s);
  return this->publish(this->get_state_topic_(), state_s);
}

}  // namespace cover

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_COVER
