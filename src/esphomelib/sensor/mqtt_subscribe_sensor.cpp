#include "esphomelib/defines.h"

#ifdef USE_MQTT_SUBSCRIBE_SENSOR

#include "esphomelib/sensor/mqtt_subscribe_sensor.h"
#include "esphomelib/mqtt/mqtt_client_component.h"
#include "esphomelib/log.h"
#include <utility>
#include <cstdlib>

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.mqtt_subscribe";

MQTTSubscribeSensor::MQTTSubscribeSensor(const std::string &name, std::string topic)
    : Sensor(name), topic_(std::move(topic)) {

}
void MQTTSubscribeSensor::setup() {
  mqtt::global_mqtt_client->subscribe(this->topic_, [this](std::string payload) {
    char *end;
    float value = ::strtof(payload.c_str(), &end);
    if (end == nullptr) {
      ESP_LOGW(TAG, "Can't convert '%s' to number!", payload.c_str());
      this->push_new_value(NAN);
      return;
    }

    this->push_new_value(value);
  }, this->qos_);
}

float MQTTSubscribeSensor::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
void MQTTSubscribeSensor::set_qos(uint8_t qos) {
  this->qos_ = qos;
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_MQTT_SUBSCRIBE_SENSOR
