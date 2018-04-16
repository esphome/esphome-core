//
//  web_server.cpp
//  esphomelib
//
//  Created by Otto Winter on 14.04.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/web_server.h"
#include "esphomelib/application.h"

#ifdef USE_WEB_SERVER

#ifdef ARDUINO_ARCH_ESP32
  #include <ESPmDNS.h>
#endif
#ifdef ARDUINO_ARCH_ESP8266
  #include <ESP8266mDNS.h>
#endif

#include <cstdlib>

namespace esphomelib {

static const char *TAG = "web_server";


void write_row(AsyncResponseStream *stream, Nameable *obj,
               const std::string &klass, const std::string &action) {
  stream->print("<tr class=\"");
  stream->print(klass.c_str());
  stream->print("\" id=\"");
  stream->print(klass.c_str());
  stream->print("-");
  stream->print(obj->get_name_id().c_str());
  stream->print("\"><td>");
  stream->print(obj->get_name().c_str());
  stream->print("</td><td></td><td>");
  stream->print(action.c_str());
  stream->print("</td>");
}

UrlMatch match_url(const std::string &url, bool only_domain = false) {
  UrlMatch match;
  match.valid = false;
  size_t domain_end = url.find('/', 1);
  if (domain_end == std::string::npos)
    return match;
  match.domain = url.substr(1, domain_end - 1);
  if (only_domain) {
    match.valid = true;
    return match;
  }
  if (url.length() == domain_end - 1)
    return match;
  size_t id_begin = domain_end + 1;
  size_t id_end = url.find('/', id_begin);
  match.valid = true;
  if (id_end == std::string::npos) {
    match.id = url.substr(id_begin, url.length() - id_begin);
    return match;
  }
  match.id = url.substr(id_begin, id_end - id_begin);
  size_t method_begin = id_end + 1;
  match.method = url.substr(method_begin, url.length() - method_begin);
  return match;
}

void WebServer::setup() {
  this->server_ = new AsyncWebServer(this->port_);
  MDNS.addService("http", "tcp", this->port_);

  this->events_.onConnect([this](AsyncEventSourceClient *client) {
    // Configure reconnect timeout
    client->send("", "ping", millis(), 30000);

#ifdef USE_SENSOR
    for (auto *obj : this->sensors_)
      client->send(this->sensor_json(obj, obj->get_value()).c_str(), "state");
#endif

#ifdef USE_SWITCH
    for (auto *obj : this->switches_)
      client->send(this->switch_json(obj, obj->get_value()).c_str(), "state");
#endif

#ifdef USE_BINARY_SENSOR
    for (auto *obj : this->binary_sensors_)
      client->send(this->binary_sensor_json(obj, obj->get_value()).c_str(), "state");
#endif

#ifdef USE_FAN
    for (auto *obj : this->fans_)
      client->send(this->fan_json(obj).c_str(), "state");
#endif

#ifdef USE_LIGHT
    for (auto *obj : this->lights_)
      client->send(this->light_json(obj).c_str(), "state");
#endif
  });

  if (global_log_component != nullptr)
    global_log_component->add_on_log_callback([this](ESPLogLevel level, const char *message) {
      this->events_.send(message, "log", millis());
    });
  this->server_->addHandler(this);
  this->server_->addHandler(&this->events_);

  this->server_->begin();

  this->set_interval(10000, [this](){
    this->events_.send("", "ping", millis(), 30000);
  });
}

WebServer::WebServer(uint16_t port)
    : port_(port) {

}
float WebServer::get_setup_priority() const {
  return setup_priority::MQTT_CLIENT;
}
#ifdef USE_SENSOR
void WebServer::register_sensor(sensor::Sensor *obj) {
  StoringController::register_sensor(obj);
  obj->add_on_value_callback([this, obj](float value) {
    this->defer([this, obj, value] {
      this->events_.send(this->sensor_json(obj, value).c_str(), "state");
    });
  });
}
void WebServer::handle_sensor_request(AsyncWebServerRequest *request, UrlMatch match) {
  for (sensor::Sensor *obj : this->sensors_) {
    if (obj->get_name_id() == match.id) {
      std::string data = this->sensor_json(obj, obj->get_value());
      request->send(200, "text/json", data.c_str());
      return;
    }
  }
  request->send(404);
}
std::string WebServer::sensor_json(sensor::Sensor *obj, float value) {
  return build_json([obj, value](JsonBuffer &buffer, JsonObject &root) {
    root["id"] = "sensor-" + obj->get_name_id();
    std::string state = value_accuracy_to_string(value, obj->get_accuracy_decimals());
    if (!obj->get_unit_of_measurement().empty())
      state += " " + obj->get_unit_of_measurement();
    root["state"] = state;
    root["value"] = value;
  });
}
#endif

#ifdef USE_SWITCH
void WebServer::register_switch(switch_::Switch *obj) {
  StoringController::register_switch(obj);
  obj->add_on_state_callback([this, obj](bool value) {
    this->defer([this, obj, value] {
      this->events_.send(this->switch_json(obj, value).c_str(), "state");
    });
  });
}
std::string WebServer::switch_json(switch_::Switch *obj, bool value) {
  return build_json([obj, value](JsonBuffer &buffer, JsonObject &root) {
    root["id"] = "switch-" + obj->get_name_id();
    root["state"] = value ? "ON" : "OFF";
    root["value"] = value;
  });
}
void WebServer::handle_switch_request(AsyncWebServerRequest *request, UrlMatch match) {
  for (switch_::Switch *obj : this->switches_) {
    if (obj->get_name_id() != match.id)
      continue;

    if (request->method() == HTTP_GET) {
      std::string data = this->switch_json(obj, obj->get_value());
      request->send(200, "text/json", data.c_str());
    } else if (match.method == "toggle") {
      if (obj->get_value())
        obj->turn_off();
      else
        obj->turn_on();
      request->send(200);
    } else if (match.method == "turn_on") {
      obj->turn_on();
      request->send(200);
    } else if (match.method == "turn_off") {
      obj->turn_off();
      request->send(200);
    } else {
      request->send(404);
    }
    return;
  }
  request->send(404);
}
#endif

#ifdef USE_BINARY_SENSOR
void WebServer::register_binary_sensor(binary_sensor::BinarySensor *obj) {
  StoringController::register_binary_sensor(obj);
  obj->add_on_state_callback([this, obj](bool value) {
    this->defer([this, obj, value] {
      this->events_.send(this->binary_sensor_json(obj, value).c_str(), "state");
    });
  });
}
std::string WebServer::binary_sensor_json(binary_sensor::BinarySensor *obj, bool value) {
  return build_json([obj, value](JsonBuffer &buffer, JsonObject &root) {
    root["id"] = "binary_sensor-" + obj->get_name_id();
    root["state"] = value ? "ON" : "OFF";
    root["value"] = value;
  });
}
void WebServer::handle_binary_sensor_request(AsyncWebServerRequest *request, UrlMatch match) {
  for (binary_sensor::BinarySensor *obj : this->binary_sensors_) {
    if (obj->get_name_id() == match.id) {
      std::string data = this->binary_sensor_json(obj, obj->get_value());
      request->send(200, "text/json", data.c_str());
      return;
    }
  }
  request->send(404);
}
#endif

#ifdef USE_FAN
void WebServer::register_fan(fan::FanState *obj) {
  StoringController::register_fan(obj);
  obj->add_on_receive_backend_state_callback([this, obj]() {
    this->defer([this, obj] {
      this->events_.send(this->fan_json(obj).c_str(), "state");
    });
  });
}
std::string WebServer::fan_json(fan::FanState *obj) {
  return build_json([obj](JsonBuffer &buffer, JsonObject &root) {
    root["id"] = "fan-" + obj->get_name_id();
    root["state"] = obj->get_state() ? "ON" : "OFF";
    root["value"] = obj->get_state();
    if (obj->get_traits().supports_speed()) {
      switch (obj->get_speed()) {
        case fan::FanState::SPEED_OFF:root["speed"] = "off";
          break;
        case fan::FanState::SPEED_LOW:root["speed"] = "low";
          break;
        case fan::FanState::SPEED_MEDIUM:root["speed"] = "medium";
          break;
        case fan::FanState::SPEED_HIGH:root["speed"] = "high";
          break;
      }
    }
    if (obj->get_traits().supports_oscillation())
      root["oscillation"] = obj->is_oscillating();
  });
}
void WebServer::handle_fan_request(AsyncWebServerRequest *request, UrlMatch match) {
  for (fan::FanState *obj : this->fans_) {
    if (obj->get_name_id() != match.id)
      continue;

    if (request->method() == HTTP_GET) {
      std::string data = this->fan_json(obj);
      request->send(200, "text/json", data.c_str());
    } else if (match.method == "toggle") {
      obj->set_state(!obj->get_state());
      request->send(200);
    } else if (match.method == "turn_on") {
      obj->set_state(true);
      if (request->hasParam("speed")) {
        String speed = request->getParam("speed")->value();
        if (!obj->set_speed(speed.c_str())) {
          request->send(404);
          return;
        }
      }
      if (request->hasParam("oscillation")) {
        String speed = request->getParam("oscillation")->value();
        auto val = parse_on_off(speed.c_str());
        if (!val.defined) {
          request->send(404);
          return;
        }
        obj->set_oscillating(val.value);
      }
      request->send(200);
    } else if (match.method == "turn_off") {
      obj->set_state(false);
      request->send(200);
    } else {
      request->send(404);
    }
    return;
  }
  request->send(404);
}
#endif

bool WebServer::canHandle(AsyncWebServerRequest *request) {
  if (request->url() == "/")
    return true;

  UrlMatch match = match_url(request->url().c_str(), true);
  if (!match.valid)
    return false;
#ifdef USE_SENSOR
  if (request->method() == HTTP_GET && match.domain == "sensor")
    return true;
#endif

#ifdef USE_SWITCH
  if ((request->method() == HTTP_POST || request->method() == HTTP_GET) &&
      match.domain == "switch")
    return true;
#endif

#ifdef USE_BINARY_SENSOR
  if (request->method() == HTTP_GET && match.domain == "binary_sensor")
    return true;
#endif

#ifdef USE_FAN
  if ((request->method() == HTTP_POST || request->method() == HTTP_GET) &&
      match.domain == "fan")
    return true;
#endif

#ifdef USE_LIGHT
  if ((request->method() == HTTP_POST || request->method() == HTTP_GET) &&
      match.domain == "light")
    return true;
#endif

  return false;
}
void WebServer::handleRequest(AsyncWebServerRequest *request) {
  if (request->url() == "/") {
    this->handle_index_request(request);
    return;
  }

  UrlMatch match = match_url(request->url().c_str());
#ifdef USE_SENSOR
  if (match.domain == "sensor") {
    this->handle_sensor_request(request, match);
    return;
  }
#endif

#ifdef USE_SWITCH
  if (match.domain == "switch") {
    this->handle_switch_request(request, match);
    return;
  }
#endif

#ifdef USE_BINARY_SENSOR
  if (match.domain == "binary_sensor") {
    this->handle_binary_sensor_request(request, match);
    return;
  }
#endif

#ifdef USE_FAN
  if (match.domain == "fan") {
    this->handle_fan_request(request, match);
    return;
  }
#endif

#ifdef USE_LIGHT
  if (match.domain == "light") {
    this->handle_light_request(request, match);
    return;
  }
#endif
}
bool WebServer::isRequestHandlerTrivial() {
  return false;
}
#ifdef USE_LIGHT
void WebServer::register_light(light::LightState *obj) {
  StoringController::register_light(obj);
  obj->add_send_callback([this, obj]() {
    this->defer([this, obj] {
      this->events_.send(this->light_json(obj).c_str(), "state");
    });
  });
}
void WebServer::handle_light_request(AsyncWebServerRequest *request, UrlMatch match) {
  for (light::LightState *obj : this->lights_) {
    if (obj->get_name_id() != match.id)
      continue;

    if (request->method() == HTTP_GET) {
      std::string data = this->light_json(obj);
      request->send(200, "text/json", data.c_str());
    } else if (match.method == "toggle") {
      auto v = obj->get_remote_values();
      if (v.get_state() > 0.0f)
        v.set_state(0.0f);
      else
        v.set_state(1.0f);
      obj->start_default_transition(v);
      request->send(200);
    } else if (match.method == "turn_on") {
      auto v = obj->get_remote_values();
      v.set_state(1.0f);
      if (obj->get_traits().supports_brightness() && request->hasParam("brightness"))
        v.set_brightness(request->getParam("brightness")->value().toFloat() / 255.0f);
      if (obj->get_traits().supports_rgb()) {
        if (request->hasParam("r"))
          v.set_red(request->getParam("r")->value().toFloat() / 255.0f);
        if (request->hasParam("g"))
          v.set_green(request->getParam("g")->value().toFloat() / 255.0f);
        if (request->hasParam("b"))
          v.set_blue(request->getParam("b")->value().toFloat() / 255.0f);
      }
      if (obj->get_traits().has_rgb_white_value() && request->hasParam("white_value"))
        v.set_white(request->getParam("white_value")->value().toFloat() / 255.0f);

      v.normalize_color(obj->get_traits());

      if (request->hasParam("flash")) {
        uint32_t length = request->getParam("flash")->value().toFloat() * 1000;
        obj->start_flash(v, length);
      } else if (request->hasParam("transition")) {
        uint32_t length = request->getParam("transition")->value().toFloat() * 1000;
        obj->start_transition(v, length);
      } else if (request->hasParam("effect")) {
        const char *effect = request->getParam("effect")->value().c_str();
        obj->start_effect(effect);
      } else {
        obj->start_default_transition(v);
      }
      request->send(200);
    } else if (match.method == "turn_off") {
      auto v = obj->get_remote_values();
      v.set_state(0.0f);
      if (request->hasParam("transition")) {
        uint32_t length = request->getParam("transition")->value().toFloat() * 1000;
        obj->start_transition(v, length);
      } else {
        obj->start_default_transition(v);
      }
      request->send(200);
    } else {
      request->send(404);
    }
    return;
  }
  request->send(404);
}
std::string WebServer::light_json(light::LightState *obj) {
  return build_json([obj](JsonBuffer &buffer, JsonObject &root) {
    root["id"] = "light-" + obj->get_name_id();
    root["state"] = obj->get_remote_values().get_state() == 1.0 ? "ON" : "OFF";
    obj->dump_json(buffer, root);
  });
}
#endif

void WebServer::handle_index_request(AsyncWebServerRequest *request) {
  AsyncResponseStream *stream = request->beginResponseStream("text/html");
  std::string title = App.get_name() + " Web Server";
  stream->print(F("<!DOCTYPE html><html><head><meta charset=UTF-8><title>"));
  stream->print(title.c_str());
  stream->print(F("</title><link rel=\"stylesheet\" href=\"https://esphomelib.com/_static/webserver-v1.min.css\">"
                  "</head><body><article class=\"markdown-body\"><h1>"));
  stream->print(title.c_str());
  stream->print(F("</h1><h2>States</h2><table id=\"states\"><thead><tr><th>Name<th>State<th>Actions<tbody>"));

#ifdef USE_SENSOR
  for (auto *obj : this->sensors_)
    write_row(stream, obj, "sensor", "");
#endif

#ifdef USE_SWITCH
  for (auto *obj : this->switches_)
    write_row(stream, obj, "switch", "<button>Toggle</button>");
#endif

#ifdef USE_BINARY_SENSOR
  for (auto *obj : this->binary_sensors_)
    write_row(stream, obj, "binary_sensor", "");
#endif

#ifdef USE_FAN
  for (auto *obj : this->fans_)
    write_row(stream, obj, "fan", "<button>Toggle</button>");
#endif

#ifdef USE_LIGHT
  for (auto *obj : this->lights_)
    write_row(stream, obj, "light", "<button>Toggle</button>");
#endif

  stream->print(
      F("</tbody></table><p>See <a href=https://esphomelib.com/web-api/index.html>esphomelib Web API</a> for REST API documentation.</p>"
        "<h2>Debug Log</h2><pre id=\"log\"></pre>"
        "<script src=\"https://esphomelib.com/_static/webserver-v1.min.js\"></script>"
        "</article></body></html>")
  );

  request->send(stream);
}
uint16_t WebServer::get_port() const {
  return this->port_;
}
void WebServer::set_port(uint16_t port) {
  this->port_ = port;
}

} // namespace esphomelib

#endif
