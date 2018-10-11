//
//  mqtt_subscribe_text_sensor.cpp
//  esphomelib
//
//  Created by Otto Winter on 2018/10/11.
//  Copyright © 2018 Otto Winter. All rights reserved.
//


#include "esphomelib/defines.h"

#ifdef USE_MQTT_SUBSCRIBE_TEXT_SENSOR

#include "esphomelib/text_sensor/mqtt_subscribe_text_sensor.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace text_sensor {

MQTTSubscribeTextSensor::MQTTSubscribeTextSensor(const std::string &name, std::string topic)
    : TextSensor(name), topic_(std::move(topic)) {

}
void MQTTSubscribeTextSensor::setup() {
  mqtt::global_mqtt_client->subscribe(this->topic_, [this](std::string payload) {
    this->push_new_value(payload);
  }, this->qos_);
}
float MQTTSubscribeTextSensor::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
void MQTTSubscribeTextSensor::set_qos(uint8_t qos) {
  this->qos_ = qos;
}

} // namespace text_sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_MQTT_SUBSCRIBE_TEXT_SENSOR
