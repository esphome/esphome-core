//
// Created by Otto Winter on 25.11.17.
//

#include "esphomelib/mqtt/mqtt_component.h"

#include <algorithm>
#include <utility>

#include "esphomelib/mqtt/mqtt_client_component.h"
#include "esphomelib/log.h"
#include "esphomelib/helpers.h"
#include "esphomelib/application.h"

namespace esphomelib {

namespace mqtt {

static const char *TAG = "mqtt::mqtt_component";

void MQTTComponent::set_retain(bool retain) {
  this->retain_ = retain;
}

std::string MQTTComponent::get_discovery_topic() const {
  if (!this->is_discovery_enabled())
    return "";
  const auto &discovery = global_mqtt_client->get_discovery_info();
  std::string sanitized_name = sanitize_string_whitelist(App.get_name(), DISCOVERY_CHARACTER_WHITELIST);
  return discovery.prefix + "/" + this->component_type() + "/" + sanitized_name + "/" + this->get_default_object_id()
      + "/config";
}

std::string MQTTComponent::get_default_topic_for(const std::string &suffix) const {
  return global_mqtt_client->get_topic_prefix() + "/" + this->component_type() + "/" + this->get_default_object_id()
      + "/"
      + suffix;
}

const std::string MQTTComponent::get_state_topic() const {
  return this->get_topic_for("state");
}

const std::string MQTTComponent::get_command_topic() const {
  return this->get_topic_for("command");
}

void MQTTComponent::send_message(const std::string &topic, const std::string &payload, const Optional<bool> &retain) {
  bool actual_retain = this->retain_;
  if (retain)
    actual_retain = retain.value;
  global_mqtt_client->publish(topic, payload, actual_retain);
}

void MQTTComponent::send_json_message(const std::string &topic, const json_build_t &f, const Optional<bool> &retain) {
  StaticJsonBuffer<JSON_BUFFER_SIZE> json_buffer;
  JsonObject &root = json_buffer.createObject();

  f(json_buffer, root);

  char buffer[MQTT_MAX_PACKET_SIZE];
  root.printTo(buffer, MQTT_MAX_PACKET_SIZE);
  this->send_message(topic, std::string(buffer), retain);
}

void MQTTComponent::send_discovery(const json_build_t &f,
                                   bool state_topic, bool command_topic, const std::string &platform) {
  if (!this->is_discovery_enabled())
    return;
  if (this->friendly_name_.empty()) // empty friendly name => no discovery
    return;

  ESP_LOGV(TAG, "Sending discovery...");

  this->send_json_message(this->get_discovery_topic(), [&](JsonBuffer &buffer, JsonObject &root) {
    root["name"] = this->friendly_name_;
    root["platform"] = platform;
    if (state_topic)
      root["state_topic"] = this->get_state_topic();
    if (command_topic)
      root["command_topic"] = this->get_command_topic();

    Availability availability = this->get_availability();
    if (!availability.topic.empty()) {
      root["availability_topic"] = availability.topic;
      if (availability.payload_available != "online")
        root["payload_available"] = availability.payload_available;
      if (availability.payload_not_available != "offline")
        root["payload_not_available"] = availability.payload_not_available;
    }

    f(buffer, root);
  }, global_mqtt_client->get_discovery_info().retain);
}

bool MQTTComponent::get_retain() const {
  return this->retain_;
}

bool MQTTComponent::is_discovery_enabled() const {
  return !this->friendly_name_.empty() && global_mqtt_client->is_discovery_enabled();
}

std::string MQTTComponent::get_default_object_id() const {
  return sanitize_string_whitelist(to_lowercase_underscore(this->friendly_name_), DISCOVERY_CHARACTER_WHITELIST);
}

const std::string &MQTTComponent::get_friendly_name() const {
  return this->friendly_name_;
}

void MQTTComponent::subscribe(const std::string &topic, const mqtt_callback_t &callback, uint8_t qos) {
  global_mqtt_client->subscribe(topic, std::move(callback), qos);
}

void MQTTComponent::parse_json(const std::string &message, const json_parse_t &f) {
  MQTTClientComponent::parse_json(message, f);
}

void MQTTComponent::subscribe_json(const std::string &topic, const json_parse_t &callback, uint8_t qos) {
  global_mqtt_client->subscribe_json(topic, callback, qos);
}

MQTTComponent::MQTTComponent(const std::string &friendly_name)
    : friendly_name_(friendly_name) {

}
float MQTTComponent::get_setup_priority() const {
  return setup_priority::MQTT_COMPONENT;
}
void MQTTComponent::disable_discovery() {
  this->friendly_name_ = "";
}
void MQTTComponent::set_custom_state_topic(const std::string &custom_state_topic) {
  this->set_custom_topic("state", custom_state_topic);
}
void MQTTComponent::set_custom_command_topic(const std::string &custom_command_topic) {
  this->set_custom_topic("command", custom_command_topic);
}
void MQTTComponent::set_friendly_name(const std::string &friendly_name) {
  this->friendly_name_ = friendly_name;
}
void MQTTComponent::set_custom_topic(const std::string &key, const std::string &custom_topic) {
  this->custom_topics_[key] = custom_topic;
}
const std::string MQTTComponent::get_topic_for(const std::string &key) const {
  if (this->custom_topics_.find(key) != this->custom_topics_.end())
    return this->custom_topics_.find(key)->second;
  return this->get_default_topic_for(key);
}

Availability MQTTComponent::get_availability() const {
  if (this->availability_.defined) {
    return this->availability_.value;
  } else {
    if (!global_mqtt_client->is_availability_enabled())
      return mqtt::Availability{
          .topic = "",
          .payload_available = "",
          .payload_not_available = ""
      };
    return mqtt::Availability{
        .topic = mqtt::global_mqtt_client->get_last_will().topic,
        .payload_available = mqtt::global_mqtt_client->get_birth_message().payload,
        .payload_not_available = mqtt::global_mqtt_client->get_last_will().payload,
    };
  }
}
void MQTTComponent::set_availability(const Availability &availability) {
  this->availability_ = availability;
}
void MQTTComponent::disable_availability() {
  this->set_availability(Availability{
      .topic = "",
      .payload_available = "",
      .payload_not_available = "",
  });
}

} // namespace mqtt

} // namespace esphomelib