#ifndef ESPHOMELIB_UTIL_H
#define ESPHOMELIB_UTIL_H

#include "esphomelib/defines.h"
#include <string>
#include <IPAddress.h>

ESPHOMELIB_NAMESPACE_BEGIN

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

ESPHOMELIB_NAMESPACE_END

#endif //ESPHOMELIB_UTIL_H
