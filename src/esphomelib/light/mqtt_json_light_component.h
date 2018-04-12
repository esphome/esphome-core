//
// Created by Otto Winter on 28.11.17.
//

#ifndef ESPHOMELIB_LIGHT_MQTT_JSON_LIGHT_COMPONENT
#define ESPHOMELIB_LIGHT_MQTT_JSON_LIGHT_COMPONENT

#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/light/light_state.h"
#include "esphomelib/defines.h"

#ifdef USE_LIGHT

namespace esphomelib {

namespace light {

class MQTTJSONLightComponent : public mqtt::MQTTComponent {
 public:
  explicit MQTTJSONLightComponent(std::string friendly_name);

  LightState *get_state() const;
  void set_state(LightState *state);

  /// Defaults to 1 second (1000 ms).
  uint32_t get_default_transition_length() const;
  /// Set the default transition length, i.e. the transition length when no transition is provided.
  void set_default_transition_length(uint32_t default_transition_length);

  void setup() override;

  void loop() override;

 protected:
  std::string component_type() const override;

  void parse_light_json(const JsonObject &root);

  void send_light_values();

  LightState *state_;
  uint32_t default_transition_length_;
  bool next_send_;
};

} // namespace light

} // namespace esphomelib

#endif //USE_LIGHT

#endif //ESPHOMELIB_LIGHT_MQTT_JSON_LIGHT_COMPONENT
