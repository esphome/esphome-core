#include "esphome/defines.h"

#ifdef USE_UART_SWITCH

#include "esphome/switch_/uart_switch.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace switch_ {

static const char *TAG = "switch.uart";

UARTSwitch::UARTSwitch(UARTComponent *parent, const std::string &name, const std::vector<uint8_t> &data)
    : Switch(name), UARTDevice(parent), data_(data) {}

void UARTSwitch::write_state(bool state) {
  if (!state) {
    this->publish_state(false);
    return;
  }

  this->publish_state(true);
  ESP_LOGD(TAG, "'%s': Sending data...", this->get_name().c_str());
  this->write_array(this->data_.data(), this->data_.size());
  this->publish_state(false);
}
void UARTSwitch::dump_config() { LOG_SWITCH("", "UART Switch", this); }

}  // namespace switch_

ESPHOME_NAMESPACE_END

#endif  // USE_UART_SWITCH
