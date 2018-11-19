#include "esphomelib/defines.h"

#ifdef USE_COVER

#include "esphomelib/cover/mqtt_cover_component.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace cover {

static const char *TAG = "cover.mqtt";

MQTTCoverComponent::MQTTCoverComponent(Cover *cover) : cover_(cover) {}
void MQTTCoverComponent::setup() {
  this->cover_->add_on_publish_state_callback([this](CoverState state) {
    this->publish_state(state);
  });
  this->subscribe(this->get_command_topic(), [this](const std::string &topic, const std::string &payload) {
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
  if (this->cover_->optimistic()) {
    ESP_LOGCONFIG(TAG, "  Optimistic: YES");
  }
  LOG_MQTT_COMPONENT(true, true)
}
void MQTTCoverComponent::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (this->cover_->optimistic())
    root["optimistic"] = true;
}

std::string MQTTCoverComponent::component_type() const {
  return "cover";
}
std::string MQTTCoverComponent::friendly_name() const {
  return this->cover_->get_name();
}
void MQTTCoverComponent::send_initial_state() {
  if (this->cover_->has_state())
    this->publish_state(this->cover_->state);
}
bool MQTTCoverComponent::is_internal() {
  return this->cover_->is_internal();
}
void MQTTCoverComponent::publish_state(cover::CoverState state) {
  const char *state_s;
  switch (state) {
    case COVER_OPEN: state_s = "open"; break;
    case COVER_CLOSED: state_s = "closed"; break;
    default: {
      ESP_LOGW(TAG, "Unknown cover state.");
      return;
    }
  }
  ESP_LOGD(TAG, "'%s': Sending state %s", this->friendly_name().c_str(), state_s);
  this->send_message(this->get_state_topic(), state_s);
}

} // namespace cover

ESPHOMELIB_NAMESPACE_END

#endif //USE_COVER
