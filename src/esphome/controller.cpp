#include "esphome/controller.h"

ESPHOME_NAMESPACE_BEGIN

#ifdef USE_BINARY_SENSOR
void Controller::register_binary_sensor(binary_sensor::BinarySensor *obj) {}

void StoringController::register_binary_sensor(binary_sensor::BinarySensor *obj) {
  this->binary_sensors_.push_back(obj);
}

binary_sensor::BinarySensor *StoringController::get_binary_sensor_by_key(uint32_t key) {
  for (auto *c : this->binary_sensors_) {
    if (c->get_object_id_hash() == key && !c->is_internal())
      return c;
  }
  return nullptr;
}
void StoringUpdateListenerController::register_binary_sensor(binary_sensor::BinarySensor *obj) {
  StoringController::register_binary_sensor(obj);
  obj->add_on_state_callback([this, obj](bool state) { this->on_binary_sensor_update(obj, state); });
}
#endif

#ifdef USE_FAN
void Controller::register_fan(fan::FanState *obj) {}
void StoringController::register_fan(fan::FanState *obj) { this->fans_.push_back(obj); }
fan::FanState *StoringController::get_fan_by_key(uint32_t key) {
  for (auto *c : this->fans_) {
    if (c->get_object_id_hash() == key && !c->is_internal())
      return c;
  }
  return nullptr;
}
void StoringUpdateListenerController::register_fan(fan::FanState *obj) {
  StoringController::register_fan(obj);
  obj->add_on_state_callback([this, obj]() { this->on_fan_update(obj); });
}
#endif

#ifdef USE_LIGHT
void Controller::register_light(light::LightState *obj) {}
void StoringController::register_light(light::LightState *obj) { this->lights_.push_back(obj); }
light::LightState *StoringController::get_light_by_key(uint32_t key) {
  for (auto *c : this->lights_) {
    if (c->get_object_id_hash() == key && !c->is_internal())
      return c;
  }
  return nullptr;
}
void StoringUpdateListenerController::register_light(light::LightState *obj) {
  StoringController::register_light(obj);
  obj->add_new_remote_values_callback([this, obj]() { this->on_light_update(obj); });
}
#endif

#ifdef USE_SENSOR
void Controller::register_sensor(sensor::Sensor *obj) {}
void StoringController::register_sensor(sensor::Sensor *obj) { this->sensors_.push_back(obj); }
sensor::Sensor *StoringController::get_sensor_by_key(uint32_t key) {
  for (auto *c : this->sensors_) {
    if (c->get_object_id_hash() == key && !c->is_internal())
      return c;
  }
  return nullptr;
}
void StoringUpdateListenerController::register_sensor(sensor::Sensor *obj) {
  StoringController::register_sensor(obj);
  obj->add_on_state_callback([this, obj](float state) { this->on_sensor_update(obj, state); });
}
#endif

#ifdef USE_SWITCH
void Controller::register_switch(switch_::Switch *obj) {}
void StoringController::register_switch(switch_::Switch *obj) { this->switches_.push_back(obj); }
switch_::Switch *StoringController::get_switch_by_key(uint32_t key) {
  for (auto *c : this->switches_) {
    if (c->get_object_id_hash() == key && !c->is_internal())
      return c;
  }
  return nullptr;
}
void StoringUpdateListenerController::register_switch(switch_::Switch *obj) {
  StoringController::register_switch(obj);
  obj->add_on_state_callback([this, obj](bool state) { this->on_switch_update(obj, state); });
}
#endif

#ifdef USE_COVER
void Controller::register_cover(cover::Cover *cover) {}
void StoringController::register_cover(cover::Cover *cover) { this->covers_.push_back(cover); }
cover::Cover *StoringController::get_cover_by_key(uint32_t key) {
  for (auto *c : this->covers_) {
    if (c->get_object_id_hash() == key && !c->is_internal())
      return c;
  }
  return nullptr;
}
void StoringUpdateListenerController::register_cover(cover::Cover *obj) {
  StoringController::register_cover(obj);
  obj->add_on_state_callback([this, obj]() { this->on_cover_update(obj); });
}
void StoringUpdateListenerController::on_cover_update(cover::Cover *obj) {}
#endif

#ifdef USE_TEXT_SENSOR
void Controller::register_text_sensor(text_sensor::TextSensor *obj) {}
void StoringController::register_text_sensor(text_sensor::TextSensor *obj) { this->text_sensors_.push_back(obj); }
text_sensor::TextSensor *StoringController::get_text_sensor_by_key(uint32_t key) {
  for (auto *c : this->text_sensors_) {
    if (c->get_object_id_hash() == key && !c->is_internal())
      return c;
  }
  return nullptr;
}
void StoringUpdateListenerController::register_text_sensor(text_sensor::TextSensor *obj) {
  StoringController::register_text_sensor(obj);
  obj->add_on_state_callback([this, obj](std::string state) { this->on_text_sensor_update(obj, state); });
}
#endif
#ifdef USE_CLIMATE
void Controller::register_climate(climate::ClimateDevice *obj) {}
void StoringController::register_climate(climate::ClimateDevice *obj) { this->climates_.push_back(obj); }
climate::ClimateDevice *StoringController::get_climate_by_key(uint32_t key) {
  for (auto *c : this->climates_) {
    if (c->get_object_id_hash() == key && !c->is_internal())
      return c;
  }
  return nullptr;
}
void StoringUpdateListenerController::register_climate(climate::ClimateDevice *obj) {
  StoringController::register_climate(obj);
  obj->add_on_state_callback([this, obj]() { this->on_climate_update(obj); });
}
void StoringUpdateListenerController::on_climate_update(climate::ClimateDevice *obj) {}
#endif

ESPHOME_NAMESPACE_END
