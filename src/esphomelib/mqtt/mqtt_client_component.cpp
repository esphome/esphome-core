//
// Created by Otto Winter on 25.11.17.
//

#include <esp_log.h>

#include <utility>
#include "mqtt_client_component.h"
#include "esphomelib/log_component.h"

static const char *TAG = "MQTTClient";

namespace esphomelib {

namespace mqtt {

MQTTClientComponent::MQTTClientComponent(MQTTCredentials credentials,
                                         std::string node_id)
    : credentials_(std::move(credentials)),
      discovery_info_(Optional<MQTTDiscoveryInfo>()),
      topic_prefix_(std::move(node_id)) {

}

void MQTTClientComponent::setup() {
  ESP_LOGD(TAG, "Setting up MQTT...");
  this->mqtt_client_.setClient(this->client_);
  this->mqtt_client_.setServer(this->credentials_.address.c_str(), this->credentials_.port);

  ESP_LOGV(TAG, "    Server Address: %s:%u", this->credentials_.address.c_str(), this->credentials_.port);
  ESP_LOGV(TAG, "    Username: '%s'", this->credentials_.username.c_str());
  ESP_LOGV(TAG, "    Password: '%s'", this->credentials_.password.c_str());
  this->credentials_.client_id = truncate_string(this->credentials_.client_id, 23);
  ESP_LOGV(TAG, "    Client ID: '%s'", this->credentials_.client_id.c_str());
  this->mqtt_client_.setCallback(pub_sub_client_callback);

  global_mqtt_client = this;

  this->reconnect();
}

void MQTTClientComponent::loop() {
  this->reconnect();
  this->mqtt_client_.loop();
}

void MQTTClientComponent::pub_sub_client_callback(char *topic, uint8_t *payload, unsigned int length) {
  std::string payload_s(reinterpret_cast<char *>(payload), length);
  std::string topic_s(topic);
  ESP_LOGV(TAG, "mqtt_callback(topic='%s', payload='%s')", topic_s.c_str(), payload_s.c_str());

  for (auto &subscription : global_mqtt_client->subscriptions_)
    if (topic_s == subscription.topic)
      subscription.callback(payload_s);
}

void MQTTClientComponent::subscribe(const std::string &topic, mqtt_callback_t callback, uint8_t qos) {
  ESP_LOGD(TAG, "Subscribing to topic='%s' qos=%u...", topic.c_str(), qos);
  MQTTSubscription subscription {
      .topic = topic,
      .qos = qos,
      .callback = std::move(callback),
  };
  this->subscriptions_.push_back(subscription);

  if (this->is_connected())
    this->mqtt_client_.subscribe(topic.c_str(), qos);
}

void MQTTClientComponent::subscribe_json(const std::string &topic, json_parse_t callback, uint8_t qos) {
  ESP_LOGD(TAG, "Subscribing to topic='%s' qos=%u with JSON...", topic.c_str(), qos);
  MQTTSubscription subscription {
      .topic = topic,
      .qos = qos,
      .callback = [this, callback](const std::string &payload) {
        this->parse_json(payload, callback);
      },
  };
  this->subscriptions_.push_back(subscription);

  if (this->is_connected())
    this->mqtt_client_.subscribe(topic.c_str(), qos);
}

bool MQTTClientComponent::is_connected() {
  return this->mqtt_client_.connected();
}

void MQTTClientComponent::reconnect() {
  if (this->is_connected())
    return;

  ESP_LOGV(TAG, "Reconnecting to MQTT...");
  uint32_t start = millis();
  do {
    ESP_LOGV(TAG, "    Attempting MQTT connection...");
    if (millis() - start > 20000) {
      ESP_LOGE(TAG, "    Can't connect to MQTT... Restarting...");
      esp_restart();
    }

    const char *id = this->credentials_.client_id.c_str();
    const char *user = this->credentials_.username.c_str();
    const char *pass = this->credentials_.password.c_str();
    const char *will_topic = nullptr;
    uint8_t will_qos = 0;
    bool will_retain = false;
    const char *will_message = nullptr;
    if (this->last_will_.defined) {
      will_topic = this->last_will_->topic.c_str();
      will_qos = this->last_will_->qos;
      will_retain = this->last_will_->retain;
      will_message = this->last_will_->payload.c_str();
    }

    if (this->mqtt_client_.connect(id, user, pass, will_topic, will_qos, will_retain, will_message)) {
      ESP_LOGV(TAG, "    Connected!");
      break;
    } else {
      ESP_LOGW(TAG, "    failed, rc=%d", this->mqtt_client_.state());
      ESP_LOGW(TAG, "    Try again in 1 second");

      delay(1000);
    }
  } while (!this->is_connected());

  if (this->birth_message_.defined)
    this->publish(this->birth_message_.value);

  for (MQTTSubscription &subscription : this->subscriptions_)
    this->mqtt_client_.subscribe(subscription.topic.c_str(), subscription.qos);
}

void MQTTClientComponent::publish(const std::string &topic, const std::string &payload, bool retain) {
  if (topic != global_log_component->get_mqtt_topic()) {
    ESP_LOGV(TAG, "Publish(topic='%s' payload='%s' retain=%d)", topic.c_str(), payload.c_str(), retain);
  }

  const auto *payload_data = reinterpret_cast<const uint8_t *>(payload.data());
  this->reconnect();
  this->mqtt_client_.publish(topic.c_str(), payload_data, payload.length(), retain);
  this->mqtt_client_.loop();
}

void MQTTClientComponent::publish(const MQTTMessage &message) {
  this->publish(message.topic, message.payload, message.retain);
}

void MQTTClientComponent::set_last_will(std::string topic, std::string payload, uint8_t qos,
                                        bool retain) {
  this->last_will_ = MQTTMessage {
      .topic = std::move(topic),
      .payload = std::move(payload),
      .qos = qos,
      .retain = retain
  };
}

void MQTTClientComponent::set_birth_message(std::string topic, std::string payload, bool retain) {
  this->birth_message_ = MQTTMessage {
      .topic = std::move(topic),
      .payload = std::move(payload),
      .qos = 0, // not used
      .retain = retain
  };
}

void MQTTClientComponent::set_discovery_info(std::string prefix, bool retain) {
  this->discovery_info_ = MQTTDiscoveryInfo {
      .prefix = std::move(prefix),
      .retain = retain
  };
}

void MQTTClientComponent::remove_last_will() {
  this->last_will_.defined = false;
}

void MQTTClientComponent::remove_birth_message() {
  this->birth_message_.defined = false;
}

void MQTTClientComponent::set_discovery_retain(bool retain) {
  this->discovery_info_->retain = retain;
}
void MQTTClientComponent::disable_discovery() {
  this->discovery_info_.defined = false;
}
float MQTTClientComponent::get_setup_priority() const {
  return setup_priority::MQTT_CLIENT;
}
const Optional<MQTTDiscoveryInfo> &MQTTClientComponent::get_discovery_info() const {
  return this->discovery_info_;
}
void MQTTClientComponent::set_topic_prefix(const std::string &topic_prefix) {
  this->topic_prefix_ = topic_prefix;
}
const std::string &MQTTClientComponent::get_topic_prefix() const {
  return this->topic_prefix_;
}
const Optional<MQTTMessage> &MQTTClientComponent::get_birth_message() const {
  return this->birth_message_;
}
void MQTTClientComponent::set_birth_message(const Optional<MQTTMessage> &birth_message) {
  this->birth_message_ = birth_message;
}
const Optional<MQTTMessage> &MQTTClientComponent::get_last_will() const {
  return this->last_will_;
}
void MQTTClientComponent::set_last_will(const Optional<MQTTMessage> &last_will) {
  this->last_will_ = last_will;
}
void MQTTClientComponent::parse_json(const std::string &message, const json_parse_t &f) {
  size_t size = message.length() + 1;
  char payload_s[size];
  memcpy(payload_s, message.c_str(), size);
  payload_s[size - 1] = 0;

  StaticJsonBuffer<JSON_BUFFER_SIZE> buffer;
  JsonObject &root = buffer.parseObject(payload_s);

  if (!root.success()) {
    ESP_LOGW(TAG, "Parsing JSON failed.");
    return;
  }

  f(root);
}

MQTTClientComponent *global_mqtt_client = nullptr;

} // namespace mqtt

} // namespace esphomelib