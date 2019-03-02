#ifndef ESPHOME_LIGHT_MQTT_JSON_LIGHT_COMPONENT
#define ESPHOME_LIGHT_MQTT_JSON_LIGHT_COMPONENT

#include "esphome/defines.h"

#ifdef USE_MQTT_LIGHT

#include "esphome/mqtt/mqtt_component.h"
#include "esphome/light/light_state.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

class MQTTJSONLightComponent : public mqtt::MQTTComponent {
 public:
  explicit MQTTJSONLightComponent(LightState *state);

  LightState *get_state() const;

  void setup() override;

  void dump_config() override;

  void send_discovery(JsonObject &root, mqtt::SendDiscoveryConfig &config) override;

  bool send_initial_state() override;

  bool is_internal() override;

 protected:
  std::string friendly_name() const override;
  std::string component_type() const override;

  bool publish_state_();

  LightState *state_;
};

}  // namespace light

ESPHOME_NAMESPACE_END

#endif  // USE_MQTT_LIGHT

#endif  // ESPHOME_LIGHT_MQTT_JSON_LIGHT_COMPONENT
