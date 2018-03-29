//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_MQTT_MQTT_CLIENT_COMPONENT_H
#define ESPHOMELIB_MQTT_MQTT_CLIENT_COMPONENT_H

#include <string>
#include <functional>
#include <vector>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

#include "esphomelib/component.h"
#include "esphomelib/helpers.h"

#ifndef JSON_BUFFER_SIZE
#define JSON_BUFFER_SIZE (JSON_OBJECT_SIZE(32))
#endif

namespace esphomelib {

namespace mqtt {

/** Callback for MQTT subscriptions.
 *
 * First parameter is the topic, the second one is the payload.
 */
using mqtt_callback_t = std::function<void(const std::string &)>;

/// Callback function typedef for parsing JsonObjects.
using json_parse_t = std::function<void(JsonObject &)>;

/// internal struct for MQTT messages.
struct MQTTMessage {
  std::string topic;
  std::string payload;
  uint8_t qos; ///< QoS. Only for last will testaments.
  bool retain;
};

/// internal struct for MQTT subscriptions.
struct MQTTSubscription {
  std::string topic;
  uint8_t qos;
  mqtt_callback_t callback;
};

/// internal struct for MQTT credentials.
struct MQTTCredentials {
  std::string address; ///< The address of the server without port number
  uint16_t port; ///< The port number of the server.
  std::string username;
  std::string password;
  std::string client_id; ///< The client ID. Will automatically be truncated to 23 characters.
};

/** Internal struct for MQTT Home Assistant discovery
 *
 * See <a href="https://home-assistant.io/docs/mqtt/discovery/">MQTT Discovery</a>.
 */
struct MQTTDiscoveryInfo {
  std::string prefix; ///< The Home Assistant discovery prefix. Empty means disabled.
  bool retain; ///< Whether to retain discovery messages.
};

class MQTTClientComponent : public Component {
 public:
  explicit MQTTClientComponent(const MQTTCredentials &credentials);

  /** Set the last will testament message.
   *
   * @param topic The topic.
   * @param payload The payload.
   * @param qos The QoS.
   * @param retain Whether to retain the testament message.
   */
  void set_last_will(const std::string &topic, const std::string &payload, uint8_t qos, bool retain);
  void set_last_will(const MQTTMessage &last_will);
  /// Return the last will used by the MQTT client, returning the default value if necessary.
  MQTTMessage get_last_will() const;
  /// Remove the last will testament message.
  void disable_last_will();
  bool is_last_will_enabled();

  /** Set the birth message.
   *
   * @param topic The topic.
   * @param payload The payload.
   * @param retain Whether to retain the birth message.
   */
  void set_birth_message(const std::string &topic, const std::string &payload, bool retain);
  void set_birth_message(const MQTTMessage &birth_message);
  /// Return the birth message used by the MQTT client, returning the default value if necessary.
  MQTTMessage get_birth_message() const;
  /// Remove the birth message.
  void disable_birth_message();
  bool is_birth_message_enabled();

  /** Set the Home Assistant discovery info
   *
   * See <a href="https://home-assistant.io/docs/mqtt/discovery/">MQTT Discovery</a>.
   * @param prefix The Home Assistant discovery prefix.
   * @param retain Whether to retain discovery messages.
   */
  void set_discovery_info(const std::string &prefix, bool retain);
  void set_discovery_info(const MQTTDiscoveryInfo &discovery_info);
  /// Get Home Assistant discovery info.
  MQTTDiscoveryInfo get_discovery_info() const;
  /// Globally disable Home Assistant discovery.
  void disable_discovery();
  bool is_discovery_enabled();

  /// Helper to efficiently determine whether availability is enabled.
  bool is_availability_enabled();

  /** Set the topic prefix that will be prepended to all topics together with "/". This will, in most cases,
   * be the name of your Application.
   *
   * For example, if "livingroom" is passed to this method, all state topics will, by default, look like
   * "livingroom/.../state"
   *
   * @param topic_prefix The topic prefix. The last "/" is appended automatically.
   */
  void set_topic_prefix(const std::string &topic_prefix);
  /// Get the topic prefix of this device, using default if necessary
  std::string get_topic_prefix() const;

  /** Subscribe to an MQTT topic and call callback when a message is received.
   *
   * @param topic The topic. Wildcards are currently not supported.
   * @param callback The callback function.
   * @param qos The QoS of this subscription.
   */
  void subscribe(const std::string &topic, const mqtt_callback_t &callback, uint8_t qos = 0);

  /** Subscribe to a MQTT topic and automatically parse JSON payload.
   *
   * If an invalid JSON payload is received, the callback will not be called.
   *
   * @param topic The topic. Wildcards are currently not supported.
   * @param callback The callback with a parsed JsonObject that will be called when a message with matching topic is received.
   * @param qos The QoS of this subscription.
   */
  void subscribe_json(const std::string &topic, const json_parse_t &callback, uint8_t qos = 0);

  /** Parse a JSON message and call f if the message is valid JSON.
   *
   * @param message The JSON message.
   * @param f The callback.
   */
  static void parse_json(const std::string &message, const json_parse_t &f);

  /** Publish a MQTTMessage
   *
   * @param message The message.
   */
  void publish(const MQTTMessage &message);

  /** Publish a MQTT message
   *
   * @param topic The topic.
   * @param payload The payload.
   * @param retain Whether to retain the message.
   */
  void publish(const std::string &topic, const std::string &payload, bool retain);

  /// Return whether this client is currently connected to the MQTT server.
  bool is_connected();

  void setup() override;
  void loop() override;
  float get_setup_priority() const override;

 protected:
  static void pub_sub_client_callback(char *topic, uint8_t *payload, unsigned int length);

  void reconnect();

  MQTTCredentials credentials_;
  /// The last will message. Disabled optional denotes it being default and
  /// an empty topic denotes the the feature being disabled.
  Optional<MQTTMessage> last_will_{};
  /// The birth message (e.g. the message that's send on an established connection.
  /// See last_will_ for what different values denote.
  Optional<MQTTMessage> birth_message_{};
  /// The discovery info options for Home Assistant. Undefined optional means
  /// default and empty prefix means disabled.
  Optional<MQTTDiscoveryInfo> discovery_info_{};
  Optional<std::string> topic_prefix_{};
  Optional<bool> availability_enabled_{};

  std::vector<MQTTSubscription> subscriptions_;
  PubSubClient mqtt_client_;
  WiFiClient client_;
};

extern MQTTClientComponent *global_mqtt_client;

} // namespace mqtt

} // namespace esphomelib

#endif //ESPHOMELIB_MQTT_MQTT_CLIENT_COMPONENT_H
