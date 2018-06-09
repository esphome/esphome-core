//
//  sntp_component.cpp
//  esphomelib
//
//  Created by Brad Davidson on 06.06.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/log.h"
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"
#include "esphomelib/component.h"
#include "esphomelib/time/sntp_component.h"

#ifdef ARDUINO_ARCH_ESP32
  #include "apps/sntp/sntp.h"
#endif
#ifdef ARDUINO_ARCH_ESP8266
  #include "sntp.h"
#endif

#ifdef USE_SNTP_COMPONENT

ESPHOMELIB_NAMESPACE_BEGIN

namespace time {

static const char *TAG = "sntp";

SNTPComponent::SNTPComponent(const std::string &server_1,
		             const std::string &server_2,
			     const std::string &server_3,
			     const std::string &tz)
    : RTCComponent(tz){
  this->server_1_ = server_1;
  this->server_2_ = server_2;
  this->server_3_ = server_3;
}
void SNTPComponent::setup(){
  ESP_LOGCONFIG(TAG, "Setting up SNTP...");
  this->setup_sntp_();
}
void SNTPComponent::set_servers(const std::string &server_1,
		                const std::string &server_2,
				const std::string &server_3){
  this->server_1_ = server_1;
  this->server_2_ = server_2;
  this->server_3_ = server_3;
  this->setup_sntp_();
}
void SNTPComponent::setup_sntp_(){
  #ifdef ARDUINO_ARCH_ESP32
  if(sntp_enabled()){
    sntp_stop();
  }
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  #endif
  #ifdef ARDUINO_ARCH_ESP8266
  sntp_stop();
  #endif

  if (!this->server_1_.empty())
    sntp_setservername(0, strdup(this->server_1_.c_str()));
  if (!this->server_2_.empty())
    sntp_setservername(1, strdup(this->server_2_.c_str()));
  if (!this->server_3_.empty())
    sntp_setservername(2, strdup(this->server_3_.c_str()));

  sntp_init();
}
float SNTPComponent::get_setup_priority() const {
  return setup_priority::LATE; // display debug info via MQTT
}

} // namespace time

ESPHOMELIB_NAMESPACE_END

#endif //USE_SNTP_COMPONENT
