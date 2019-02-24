#include "esphome/defines.h"

#ifdef USE_API

#include "esphome/api/list_entities.h"
#include "esphome/util.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace api {

std::string get_default_unique_id(const std::string &component_type, Nameable *nameable) {
  return get_app_name() + component_type + nameable->get_object_id();
}

#ifdef USE_BINARY_SENSOR
bool ListEntitiesIterator::on_binary_sensor(binary_sensor::BinarySensor *binary_sensor) {
  auto buffer = this->client_->get_buffer();
  buffer.encode_nameable(binary_sensor);
  // string unique_id = 4;
  buffer.encode_string(4, get_default_unique_id("binary_sensor", binary_sensor));
  // string device_class = 5;
  buffer.encode_string(5, binary_sensor->get_device_class());
  // bool is_status_binary_sensor = 6;
  buffer.encode_bool(6, binary_sensor->is_status_binary_sensor());
  return this->client_->send_buffer(APIMessageType::LIST_ENTITIES_BINARY_SENSOR_RESPONSE);
}
#endif
#ifdef USE_COVER
bool ListEntitiesIterator::on_cover(cover::Cover *cover) {
  auto buffer = this->client_->get_buffer();
  buffer.encode_nameable(cover);
  // string unique_id = 4;
  buffer.encode_string(4, get_default_unique_id("cover", cover));
  // bool assumed_state = 5;
  buffer.encode_bool(5, cover->assumed_state());
  return this->client_->send_buffer(APIMessageType::LIST_ENTITIES_COVER_RESPONSE);
}
#endif
#ifdef USE_FAN
bool ListEntitiesIterator::on_fan(fan::FanState *fan) {
  auto buffer = this->client_->get_buffer();
  buffer.encode_nameable(fan);
  // string unique_id = 4;
  buffer.encode_string(4, get_default_unique_id("fan", fan));
  // bool supports_oscillation = 5;
  buffer.encode_bool(5, fan->get_traits().supports_oscillation());
  // bool supports_speed = 6;
  buffer.encode_bool(6, fan->get_traits().supports_speed());
  return this->client_->send_buffer(APIMessageType::LIST_ENTITIES_FAN_RESPONSE);
}
#endif
#ifdef USE_LIGHT
bool ListEntitiesIterator::on_light(light::LightState *light) {
  auto buffer = this->client_->get_buffer();
  buffer.encode_nameable(light);
  // string unique_id = 4;
  buffer.encode_string(4, get_default_unique_id("light", light));
  // bool supports_brightness = 5;
  buffer.encode_bool(5, light->get_traits().has_brightness());
  // bool supports_rgb = 6;
  buffer.encode_bool(6, light->get_traits().has_rgb());
  // bool supports_white_value = 7;
  buffer.encode_bool(7, light->get_traits().has_rgb_white_value());
  // bool supports_color_temperature = 8;
  buffer.encode_bool(8, light->get_traits().has_color_temperature());
  // float min_mireds = 9;
  buffer.encode_float(9, light->get_traits().get_min_mireds());
  // float max_mireds = 10;
  buffer.encode_float(10, light->get_traits().get_max_mireds());
  // repeated string effects = 11;
  if (light->supports_effects()) {
    buffer.encode_string(11, "None");
    for (auto *effect : light->get_effects()) {
      buffer.encode_string(11, effect->get_name());
    }
  }
  return this->client_->send_buffer(APIMessageType::LIST_ENTITIES_LIGHT_RESPONSE);
}
#endif
#ifdef USE_SENSOR
bool ListEntitiesIterator::on_sensor(sensor::Sensor *sensor) {
  auto buffer = this->client_->get_buffer();
  buffer.encode_nameable(sensor);
  // string unique_id = 4;
  std::string unique_id = sensor->unique_id();
  if (unique_id.empty())
    unique_id = get_default_unique_id("sensor", sensor);
  buffer.encode_string(4, unique_id);
  // string icon = 5;
  buffer.encode_string(5, sensor->get_icon());
  // string unit_of_measurement = 6;
  buffer.encode_string(6, sensor->get_unit_of_measurement());
  // int32 accuracy_decimals = 7;
  buffer.encode_int32(7, sensor->get_accuracy_decimals());
  return this->client_->send_buffer(APIMessageType::LIST_ENTITIES_SENSOR_RESPONSE);
}
#endif
#ifdef USE_SWITCH
bool ListEntitiesIterator::on_switch(switch_::Switch *switch_) {
  auto buffer = this->client_->get_buffer();
  buffer.encode_nameable(switch_);
  // string unique_id = 4;
  buffer.encode_string(4, get_default_unique_id("switch", switch_));
  // string icon = 5;
  buffer.encode_string(5, switch_->get_icon());
  // bool assumed_state = 6;
  buffer.encode_bool(6, switch_->assumed_state());
  return this->client_->send_buffer(APIMessageType::LIST_ENTITIES_SWITCH_RESPONSE);
}
#endif
#ifdef USE_TEXT_SENSOR
bool ListEntitiesIterator::on_text_sensor(text_sensor::TextSensor *text_sensor) {
  auto buffer = this->client_->get_buffer();
  buffer.encode_nameable(text_sensor);
  // string unique_id = 4;
  std::string unique_id = text_sensor->unique_id();
  if (unique_id.empty())
    unique_id = get_default_unique_id("text_sensor", text_sensor);
  buffer.encode_string(4, unique_id);
  // string icon = 5;
  buffer.encode_string(5, text_sensor->get_icon());
  return this->client_->send_buffer(APIMessageType::LIST_ENTITIES_TEXT_SENSOR_RESPONSE);
}
#endif

bool ListEntitiesIterator::on_end() {
  return this->client_->send_empty_message(APIMessageType::LIST_ENTITIES_DONE_RESPONSE);
}
ListEntitiesIterator::ListEntitiesIterator(APIServer *server, APIConnection *client)
    : ComponentIterator(server), client_(client) {

}
bool ListEntitiesIterator::on_service(UserServiceDescriptor *service) {
  auto buffer = this->client_->get_buffer();
  service->encode_list_service_response(buffer);
  return this->client_->send_buffer(APIMessageType::LIST_SERVICES_SERVICE_RESPONSE);
}

APIMessageType ListEntitiesRequest::message_type() const {
  return APIMessageType::LIST_ENTITIES_REQUEST;
}

} // namespace api

ESPHOME_NAMESPACE_END

#endif //USE_API
