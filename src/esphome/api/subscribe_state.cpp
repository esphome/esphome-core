#include "esphome/defines.h"

#ifdef USE_API

#include "esphome/api/subscribe_state.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace api {

#ifdef USE_BINARY_SENSOR
bool InitialStateIterator::on_binary_sensor(binary_sensor::BinarySensor *binary_sensor) {
  if (!binary_sensor->has_state())
    return true;

  return this->client_->send_binary_sensor_state(binary_sensor, binary_sensor->state);
}
#endif
#ifdef USE_COVER
bool InitialStateIterator::on_cover(cover::Cover *cover) {
  if (!cover->has_state())
    return true;

  return this->client_->send_cover_state(cover);
}
#endif
#ifdef USE_FAN
bool InitialStateIterator::on_fan(fan::FanState *fan) {
  return this->client_->send_fan_state(fan);
}
#endif
#ifdef USE_LIGHT
bool InitialStateIterator::on_light(light::LightState *light) {
  return this->client_->send_light_state(light);
}
#endif
#ifdef USE_SENSOR
bool InitialStateIterator::on_sensor(sensor::Sensor *sensor) {
  if (!sensor->has_state())
    return true;

  return this->client_->send_sensor_state(sensor, sensor->state);
}
#endif
#ifdef USE_SWITCH
bool InitialStateIterator::on_switch(switch_::Switch *switch_) {
  return this->client_->send_switch_state(switch_, switch_->state);
}
#endif
#ifdef USE_TEXT_SENSOR
bool InitialStateIterator::on_text_sensor(text_sensor::TextSensor *text_sensor) {
  if (!text_sensor->has_state())
    return true;

  return this->client_->send_text_sensor_state(text_sensor, text_sensor->state);
}
#endif
InitialStateIterator::InitialStateIterator(APIServer *server, APIConnection *client)
    : ComponentIterator(server), client_(client) {

}

APIMessageType SubscribeStatesRequest::message_type() const {
  return APIMessageType::SUBSCRIBE_STATES_REQUEST;
}

bool HomeAssistantStateResponse::decode_length_delimited(uint32_t field_id, const uint8_t *value, size_t len) {
  switch (field_id) {
    case 1:
      // string entity_id = 1;
      this->entity_id_ = as_string(value, len);
      return true;
    case 2:
      // string state = 2;
      this->state_ = as_string(value, len);
      return true;
    default:
      return false;
  }
}
APIMessageType HomeAssistantStateResponse::message_type() const {
  return APIMessageType::HOME_ASSISTANT_STATE_RESPONSE;
}
const std::string &HomeAssistantStateResponse::get_entity_id() const {
  return this->entity_id_;
}
const std::string &HomeAssistantStateResponse::get_state() const {
  return this->state_;
}
APIMessageType SubscribeHomeAssistantStatesRequest::message_type() const {
  return APIMessageType::SUBSCRIBE_HOME_ASSISTANT_STATES_REQUEST;
}

} // namespace api

ESPHOME_NAMESPACE_END

#endif //USE_API
