#include "esphomelib/defines.h"

#ifdef USE_UART_SWITCH

#include "esphomelib/switch_/uart_switch.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.uart";

UARTSwitch::UARTSwitch(UARTComponent *parent, const std::string &name, const std::vector<uint8_t> &data)
    : Switch(name), UARTDevice(parent), data_(data) {}

void UARTSwitch::turn_on() {
  this->publish_state(true);
  ESP_LOGD(TAG, "'%s': Sending data...", this->get_name().c_str());
  this->write_array(this->data_.data(), this->data_.size());
  this->publish_state(false);
}

void UARTSwitch::turn_off() {
  // Do nothing
}

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_UART_SWITCH
