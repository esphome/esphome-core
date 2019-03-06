#include "esphome/util.h"
#include "esphome/defines.h"
#include "esphome/wifi_component.h"
#include "esphome/ethernet_component.h"
#include "esphome/api/api_server.h"
#include "esphome/application.h"

#ifdef ARDUINO_ARCH_ESP32
#include <ESPmDNS.h>
#endif
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266mDNS.h>
#endif

ESPHOME_NAMESPACE_BEGIN

bool network_is_connected() {
#ifdef USE_ETHERNET
  if (global_eth_component != nullptr && global_eth_component->is_connected())
    return true;
#endif

  if (global_wifi_component != nullptr)
    return global_wifi_component->is_connected();

  return false;
}

void network_setup() {
  bool ready = true;
#ifdef USE_ETHERNET
  if (global_eth_component != nullptr) {
    global_eth_component->call_setup();
    ready = false;
  }
#endif

  if (global_wifi_component != nullptr) {
    global_wifi_component->call_setup();
    ready = false;
  }

  while (!ready) {
#ifdef USE_ETHERNET
    if (global_eth_component != nullptr) {
      global_eth_component->call_loop();
      ready = ready || global_eth_component->can_proceed();
    }
#endif
    if (global_wifi_component != nullptr) {
      global_wifi_component->call_loop();
      ready = ready || global_wifi_component->can_proceed();
    }
    tick_status_led();
    yield();
  }
}
void network_tick() {
#ifdef USE_ETHERNET
  if (global_eth_component != nullptr)
    global_eth_component->call_loop();
#endif
  if (global_wifi_component != nullptr)
    global_wifi_component->call_loop();
}

void network_setup_mdns() {
  MDNS.begin(get_app_name().c_str());
#ifdef USE_API
  if (api::global_api_server != nullptr) {
    MDNS.addService("esphomelib", "tcp", api::global_api_server->get_port());
    // DNS-SD (!=mDNS !) requires at least one TXT record for service discovery - let's add version
    MDNS.addServiceTxt("esphomelib", "tcp", "version", ESPHOME_VERSION);
    MDNS.addServiceTxt("esphomelib", "tcp", "address", network_get_address().c_str());
  } else {
#endif
    // Publish "http" service if not using native API.
    // This is just to have *some* mDNS service so that .local resolution works
    MDNS.addService("http", "tcp", 80);
    MDNS.addServiceTxt("http", "tcp", "version", ESPHOME_VERSION);
#ifdef USE_API
  }
#endif
}
void network_tick_mdns() {
#ifdef ARDUINO_ARCH_ESP8266
  MDNS.update();
#endif
}

std::string network_get_address() {
#ifdef USE_ETHERNET
  if (global_eth_component != nullptr)
    return global_eth_component->get_use_address();
#endif
  if (global_wifi_component != nullptr)
    return global_wifi_component->get_use_address();
  return "";
}

std::string get_app_name() { return App.get_name(); }

std::string get_app_compilation_time() { return App.get_compilation_time(); }

ESPHOME_NAMESPACE_END
