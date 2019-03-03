#ifndef ESPHOME_UTIL_H
#define ESPHOME_UTIL_H

#include "esphome/defines.h"
#include <string>
#include <IPAddress.h>

ESPHOME_NAMESPACE_BEGIN

/// Return whether the node is connected to the network (through wifi, eth, ...)
bool network_is_connected();
/// Get the active network hostname
std::string network_get_address();

/// Manually set up the network stack (outside of the App.setup() loop, for example in OTA safe mode)
void network_setup();
void network_tick();
void network_setup_mdns();
void network_tick_mdns();

std::string get_app_name();
std::string get_app_compilation_time();

ESPHOME_NAMESPACE_END

#endif  // ESPHOME_UTIL_H
