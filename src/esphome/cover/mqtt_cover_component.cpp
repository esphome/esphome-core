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
  this->subscribe(this->position_command_topic(), [this](const std::string &topic, const std::string payload) {
      ESP_LOGD(TAG, "'%s': Setting position = %s", this->friendly_name().c_str(), payload.c_str());
      auto val = atof(payload.c_str());
      // TODO: Handle errors
      this->cover_->set_position(val);
      if (cover_->optimistic()) {
        this->publish_position(val);
        if (this->custom_command_topic_.empty()) { // if no open/closed command then position sets state
          if (val <= cover_->position_closed())
    	    this->publish_state(COVER_CLOSED);
          else
    	    this->publish_state(COVER_OPEN);
        }
      }
  });
  this->subscribe(this->tilt_command_topic(), [this](const std::string &topic, const std::string payload) {
      ESP_LOGD(TAG, "'%s': Setting tilt = %s", this->friendly_name().c_str(), payload.c_str());
      auto val = atof(payload.c_str());
      // TODO: Handle errors
      this->cover_->set_tilt(val);
      if (cover_->optimistic()) {
        this->publish_tilt(val);
        if (this->custom_position_command_topic_.empty()) { // if no position then tilt sets state
          if (
              (val <= cover_->tilt_closed_value() && !cover_->tilt_invert_state()) ||
              (val >= cover_->tilt_closed_value() && cover_->tilt_invert_state())
          )
            this->publish_state(COVER_CLOSED);
          else
            this->publish_state(COVER_OPEN);
        }
      }
  });
}

void MQTTCoverComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "MQTT cover '%s':", this->cover_->get_name().c_str());
  if (!this->custom_command_topic_.empty())
    ESP_LOGCONFIG(TAG, "  Command Topic '%s':", this->custom_command_topic_.c_str());
  if (!this->custom_state_topic_.empty())
    ESP_LOGCONFIG(TAG, "  State Topic '%s':", this->custom_state_topic_.c_str());
  if (!this->custom_position_command_topic_.empty())
    ESP_LOGCONFIG(TAG, "  Position Command Topic '%s':", this->custom_position_command_topic_.c_str());
  if (!this->custom_position_state_topic_.empty())
    ESP_LOGCONFIG(TAG, "  Position State Topic '%s':", this->custom_position_state_topic_.c_str());
  if (!this->custom_tilt_command_topic_.empty())
    ESP_LOGCONFIG(TAG, "  Tilt Command Topic '%s':", this->custom_tilt_command_topic_.c_str());
  if (!this->custom_tilt_status_topic_.empty())
    ESP_LOGCONFIG(TAG, "  Tilt Status Topic '%s':", this->custom_tilt_status_topic_.c_str());
}
void MQTTCoverComponent::send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) {
  if (this->custom_command_topic_.empty()) {
//    config.state_topic = false;
    config.command_topic = false;
  }
  if (this->cover_->assumed_state())
    root["optimistic"] = true;
  if (!this->custom_position_command_topic_.empty()) {
    root["set_position_topic"] = this->position_command_topic();
    root["position_topic"] = this->position_state_topic();
    root["position_closed"] = this->cover_->position_closed();
    root["position_open"] = this->cover_->position_open();
  }
  if (!this->custom_tilt_command_topic_.empty()) {
    root["tilt_command_topic"] = this->tilt_command_topic();
    root["tilt_status_topic"] = this->tilt_status_topic();
    root["tilt_min"] = this->cover_->tilt_min();
    root["tilt_max"] = this->cover_->tilt_max();
    root["tilt_closed_value"] = this->cover_->tilt_closed_value();
    root["tilt_opened_value"] = this->cover_->tilt_opened_value();
    root["tilt_invert_state"] = this->cover_->tilt_invert_state();
  }
  root["optimistic"] = this-cover_->optimistic();
}

std::string MQTTCoverComponent::component_type() const { return "cover"; }
std::string MQTTCoverComponent::friendly_name() const { return this->cover_->get_name(); }

void MQTTCoverComponent::set_custom_command_topic(const std::string &topic) {
  this->custom_command_topic_ = topic;
}
void MQTTCoverComponent::set_custom_state_topic(const std::string &topic) {
  this->custom_state_topic_ = topic;
}
void MQTTCoverComponent::set_custom_position_command_topic(const std::string &topic) {
  this->custom_position_command_topic_ = topic;
}
void MQTTCoverComponent::set_custom_position_state_topic(const std::string &topic) {
  this->custom_position_state_topic_ = topic;
}
void MQTTCoverComponent::set_custom_tilt_command_topic(const std::string &topic) {
  this->custom_tilt_command_topic_ = topic;
}
void MQTTCoverComponent::set_custom_tilt_status_topic(const std::string &topic) {
  this->custom_tilt_status_topic_ = topic;
}
const std::string MQTTCoverComponent::command_topic() const {
  if (this->custom_command_topic_.empty())
    return this->get_default_topic_for_("command");
  return this->custom_command_topic_;
}
const std::string MQTTCoverComponent::state_topic() const {
  if (this->custom_state_topic_.empty())
    return this->get_default_topic_for_("state");
  return this->custom_state_topic_;
}
const std::string MQTTCoverComponent::position_command_topic() const {
  if (this->custom_position_command_topic_.empty())
    return this->get_default_topic_for_("position/command");
  return this->custom_position_command_topic_;
}
const std::string MQTTCoverComponent::position_state_topic() const {
  if (this->custom_position_state_topic_.empty())
    return this->get_default_topic_for_("position/state");
  return this->custom_position_state_topic_;
}
const std::string MQTTCoverComponent::tilt_command_topic() const {
  if (this->custom_tilt_command_topic_.empty())
    return this->get_default_topic_for_("tilt/command");
  return this->custom_tilt_command_topic_;
}
const std::string MQTTCoverComponent::tilt_status_topic() const {
  if (this->custom_tilt_status_topic_.empty())
    return this->get_default_topic_for_("tilt/state");
  return this->custom_tilt_status_topic_;
}

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
bool MQTTCoverComponent::publish_position(float position_value) {
  const char *position_buf;
  char buf[64];
  sprintf(buf, "%.0f", position_value);
  position_buf = buf;
  ESP_LOGD(TAG, "'%s': Sending position = %s", this->friendly_name().c_str(), position_buf);
  return this->publish(this->position_state_topic(), position_buf);
}
bool MQTTCoverComponent::publish_tilt(float tilt_value) {
  const char *tilt_buf;
  char buf[64];
  sprintf(buf, "%.0f", tilt_value);
  tilt_buf = buf;
  ESP_LOGD(TAG, "'%s': Sending tilt = %s", this->friendly_name().c_str(), tilt_buf);
  return this->publish(this->tilt_status_topic(), tilt_buf);
}

}  // namespace cover

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_COVER
