#ifndef ESPHOMELIB_UTIL_H
#define ESPHOMELIB_UTIL_H

#include "esphomelib/defines.h"
#include <string>

ESPHOMELIB_NAMESPACE_BEGIN

/// Return whether the node is connected to the network (through wifi, eth, ...)
bool network_is_connected();
/// Get the active network hostname
std::string network_get_hostname();

/// Manually set up the network stack (outside of the App.setup() loop, for example in OTA safe mode)
void network_setup();
void network_tick();
void network_setup_mdns(const std::string &hostname);

ESPHOMELIB_NAMESPACE_END

#endif //ESPHOMELIB_UTIL_H
