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
  explicit MQTTJSONLightComponent(LightState *state);

  LightState *get_state() const;

  void setup() override;

  void loop() override;

 protected:
  std::string friendly_name() const override;
  std::string component_type() const override;

  void send_light_values();

  LightState *state_;
  bool next_send_{true};
};

} // namespace light

} // namespace esphomelib

#endif //USE_LIGHT

#endif //ESPHOMELIB_LIGHT_MQTT_JSON_LIGHT_COMPONENT
