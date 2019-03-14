#ifndef ESPHOME_COVER_MQTT_COVER_COMPONENT_H
#define ESPHOME_COVER_MQTT_COVER_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_MQTT_COVER

#include "esphome/mqtt/mqtt_component.h"
#include "esphome/cover/cover.h"

ESPHOME_NAMESPACE_BEGIN

namespace cover {

class MQTTCoverComponent : public mqtt::MQTTComponent {
 public:
  explicit MQTTCoverComponent(Cover *cover);

  void setup() override;
  void send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) override;

  bool send_initial_state() override;
  bool is_internal() override;

  bool publish_state(cover::CoverState state);
  bool publish_position(cover::position_value_t position_value);
  bool publish_tilt(cover::tilt_value_t tilt_value);

  void dump_config() override;

  void set_custom_command_topic(const std::string &topic);
  void set_custom_state_topic(const std::string &topic);
  void set_custom_position_command_topic(const std::string &topic);
  void set_custom_position_state_topic(const std::string &topic);
  void set_custom_tilt_command_topic(const std::string &topic);
  void set_custom_tilt_status_topic(const std::string &topic);

  const std::string command_topic() const;
  const std::string state_topic() const;
  const std::string position_command_topic() const;
  const std::string position_state_topic() const;
  const std::string tilt_command_topic() const;
  const std::string tilt_status_topic() const;

 protected:
  std::string component_type() const override;
  std::string friendly_name() const override;
  std::string custom_command_topic_;
  std::string custom_state_topic_;
  std::string custom_position_command_topic_;
  std::string custom_position_state_topic_;
  std::string custom_tilt_command_topic_;
  std::string custom_tilt_status_topic_;

  Cover *cover_;
};

}  // namespace cover

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_COVER

#endif  // ESPHOME_COVER_MQTT_COVER_COMPONENT_H
