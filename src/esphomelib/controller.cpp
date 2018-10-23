#include "esphomelib/controller.h"

ESPHOMELIB_NAMESPACE_BEGIN

#ifdef USE_BINARY_SENSOR
void Controller::register_binary_sensor(binary_sensor::BinarySensor *obj) {

}

void StoringController::register_binary_sensor(esphomelib::binary_sensor::BinarySensor *obj) {
  this->binary_sensors_.push_back(obj);
}
#endif

#ifdef USE_FAN
void Controller::register_fan(fan::FanState *obj) {

}
void StoringController::register_fan(fan::FanState *obj) {
  this->fans_.push_back(obj);
}
#endif

#ifdef USE_LIGHT
void Controller::register_light(light::LightState *obj) {

}
void StoringController::register_light(light::LightState *obj) {
  this->lights_.push_back(obj);
}
#endif

#ifdef USE_SENSOR
void Controller::register_sensor(sensor::Sensor *obj) {

}
void StoringController::register_sensor(sensor::Sensor *obj) {
  this->sensors_.push_back(obj);
}
#endif

#ifdef USE_SWITCH
void Controller::register_switch(switch_::Switch *obj) {

}
void StoringController::register_switch(switch_::Switch *obj) {
  this->switches_.push_back(obj);
}
#endif

#ifdef USE_COVER
void Controller::register_cover(cover::Cover *cover) {

}
void StoringController::register_cover(cover::Cover *cover) {
  this->covers_.push_back(cover);
}
#endif

#ifdef USE_TEXT_SENSOR
void Controller::register_text_sensor(text_sensor::TextSensor *obj) {

}
void StoringController::register_text_sensor(text_sensor::TextSensor *obj) {
  this->text_sensors_.push_back(obj);
}
#endif

ESPHOMELIB_NAMESPACE_END
