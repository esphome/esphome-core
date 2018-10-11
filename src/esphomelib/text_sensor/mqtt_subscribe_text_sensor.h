//
//  mqtt_subscribe_text_sensor.h
//  esphomelib
//
//  Created by Otto Winter on 2018/10/11.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_MQTT_SUBSCRIBE_TEXT_SENSOR_H
#define ESPHOMELIB_MQTT_SUBSCRIBE_TEXT_SENSOR_H

#include "esphomelib/defines.h"

#ifdef USE_MQTT_SUBSCRIBE_TEXT_SENSOR

#include "esphomelib/component.h"
#include "esphomelib/text_sensor/text_sensor.h"
#include "esphomelib/mqtt/mqtt_client_component.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

class MQTTSubscribeTextSensor : public TextSensor, public Component {
 public:
  MQTTSubscribeTextSensor(const std::string &name, std::string topic);

  void setup() override;
  float get_setup_priority() const override;
  void set_qos(uint8_t qos);

 protected:
  std::string topic_;
  uint8_t qos_{0};
};

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_MQTT_SUBSCRIBE_TEXT_SENSOR

#endif //ESPHOMELIB_MQTT_SUBSCRIBE_TEXT_SENSOR_H
