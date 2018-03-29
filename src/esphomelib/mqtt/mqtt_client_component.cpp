//
// Created by Otto Winter on 25.11.17.
//

#include "esphomelib/mqtt/mqtt_client_component.h"

#include <utility>

#include "esphomelib/log.h"
#include "esphomelib/log_component.h"
#include "esphomelib/application.h"

static const char *TAG = "mqtt:mqtt_client";

namespace esphomelib {

namespace mqtt {

MQTTClientComponent::MQTTClientComponent(const MQTTCredentials &credentials)
    : credentials_(credentials) {
  global_mqtt_client = this;
}

void MQTTClientComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MQTT...");
  this->mqtt_client_.setClient(this->client_);

  ESP_LOGCONFIG(TAG, "    Server Address: %s:%u", this->credentials_.address.c_str(), this->credentials_.port);
  ESP_LOGCONFIG(TAG, "    Username: '%s'", this->credentials_.username.c_str());
  ESP_LOGCONFIG(TAG, "    Password: '%s'", this->credentials_.password.c_str());
  this->credentials_.client_id = truncate_string(this->credentials_.client_id, 23);
  ESP_LOGCONFIG(TAG, "    Client ID: '%s'", this->credentials_.client_id.c_str());
  this->mqtt_client_.setCallback(pub_sub_client_callback);

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

void MQTTClientComponent::subscribe(const std::string &topic, const mqtt_callback_t &callback, uint8_t qos) {
  ESP_LOGD(TAG, "Subscribing to topic='%s' qos=%u...", topic.c_str(), qos);
  MQTTSubscription subscription{
      .topic = topic,
      .qos = qos,
      .callback = callback,
  };
  this->subscriptions_.push_back(subscription);

  if (this->is_connected())
    this->mqtt_client_.subscribe(topic.c_str(), qos);
}

void MQTTClientComponent::subscribe_json(const std::string &topic, const json_parse_t &callback, uint8_t qos) {
  ESP_LOGD(TAG, "Subscribing to topic='%s' qos=%u with JSON...", topic.c_str(), qos);
  MQTTSubscription subscription{
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

  ESP_LOGD(TAG, "Reconnecting to MQTT...");
  uint32_t start = millis();
  do {
    ESP_LOGD(TAG, "    Attempting MQTT connection...");
    if (millis() - start > 20000) {
      ESP_LOGE(TAG, "    Can't connect to MQTT... Restarting...");
      ESP.restart();
    }

    const char *id = this->credentials_.client_id.c_str();
    const char *user = nullptr;
    if (!this->credentials_.username.empty())
      user = this->credentials_.username.c_str();
    const char *pass = nullptr;
    if (!this->credentials_.password.empty())
      pass = this->credentials_.password.c_str();
    const char *will_topic = nullptr;
    uint8_t will_qos = 0;
    bool will_retain = false;
    const char *will_message = nullptr;
    if (this->is_last_will_enabled()) {
      MQTTMessage last_will = this->get_last_will();
      will_topic = last_will.topic.c_str();
      will_qos = last_will.qos;
      will_retain = last_will.retain;
      will_message = last_will.payload.c_str();
    }
    // temporary hack to fix issue in PubSubClient
    char c[256];
    strcpy(c, this->credentials_.address.c_str());
    this->mqtt_client_.setServer(c, this->credentials_.port);

    if (this->mqtt_client_.connect(id, user, pass, will_topic, will_qos, will_retain, will_message)) {
      ESP_LOGD(TAG, "    Connected!");
      break;
    } else {
      ESP_LOGW(TAG, "    failed, rc=%d", this->mqtt_client_.state());
      ESP_LOGW(TAG, "    Try again in 1 second");

      delay(1000);
    }
  } while (!this->is_connected());

  if (this->is_birth_message_enabled())
    this->publish(this->get_birth_message());

  for (MQTTSubscription &subscription : this->subscriptions_)
    this->mqtt_client_.subscribe(subscription.topic.c_str(), subscription.qos);
}

void MQTTClientComponent::publish(const std::string &topic, const std::string &payload, bool retain) {
  bool logging_topic = topic == global_log_component->get_logging_topic();
  if (!logging_topic) {
    ESP_LOGV(TAG, "Publish(topic='%s' payload='%s' retain=%d)", topic.c_str(), payload.c_str(), retain);
  }

  const auto *payload_data = reinterpret_cast<const uint8_t *>(payload.data());
  this->reconnect();
  bool ret = this->mqtt_client_.publish(topic.c_str(), payload_data, payload.length(), retain);
  if (!ret && !logging_topic)
    ESP_LOGW(TAG, "PubSubClient::publish() failed (connected=%d, MQTT_MAX_PACKET_SIZE=%d)!", this->is_connected(),
             MQTT_MAX_PACKET_SIZE);
  ret = this->mqtt_client_.loop();
  if (!ret && !logging_topic)
    ESP_LOGW(TAG, "PubSubClient::loop() failed!");
  yield();
}

void MQTTClientComponent::publish(const MQTTMessage &message) {
  this->publish(message.topic, message.payload, message.retain);
}

void MQTTClientComponent::set_last_will(const std::string &topic, const std::string &payload, uint8_t qos,
                                        bool retain) {
  this->set_last_will(MQTTMessage{
      .topic = topic,
      .payload = payload,
      .qos = qos,
      .retain = retain
  });
  this->availability_enabled_.defined = false;
}

void MQTTClientComponent::set_birth_message(const std::string &topic, const std::string &payload, bool retain) {
  this->set_birth_message(MQTTMessage{
      .topic = topic,
      .payload = payload,
      .qos = 0, // not used
      .retain = retain
  });
}

void MQTTClientComponent::set_discovery_info(const std::string &prefix, bool retain) {
  this->set_discovery_info(MQTTDiscoveryInfo{
      .prefix = prefix,
      .retain = retain,
  });
}

void MQTTClientComponent::disable_last_will() {
  this->set_last_will(MQTTMessage{
      .topic = "",
      .payload = "",
      .qos = 0,
      .retain = false
  });
}

void MQTTClientComponent::disable_discovery() {
  this->discovery_info_ = MQTTDiscoveryInfo{
      .prefix = "",
      .retain = false
  };
}
float MQTTClientComponent::get_setup_priority() const {
  return setup_priority::MQTT_CLIENT;
}
MQTTDiscoveryInfo MQTTClientComponent::get_discovery_info() const {
  if (this->discovery_info_.defined) {
    return this->discovery_info_.value;
  } else {
    return MQTTDiscoveryInfo {
        .prefix = "homeassistant",
        .retain = true,
    };
  }
}
void MQTTClientComponent::set_topic_prefix(const std::string &topic_prefix) {
  this->topic_prefix_ = topic_prefix;
}
std::string MQTTClientComponent::get_topic_prefix() const {
  if (this->topic_prefix_.defined) {
    return this->topic_prefix_.value;
  } else {
    return App.get_name();
  }
}
MQTTMessage MQTTClientComponent::get_birth_message() const {
  if (this->birth_message_.defined) {
    return this->birth_message_.value;
  } else {
    return MQTTMessage {
        .topic = this->get_topic_prefix() + "/status",
        .payload = "online",
        .qos = 0,
        .retain = true,
    };
  }
}
void MQTTClientComponent::set_birth_message(const MQTTMessage &birth_message) {
  this->birth_message_ = birth_message;
  this->availability_enabled_.defined = false;
}
MQTTMessage MQTTClientComponent::get_last_will() const {
  if (this->birth_message_.defined) {
    return this->birth_message_.value;
  } else {
    return MQTTMessage {
        .topic = this->get_topic_prefix() + "/status",
        .payload = "offline",
        .qos = 0,
        .retain = true,
    };
  }
}
void MQTTClientComponent::set_last_will(const MQTTMessage &last_will) {
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
void MQTTClientComponent::disable_birth_message() {
  this->set_birth_message(MQTTMessage{
      .topic = "",
      .payload = "",
      .qos = 0,
      .retain = false
  });
}
void MQTTClientComponent::set_discovery_info(const MQTTDiscoveryInfo &discovery_info) {
  this->discovery_info_ = discovery_info;
}
bool MQTTClientComponent::is_discovery_enabled() {
  return !this->discovery_info_.defined || !this->discovery_info_->prefix.empty();
}
bool MQTTClientComponent::is_last_will_enabled() {
  return !this->last_will_.defined || !this->last_will_->topic.empty();
}
bool MQTTClientComponent::is_birth_message_enabled() {
  return !this->birth_message_.defined || !this->birth_message_->topic.empty();
}
bool MQTTClientComponent::is_availability_enabled() {
  if (this->availability_enabled_.defined)
    return this->availability_enabled_.value;
  bool enabled;
  if (!this->is_birth_message_enabled() || !this->is_last_will_enabled()) {
    enabled = false;
  } else {
    enabled = this->get_birth_message().topic == this->get_last_will().topic;
  }

  this->availability_enabled_ = enabled;
  return enabled;
}

MQTTClientComponent *global_mqtt_client = nullptr;

} // namespace mqtt

} // namespace esphomelib