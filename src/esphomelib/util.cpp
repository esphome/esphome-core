#include "esphomelib/util.h"
#include "esphomelib/defines.h"
#include "esphomelib/wifi_component.h"
#include "esphomelib/ethernet_component.h"

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

ESPHOMELIB_NAMESPACE_END
