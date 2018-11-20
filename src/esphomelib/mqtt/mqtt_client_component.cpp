#include "esphomelib/mqtt/mqtt_client_component.h"

#include "esphomelib/log.h"
#include "esphomelib/log_component.h"
#include "esphomelib/util.h"

static const char *TAG = "mqtt.client";

ESPHOMELIB_NAMESPACE_BEGIN

namespace mqtt {

MQTTClientComponent::MQTTClientComponent(const MQTTCredentials &credentials, const std::string &topic_prefix)
    : credentials_(credentials) {
  global_mqtt_client = this;
  this->set_topic_prefix(topic_prefix);
}

void MQTTClientComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MQTT...");
  if (this->credentials_.client_id.empty())
    this->credentials_.client_id = generate_hostname(this->topic_prefix_);
  this->mqtt_client_.onMessage([this](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total){
    std::string payload_s(payload, len);
    std::string topic_s(topic);
    this->on_message(topic_s, payload_s);
  });
  this->mqtt_client_.onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
    this->state_ = MQTT_CLIENT_DISCONNECTED;
    this->disconnect_reason_ = reason;
  });
  if (this->is_log_message_enabled()) {
    global_log_component->add_on_log_callback([this](int level, const char *message) {
      if (level <= this->log_level_ && this->is_connected()) {
        this->publish(this->log_message_.topic, message, strlen(message),
            this->log_message_.qos, this->log_message_.retain);
      }
    });
  }

  add_shutdown_hook([this](const char *cause){
    if (!this->shutdown_message_.topic.empty()) {
      yield();
      this->publish(this->shutdown_message_);
      yield();
    }
    this->mqtt_client_.disconnect(true);
  });

  this->last_connected_ = millis();
  this->start_connect();
}
void MQTTClientComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "MQTT:");
  ESP_LOGCONFIG(TAG, "  Server Address: %s:%u", this->credentials_.address.c_str(), this->credentials_.port);
  ESP_LOGCONFIG(TAG, "  Username: '%s'", this->credentials_.username.c_str());
  ESP_LOGCONFIG(TAG, "  Client ID: '%s'", this->credentials_.client_id.c_str());
  if (!this->discovery_info_.prefix.empty()) {
    ESP_LOGCONFIG(TAG, "  Discovery prefix: '%s'", this->discovery_info_.prefix.c_str());
    ESP_LOGCONFIG(TAG, "  Discovery retain: %s", YESNO(this->discovery_info_.retain));
  }
  ESP_LOGCONFIG(TAG, "  Topic Prefix: '%s'", this->topic_prefix_.c_str());
  if (!this->log_message_.topic.empty()) {
    ESP_LOGCONFIG(TAG, "  Log Topic: '%s'", this->log_message_.topic.c_str());
  }
  if (!this->availability_.topic.empty()) {
    ESP_LOGCONFIG(TAG, "  Availability: '%s'", this->availability_.topic.c_str());
  }
}
bool MQTTClientComponent::can_proceed() {
  return this->is_connected();
}

void MQTTClientComponent::start_connect() {
  if (!network_is_connected())
    return;

  this->status_set_warning();
  ESP_LOGI(TAG, "Connecting to MQTT...");
  // Force disconnect first
  this->mqtt_client_.disconnect(true);

  this->mqtt_client_.setClientId(this->credentials_.client_id.c_str());
  const char *username = nullptr;
  if (!this->credentials_.username.empty())
    username = this->credentials_.username.c_str();
  const char *password = nullptr;
  if (!this->credentials_.password.empty())
    password = this->credentials_.password.c_str();

  this->mqtt_client_.setCredentials(username, password);

  this->mqtt_client_.setServer(this->credentials_.address.c_str(), this->credentials_.port);
  if (!this->last_will_.topic.empty()) {
    this->mqtt_client_.setWill(this->last_will_.topic.c_str(), this->last_will_.qos, this->last_will_.retain,
                               this->last_will_.payload.c_str(), this->last_will_.payload.length());
  }

  this->mqtt_client_.connect();
  this->state_ = MQTT_CLIENT_CONNECTING;
  this->connect_begin_ = millis();
}

void MQTTClientComponent::check_connected() {
  if (!this->mqtt_client_.connected()) {
    if (millis() - this->connect_begin_ > 15000) {
      this->state_ = MQTT_CLIENT_DISCONNECTED;
      this->start_connect();
    }
    return;
  }

  this->state_ = MQTT_CLIENT_CONNECTED;
  this->status_clear_warning();
  ESP_LOGI(TAG, "MQTT Connected!");
  // MQTT Client needs some time to be fully set up.
  delay(100);

  if (!this->birth_message_.topic.empty())
    this->publish(this->birth_message_);

  for (MQTTSubscription &subscription : this->subscriptions_)
    this->mqtt_client_.subscribe(subscription.topic.c_str(), subscription.qos);

  for (MQTTComponent *component : this->children_)
    component->schedule_resend_state();
}

void MQTTClientComponent::loop() {
  if (this->disconnect_reason_.has_value()) {
    const char *reason_s = nullptr;
    switch (*this->disconnect_reason_) {
      case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
        reason_s = "TCP disconnected";
        break;
      case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
        reason_s = "Unacceptable Protocol Version";
        break;
      case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
        reason_s = "Identifier Rejected";
        break;
      case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE:
        reason_s = "Server Unavailable";
        break;
      case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
        reason_s = "Malformed Credentials";
        break;
      case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
        reason_s = "Not Authorized";
        break;
      case AsyncMqttClientDisconnectReason::ESP8266_NOT_ENOUGH_SPACE:
        reason_s = "Not Enough Space";
        break;
      case AsyncMqttClientDisconnectReason::TLS_BAD_FINGERPRINT:
        reason_s = "TLS Bad Fingerprint";
        break;
      default:
        reason_s = "Unknown";
        break;
    }
    if (!network_is_connected()) {
      reason_s = "WiFi disconnected";
    }
    ESP_LOGW(TAG, "MQTT Disconnected: %s.", reason_s);
    this->disconnect_reason_.reset();
  }

  const uint32_t now = millis();

  switch (this->state_) {
    case MQTT_CLIENT_DISCONNECTED:
      if (now - this->connect_begin_ > 5000) {
        this->start_connect();
      }
      break;
    case MQTT_CLIENT_CONNECTING:
      this->check_connected();
      break;
    case MQTT_CLIENT_CONNECTED:
      if (!this->mqtt_client_.connected()) {
        this->state_ = MQTT_CLIENT_DISCONNECTED;
        ESP_LOGW(TAG, "Lost MQTT Client connection!");
        this->start_connect();
      } else {
        this->last_connected_ = now;
      }
      break;
  }

  if (millis() - this->last_connected_ > this->reboot_timeout_ && this->reboot_timeout_ != 0) {
    ESP_LOGE(TAG, "    Can't connect to MQTT... Restarting...");
    reboot("mqtt");
  }
}

void MQTTClientComponent::set_keep_alive(uint16_t keep_alive_s) {
  this->mqtt_client_.setKeepAlive(keep_alive_s);
}

void MQTTClientComponent::subscribe(const std::string &topic, mqtt_callback_t callback, uint8_t qos) {
  ESP_LOGV(TAG, "Subscribing to topic='%s' qos=%u...", topic.c_str(), qos);
  MQTTSubscription subscription{
      .topic = topic,
      .qos = qos,
      .callback = std::move(callback),
  };
  this->subscriptions_.push_back(subscription);

  if (this->mqtt_client_.connected())
    this->mqtt_client_.subscribe(topic.c_str(), qos);
}

void MQTTClientComponent::subscribe_json(const std::string &topic, json_parse_t callback, uint8_t qos) {
  ESP_LOGV(TAG, "Subscribing to topic='%s' qos=%u with JSON...", topic.c_str(), qos);
  auto f = std::bind(&parse_json, std::placeholders::_1, callback);
  MQTTSubscription subscription{
      .topic = topic,
      .qos = qos,
      .callback = f,
  };
  this->subscriptions_.push_back(subscription);

  if (this->mqtt_client_.connected())
    this->mqtt_client_.subscribe(topic.c_str(), qos);
}

void MQTTClientComponent::publish(const std::string &topic, const std::string &payload, uint8_t qos, bool retain) {
  this->publish(topic, payload.data(), payload.size(), qos, retain);
}

void MQTTClientComponent::publish(const std::string &topic, const char *payload, size_t payload_length,
    uint8_t qos, bool retain) {
  if (!this->is_connected()) {
    // critical components will re-transmit their messages
    return;
  }
  bool logging_topic = topic == this->log_message_.topic;
  if (!logging_topic) {
    ESP_LOGV(TAG, "Publish(topic='%s' payload='%s' retain=%d)", topic.c_str(), payload, retain);
  }

  this->loop();
  uint16_t ret = this->mqtt_client_.publish(topic.c_str(), qos, retain, payload, payload_length);
  yield();
  if (ret == 0 && !logging_topic && this->is_connected()) {
    delay(5);
    ret = this->mqtt_client_.publish(topic.c_str(), qos, retain, payload, payload_length);
    if (ret == 0) {
      ESP_LOGW(TAG, "Publish failed!");
      this->status_momentary_warning("publish", 5000);
    }
    delay(1);
  }
}

void MQTTClientComponent::publish(const MQTTMessage &message) {
  this->publish(message.topic, message.payload, message.qos, message.retain);
}

void MQTTClientComponent::set_last_will(MQTTMessage &&message) {
  this->last_will_ = std::move(message);
  this->recalculate_availability();
}

void MQTTClientComponent::set_birth_message(MQTTMessage &&message) {
  this->birth_message_ = std::move(message);
  this->recalculate_availability();
}

void MQTTClientComponent::set_shutdown_message(MQTTMessage &&message) {
  this->shutdown_message_ = std::move(message);
}

void MQTTClientComponent::set_discovery_info(std::string &&prefix, bool retain) {
  this->discovery_info_.prefix = std::move(prefix);
  this->discovery_info_.retain = retain;
}

void MQTTClientComponent::disable_last_will() {
  this->last_will_.topic = "";
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
const MQTTDiscoveryInfo &MQTTClientComponent::get_discovery_info() const {
  return this->discovery_info_;
}
void MQTTClientComponent::set_topic_prefix(std::string topic_prefix) {
  this->topic_prefix_ = std::move(topic_prefix);
  this->set_birth_message(MQTTMessage{
      .topic = this->topic_prefix_ + "/status",
      .payload = "online",
      .qos = 0,
      .retain = true,
  });
  this->set_last_will(MQTTMessage{
      .topic = this->topic_prefix_ + "/status",
      .payload = "offline",
      .qos = 0,
      .retain = true,
  });
  this->set_log_message_template(MQTTMessage{
      .topic = this->topic_prefix_ + "/debug",
      .payload = "",
      .qos = 0,
      .retain = false,
  });
}
const std::string &MQTTClientComponent::get_topic_prefix() const {
  return this->topic_prefix_;
}
void MQTTClientComponent::disable_birth_message() {
  this->birth_message_.topic = "";
}
void MQTTClientComponent::disable_shutdown_message() {
  this->shutdown_message_.topic = "";
}
bool MQTTClientComponent::is_discovery_enabled() const {
  return !this->discovery_info_.prefix.empty();
}
void MQTTClientComponent::set_client_id(std::string client_id) {
  this->credentials_.client_id = std::move(client_id);
}
const Availability &MQTTClientComponent::get_availability() {
  return this->availability_;
}
void MQTTClientComponent::recalculate_availability() {
  if (this->birth_message_.topic.empty() || this->birth_message_.topic != this->last_will_.topic) {
    this->availability_.topic = "";
    return;
  }
  this->availability_.topic = this->birth_message_.topic;
  this->availability_.payload_available = this->birth_message_.payload;
  this->availability_.payload_not_available = this->last_will_.payload;
}
void MQTTClientComponent::publish_json(const std::string &topic, const json_build_t &f, uint8_t qos, bool retain) {
  size_t len;
  const char *message = build_json(f, &len);
  this->publish(topic, message, len, qos, retain);
}
void MQTTClientComponent::set_log_message_template(MQTTMessage &&message) {
  this->log_message_ = std::move(message);
}

// INFO: MQTT spec mandates that topics must not be empty and must be valid NULL-terminated UTF-8 strings.
static bool topic_match_(const char *message, const char *subscription, bool is_normal, bool past_separator) {
  // Reached end of both strings at the same time, this means we have a successful match
  if (*message == '\0' && *subscription == '\0')
    return true;

  // Either the message or the subscribe are at the end. This means they don't match.
  if (*message == '\0' || *subscription == '\0')
    return false;

  bool do_wildcards = is_normal || past_separator;

  if (*subscription == '+' && do_wildcards) {
    // single level wildcard
    // consume + from subscription
    subscription++;
    // consume everything from message until '/' found or end of string
    while (*message != '\0' && *message != '/') {
      message++;
    }
    // after this, both pointers will point to a '/' or to the end of the string

    return topic_match_(message, subscription, is_normal, true);
  }

  if (*subscription == '#' && do_wildcards) {
    // multilevel wildcard - MQTT mandates that this must be at end of subscribe topic
    return true;
  }

  // this handles '/' and normal characters at the same time.
  if (*message != *subscription)
    return false;

  past_separator = past_separator || *subscription == '/';

  // consume characters
  subscription++;
  message++;

  return topic_match_(message, subscription, is_normal, past_separator);
}

static bool topic_match(const char *message, const char *subscription) {
  return topic_match_(message, subscription, *message != '\0' && *message != '$', false);
}

void MQTTClientComponent::on_message(const std::string &topic, const std::string &payload) {
#ifdef ARDUINO_ARCH_ESP8266
  this->defer([this, topic, payload]() {
#endif
    for (auto &subscription : this->subscriptions_)
      if (topic_match(topic.c_str(), subscription.topic.c_str()))
        subscription.callback(payload);
#ifdef ARDUINO_ARCH_ESP8266
  });
#endif
}
void MQTTClientComponent::disable_log_message() {
  this->log_message_.topic = "";
}
bool MQTTClientComponent::is_log_message_enabled() const {
  return !this->log_message_.topic.empty();
}
MQTTMessageTrigger *MQTTClientComponent::make_message_trigger(const std::string &topic, uint8_t qos) {
  return new MQTTMessageTrigger(topic, qos);
}
MQTTJsonMessageTrigger *MQTTClientComponent::make_json_message_trigger(const std::string &topic, uint8_t qos) {
  return new MQTTJsonMessageTrigger(topic, qos);
}
void MQTTClientComponent::set_reboot_timeout(uint32_t reboot_timeout) {
  this->reboot_timeout_ = reboot_timeout;
}
void MQTTClientComponent::register_mqtt_component(MQTTComponent *component) {
  this->children_.push_back(component);
}
void MQTTClientComponent::set_log_level(int level) {
  this->log_level_ = level;
}
bool MQTTClientComponent::is_connected() {
  return this->state_ == MQTT_CLIENT_CONNECTED && this->mqtt_client_.connected();
}

#if ASYNC_TCP_SSL_ENABLED
void MQTTClientComponent::add_ssl_fingerprint(const std::array<uint8_t, SHA1_SIZE> &fingerprint) {
  this->mqtt_client_.setSecure(true);
  this->mqtt_client_.addServerFingerprint(fingerprint.data());
}
#endif

MQTTClientComponent *global_mqtt_client = nullptr;

MQTTMessageTrigger::MQTTMessageTrigger(const std::string &topic, uint8_t qos) {
  global_mqtt_client->subscribe(topic, [this](const std::string &payload) {
    this->trigger(payload);
  }, qos);
}

MQTTJsonMessageTrigger::MQTTJsonMessageTrigger(const std::string &topic, uint8_t qos) {
  global_mqtt_client->subscribe_json(topic, [this](JsonObject &root) {
    this->trigger(root);
  }, qos);
}

} // namespace mqtt

ESPHOMELIB_NAMESPACE_END