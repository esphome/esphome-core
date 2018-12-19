#ifndef ESPHOMELIB_LIGHT_MQTT_JSON_LIGHT_COMPONENT
#define ESPHOMELIB_LIGHT_MQTT_JSON_LIGHT_COMPONENT

#include "esphomelib/defines.h"

#ifdef USE_LIGHT

#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/light/light_state.h"

ESPHOMELIB_NAMESPACE_BEGIN

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

  bool publish_state();

  LightState *state_;
};

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_LIGHT

#endif //ESPHOMELIB_LIGHT_MQTT_JSON_LIGHT_COMPONENT
