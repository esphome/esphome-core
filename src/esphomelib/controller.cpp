//
//  controller.cpp
//  esphomelib
//
//  Created by Otto Winter on 12.04.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/controller.h"

ESPHOMELIB_NAMESPACE_BEGIN

#ifdef USE_BINARY_SENSOR
void Controller::register_binary_sensor(binary_sensor::BinarySensor *obj) {

}
#endif

#ifdef USE_FAN
void Controller::register_fan(fan::FanState *obj) {

}
#endif

#ifdef USE_LIGHT
void Controller::register_light(light::LightState *obj) {

}
#endif

#ifdef USE_SENSOR
void Controller::register_sensor(sensor::Sensor *obj) {

}
#endif

#ifdef USE_SWITCH
void Controller::register_switch(switch_::Switch *obj) {

}
#endif

ESPHOMELIB_NAMESPACE_END
