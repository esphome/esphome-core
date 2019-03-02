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

  void dump_config() override;

 protected:
  std::string component_type() const override;
  std::string friendly_name() const override;

  Cover *cover_;
};

}  // namespace cover

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_COVER

#endif  // ESPHOME_COVER_MQTT_COVER_COMPONENT_H
