//
//  controller.h
//  esphomelib
//
//  Created by Otto Winter on 12.04.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_CONTROLLER_H
#define ESPHOMELIB_CONTROLLER_H

#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/fan/fan_state.h"
#include "esphomelib/light/light_state.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/switch_/switch.h"
#include "esphomelib/defines.h"

ESPHOMELIB_NAMESPACE_BEGIN

/// Controllers allow an object to be notified of every component that's added to the Application.
class Controller {
 public:
#ifdef USE_BINARY_SENSOR
  virtual void register_binary_sensor(binary_sensor::BinarySensor *obj);
#endif

#ifdef USE_FAN
  virtual void register_fan(fan::FanState *obj);
#endif

#ifdef USE_LIGHT
  virtual void register_light(light::LightState *obj);
#endif

#ifdef USE_SENSOR
  virtual void register_sensor(sensor::Sensor *obj);
#endif

#ifdef USE_SWITCH
  virtual void register_switch(switch_::Switch *obj);
#endif
};

/// A StoringController is a controller that automatically stores all components internally in vectors.
class StoringController : public Controller {
 public:
#ifdef USE_BINARY_SENSOR
  void register_binary_sensor(binary_sensor::BinarySensor *obj) override {
    this->binary_sensors_.push_back(obj);
  }
#endif

#ifdef USE_FAN
  void register_fan(fan::FanState *obj) override {
    this->fans_.push_back(obj);
  }
#endif

#ifdef USE_LIGHT
  void register_light(light::LightState *obj) override {
    this->lights_.push_back(obj);
  }
#endif

#ifdef USE_SENSOR
  void register_sensor(sensor::Sensor *obj) override {
    this->sensors_.push_back(obj);
  }
#endif

#ifdef USE_SWITCH
  void register_switch(switch_::Switch *obj) override {
    this->switches_.push_back(obj);
  }
#endif

 protected:
#ifdef USE_BINARY_SENSOR
  std::vector<binary_sensor::BinarySensor *> binary_sensors_;
#endif

#ifdef USE_FAN
  std::vector<fan::FanState *> fans_;
#endif

#ifdef USE_LIGHT
  std::vector<light::LightState *> lights_;
#endif

#ifdef USE_SENSOR
  std::vector<sensor::Sensor *> sensors_;
#endif

#ifdef USE_SWITCH
  std::vector<switch_::Switch *> switches_;
#endif
};

ESPHOMELIB_NAMESPACE_END

#endif //ESPHOMELIB_CONTROLLER_H
