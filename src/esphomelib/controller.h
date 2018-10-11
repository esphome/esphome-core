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
#include "esphomelib/cover/cover.h"
#include "esphomelib/text_sensor/text_sensor.h"
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

#ifdef USE_COVER
 virtual void register_cover(cover::Cover *cover);
#endif

#ifdef USE_TEXT_SENSOR
  virtual void register_text_sensor(text_sensor::TextSensor *obj);
#endif
};

/// A StoringController is a controller that automatically stores all components internally in vectors.
class StoringController : public Controller {
 public:
#ifdef USE_BINARY_SENSOR
  void register_binary_sensor(binary_sensor::BinarySensor *obj) override;
#endif

#ifdef USE_FAN
  void register_fan(fan::FanState *obj) override;
#endif

#ifdef USE_LIGHT
  void register_light(light::LightState *obj) override;
#endif

#ifdef USE_SENSOR
  void register_sensor(sensor::Sensor *obj) override;
#endif

#ifdef USE_SWITCH
  void register_switch(switch_::Switch *obj) override;
#endif

#ifdef USE_COVER
  void register_cover(cover::Cover *cover) override;
#endif

#ifdef USE_TEXT_SENSOR
  void register_text_sensor(text_sensor::TextSensor *obj) override;
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

#ifdef USE_COVER
  std::vector<cover::Cover *> covers_;
#endif

#ifdef USE_TEXT_SENSOR
  std::vector<text_sensor::TextSensor *> text_sensors_;
#endif
};

ESPHOMELIB_NAMESPACE_END

#endif //ESPHOMELIB_CONTROLLER_H
