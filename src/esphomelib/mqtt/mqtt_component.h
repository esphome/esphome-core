//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_MQTT_MQTT_COMPONENT_H
#define ESPHOMELIB_MQTT_MQTT_COMPONENT_H

#include <ArduinoJson.h>

#include "esphomelib/component.h"
#include "esphomelib/mqtt/mqtt_client_component.h"

namespace esphomelib {

namespace mqtt {

/** MQTTComponent is the base class for all components that interact with MQTT to expose
 * certain functionality or data from actuators or sensors to clients.
 *
 * Although this class should work with all MQTT solutions, it has been specifically designed for use
 * with Home Assistant. For example, this class supports Home Assistant MQTT discovery out of the box.
 *
 * In order to implement automatic Home Assistant discovery, all sub-classes should:
 *
 *  1. Call send_discovery() with a callback that adds discovery information during setup().
 *  2. Override component_type() to return the appropriate component type such as "light" or "sensor".
 *  3. Subscribe to command topics using subscribe() or subscribe_json() during setup().
 *
 * In order to best separate the front- and back-end of esphomelib, all sub-classes should
 * only parse/send MQTT messages and interact with back-end components via callbacks to ensure
 * a clean separation.
 */
class MQTTComponent : public Component {
 public:
  /// Constructs a MQTTComponent.
  explicit MQTTComponent();

  /// Set whether state message should be retained.
  void set_retain(bool retain);
  bool get_retain() const;

  /// Disable discovery. Sets friendly name to "".
  void disable_discovery();
  bool is_discovery_enabled() const;

  /// Override this method to return the component type (e.g. "light", "sensor", ...)
  virtual std::string component_type() const = 0;

  /// Set a custom state topic. Set to "" for default behavior.
  void set_custom_state_topic(const std::string &custom_state_topic);
  /// Set a custom command topic. Set to "" for default behavior.
  void set_custom_command_topic(const std::string &custom_command_topic);
  void set_custom_topic(const std::string &key, const std::string &custom_topic);

  float get_setup_priority() const override;

  /** Set the Home Assistant availability data.
   *
   * See See <a href="https://home-assistant.io/components/binary_sensor.mqtt/">Home Assistant</a> for more info.
   */
  void set_availability(std::string topic, std::string payload_available, std::string payload_not_available);
  void disable_availability();
  
 protected:

  /// Helper method to get the discovery topic for this component.
  virtual std::string get_discovery_topic(const MQTTDiscoveryInfo &discovery_info) const;

  /** Get this components state/command/... topic.
   *
   * @param suffix The suffix/key such as "state" or "command".
   * @return The full topic.
   */
  virtual std::string get_default_topic_for(const std::string &suffix) const;

  /// Get the friendly name of this MQTT component.
  virtual std::string friendly_name() const = 0;

  /// Get the MQTT topic that new states will be shared to.
  const std::string get_state_topic() const;

  /// Get the MQTT topic for listening to commands.
  const std::string get_command_topic() const;

  /// Get the MQTT topic for a specific suffix/key, if a custom topic has been defined, that one will be used.
  /// Otherwise, one will be generated with get_default_topic_for().
  const std::string get_topic_for(const std::string &key) const;

  /** Send discovery info.
   *
   * Sends discovery info to the topic returned by get_discovery_topic() using the specified component type ("light",
   * "sensor", ...). Automatically populates "name" and "platform" which defaults to "mqtt" but can be overridden.
   * If state_topic is set to true, the state topic from get_state_topic() is aditionally included. The same applies to
   * command_topic. Aditionally a "platform" can be specified to specify which platform should
   *
   * @param f This method will be called for building the discovery info.
   * @param state_topic Whether to include "state_topic".
   * @param command_topic Whether to include "command_topic".
   * @param platform
   */
  void send_discovery(const json_build_t &f,
                      bool state_topic = true,
                      bool command_topic = true,
                      const std::string &platform = "mqtt");

  /** Send a MQTT message.
   *
   * @param topic The topic.
   * @param payload The payload.
   * @param retain Whether to retain the message. If not set, defaults to get_retain.
   */
  void send_message(const std::string &topic,
                    const std::string &payload,
                    const Optional<bool> &retain = Optional<bool>());

  /** Construct and send a JSON MQTT message.
   *
   * @param topic The topic.
   * @param f The Json Message builder.
   * @param retain Whether to retain the message.
   */
  void send_json_message(const std::string &topic,
                         const json_build_t &f,
                         const Optional<bool> &retain = Optional<bool>());

  /** Subscribe to a MQTT topic.
   *
   * @param topic The topic. Wildcards are currently not supported.
   * @param callback The callback that will be called when a message with matching topic is received.
   * @param qos The MQTT quality of service. Defaults to 0.
   */
  void subscribe(const std::string &topic, mqtt_callback_t callback, uint8_t qos = 0);

  /** Subscribe to a MQTT topic and automatically parse JSON payload.
   *
   * If an invalid JSON payload is received, the callback will not be called.
   *
   * @param topic The topic. Wildcards are currently not supported.
   * @param callback The callback with a parsed JsonObject that will be called when a message with matching topic is received.
   * @param qos The MQTT quality of service. Defaults to 0.
   */
  void subscribe_json(const std::string &topic, json_parse_t callback, uint8_t qos = 0);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Generate the Home Assistant MQTT discovery object id by automatically transforming the friendly name.
  std::string get_default_object_id() const;

 protected:
  std::map<std::string, std::string> custom_topics_{};
  bool retain_{true};
  bool discovery_enabled_{true};
  Availability *availability_{nullptr};
};

} // namespace mqtt

} // namespace esphomelib

#endif //ESPHOMELIB_MQTT_MQTT_COMPONENT_H
