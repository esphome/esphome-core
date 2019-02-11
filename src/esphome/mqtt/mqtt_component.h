#ifndef ESPHOME_MQTT_MQTT_COMPONENT_H
#define ESPHOME_MQTT_MQTT_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_MQTT

#include "esphome/component.h"
#include "esphome/mqtt/mqtt_client_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace mqtt {

/// Simple Helper struct used for Home Assistant MQTT send_discovery().
struct SendDiscoveryConfig {
  bool state_topic{true}; ///< If the state topic should be included. Defaults to true.
  bool command_topic{true}; ///< If the command topic should be included. Default to true.
  const char *platform{"mqtt"}; ///< The platform of this component. Defaults to "mqtt".
};

#define LOG_MQTT_COMPONENT(state_topic, command_topic) \
    if (state_topic) { ESP_LOGCONFIG(TAG, "  State Topic: '%s'", this->get_state_topic().c_str()); } \
    if (command_topic) { ESP_LOGCONFIG(TAG, "  Command Topic: '%s'", this->get_command_topic().c_str()); } \


/** MQTTComponent is the base class for all components that interact with MQTT to expose
 * certain functionality or data from actuators or sensors to clients.
 *
 * Although this class should work with all MQTT solutions, it has been specifically designed for use
 * with Home Assistant. For example, this class supports Home Assistant MQTT discovery out of the box.
 *
 * In order to implement automatic Home Assistant discovery, all sub-classes should:
 *
 *  1. Implement send_discovery that creates a Home Assistant discovery payload.
 *  2. Override component_type() to return the appropriate component type such as "light" or "sensor".
 *  3. Subscribe to command topics using subscribe() or subscribe_json() during setup().
 *
 * In order to best separate the front- and back-end of ESPHome, all sub-classes should
 * only parse/send MQTT messages and interact with back-end components via callbacks to ensure
 * a clean separation.
 */
class MQTTComponent : public Component {
 public:
  /// Constructs a MQTTComponent.
  explicit MQTTComponent();

  /// Override setup_ so that we can call send_discovery() when needed.
  void setup_() override;

  void loop_() override;

  /// Send discovery info the Home Assistant, override this.
  virtual void send_discovery(JsonObject &root, SendDiscoveryConfig &config) = 0;

  virtual bool send_initial_state() = 0;

  virtual bool is_internal() = 0;

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

  /// MQTT_COMPONENT setup priority.
  float get_setup_priority() const override;

  /** Set the Home Assistant availability data.
   *
   * See See <a href="https://www.home-assistant.io/components/binary_sensor.mqtt/">Home Assistant</a> for more info.
   */
  void set_availability(std::string topic, std::string payload_available, std::string payload_not_available);
  void disable_availability();

  /// Internal method for the MQTT client base to schedule a resend of the state on reconnect.
  void schedule_resend_state();
  
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

  /** A unique ID for this MQTT component, empty for no unique id. See unique ID requirements:
   * https://developers.home-assistant.io/docs/en/entity_registry_index.html#unique-id-requirements
   *
   * @return The unique id as a string.
   */
  virtual std::string unique_id();

  /// Get the MQTT topic that new states will be shared to.
  const std::string get_state_topic() const;

  /// Get the MQTT topic for listening to commands.
  const std::string get_command_topic() const;

  bool is_connected() const;

  /// Internal method to start sending discovery info, this will call send_discovery().
  bool send_discovery_();

  /** Send a MQTT message.
   *
   * @param topic The topic.
   * @param payload The payload.
   */
  bool publish(const std::string &topic, const std::string &payload);

  /** Construct and send a JSON MQTT message.
   *
   * @param topic The topic.
   * @param f The Json Message builder.
   */
  bool publish_json(const std::string &topic, const json_build_t &f);

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
  void subscribe_json(const std::string &topic, mqtt_json_callback_t callback, uint8_t qos = 0);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Generate the Home Assistant MQTT discovery object id by automatically transforming the friendly name.
  std::string get_default_object_id() const;

 protected:
  std::string custom_state_topic_{};
  std::string custom_command_topic_{};
  bool retain_{true};
  bool discovery_enabled_{true};
  Availability *availability_{nullptr};
  bool resend_state_{false};
};

} // namespace mqtt

ESPHOME_NAMESPACE_END

#endif //USE_MQTT

#endif //ESPHOME_MQTT_MQTT_COMPONENT_H
