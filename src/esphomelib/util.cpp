#include "esphomelib/util.h"
#include "esphomelib/defines.h"
#include "esphomelib/wifi_component.h"
#include "esphomelib/ethernet_component.h"
#include "esphomelib/api/api_server.h"

#ifdef ARDUINO_ARCH_ESP32
  #include <ESPmDNS.h>
#endif
#ifdef ARDUINO_ARCH_ESP8266
  #include <ESP8266mDNS.h>
#endif

ESPHOMELIB_NAMESPACE_BEGIN

bool network_is_connected() {
#ifdef USE_ETHERNET
  if (global_eth_component != nullptr && global_eth_component->is_connected())
    return true;
#endif

  if (global_wifi_component != nullptr)
    return global_wifi_component->is_connected();

  return false;
}

std::string network_get_hostname() {
#ifdef USE_ETHERNET
  if (global_eth_component != nullptr)
    return global_eth_component->get_hostname();
#endif

  if (global_wifi_component != nullptr)
    return global_wifi_component->get_hostname();

  return "";
}

void network_setup() {
  bool ready = true;
#ifdef USE_ETHERNET
  if (global_eth_component != nullptr) {
    global_eth_component->setup_();
    ready = false;
  }
#endif

  if (global_wifi_component != nullptr) {
    global_wifi_component->setup_();
    ready = false;
  }

  while (!ready) {
#ifdef USE_ETHERNET
    if (global_eth_component != nullptr) {
      global_eth_component->loop_();
      ready = ready || global_eth_component->can_proceed();
    }
#endif
    if (global_wifi_component != nullptr) {
      global_wifi_component->loop_();
      ready = ready || global_wifi_component->can_proceed();
    }
    tick_status_led();
    yield();
  }
}
void network_tick() {
#ifdef USE_ETHERNET
  if (global_eth_component != nullptr)
    global_eth_component->loop_();
#endif
  if (global_wifi_component != nullptr)
    global_wifi_component->loop_();
}

void network_setup_mdns(const std::string &hostname) {
  MDNS.begin(hostname.c_str());
#ifdef USE_API
  if (api::global_api_server != nullptr) {
    MDNS.addService("esphomelib", "tcp", api::global_api_server->get_port());
    // DNS-SD (!=mDNS !) requires at least one TXT record for service discovery - let's add version
    MDNS.addServiceTxt("esphomelib", "tcp", "version", ESPHOMELIB_VERSION);
  } else {
#endif
    // Publish "http" service if not using native API.
    // This is just to have *some* mDNS service so that .local resolution works
    MDNS.addService("http", "tcp", 80);
    MDNS.addServiceTxt("http", "tcp", "version", ESPHOMELIB_VERSION);
#ifdef USE_API
  }
#endif
}

IPAddress network_get_address() {
#ifdef USE_ETHERNET
  if (global_eth_component != nullptr)
    global_eth_component->get_ip_address();
#endif
  if (global_wifi_component != nullptr)
    global_wifi_component->get_ip_address();
  return IPAddress();
}


ESPHOMELIB_NAMESPACE_END
