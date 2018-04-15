//
//  web_server.h
//  esphomelib
//
//  Created by Otto Winter on 14.04.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_WEB_SERVER_H
#define ESPHOMELIB_WEB_SERVER_H

#include "esphomelib/component.h"
#include "esphomelib/controller.h"
#include "esphomelib/switch_/switch.h"
#include "esphomelib/defines.h"

#include <vector>

#ifdef USE_WEB_SERVER

#include <ESPAsyncWebServer.h>

namespace esphomelib {

struct UrlMatch {
  std::string domain;
  std::string id;
  std::string method;
  bool valid;
};

class WebServer : public StoringController, public Component, public AsyncWebHandler {
 public:
  explicit WebServer(uint16_t port);
  void setup() override;
#ifdef USE_SENSOR
  void register_sensor(sensor::Sensor *obj) override;

  void send_sensor_event(sensor::Sensor *obj, float value);

  void handle_sensor_request(AsyncWebServerRequest *request, UrlMatch match);

  std::string sensor_json(sensor::Sensor *obj, float value);
#endif

#ifdef USE_SWITCH
  void register_switch(switch_::Switch *obj) override;

  void handle_switch_request(AsyncWebServerRequest *request, UrlMatch match);

  std::string switch_json(switch_::Switch *obj, bool value);
#endif

#ifdef USE_BINARY_SENSOR
  void register_binary_sensor(binary_sensor::BinarySensor *obj) override;

  void handle_binary_sensor_request(AsyncWebServerRequest *request, UrlMatch match);

  std::string binary_sensor_json(binary_sensor::BinarySensor *obj, bool value);
#endif

#ifdef USE_FAN
  void register_fan(fan::FanState *obj) override;

  void handle_fan_request(AsyncWebServerRequest *request, UrlMatch match);

  std::string fan_json(fan::FanState *obj);
#endif

#ifdef USE_LIGHT
  void register_light(light::LightState *obj) override;

  void handle_light_request(AsyncWebServerRequest *request, UrlMatch match);

  std::string light_json(light::LightState *obj);
#endif

  float get_setup_priority() const override;

  bool canHandle(AsyncWebServerRequest *request) override;
  void handleRequest(AsyncWebServerRequest *request) override;
  bool isRequestHandlerTrivial() override;

 protected:
  AsyncWebServer server_;
  AsyncEventSource events_{"/events"};
};

} // namespace esphomelib

#endif //USE_WEB_SERVER

#endif //ESPHOMELIB_WEB_SERVER_H
