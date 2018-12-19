#ifndef ESPHOMELIB_COVER_MQTT_COVER_COMPONENT_H
#define ESPHOMELIB_COVER_MQTT_COVER_COMPONENT_H

#include "esphomelib/defines.h"

#ifdef USE_COVER

#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/cover/cover.h"

ESPHOMELIB_NAMESPACE_BEGIN

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

} // namespace cover

ESPHOMELIB_NAMESPACE_END

#endif //USE_COVER

#endif //ESPHOMELIB_COVER_MQTT_COVER_COMPONENT_H
