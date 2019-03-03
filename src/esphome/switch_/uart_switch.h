#ifndef ESPHOME_UART_SWITCH_H
#define ESPHOME_UART_SWITCH_H

#include "esphome/defines.h"

#ifdef USE_UART_SWITCH

#include "esphome/switch_/switch.h"
#include "esphome/uart_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace switch_ {

class UARTSwitch : public Switch, public UARTDevice, public Component {
 public:
  UARTSwitch(UARTComponent *parent, const std::string &name, const std::vector<uint8_t> &data);

  void dump_config() override;

 protected:
  void write_state(bool state) override;
  std::vector<uint8_t> data_;
};

}  // namespace switch_

ESPHOME_NAMESPACE_END

#endif  // USE_UART_SWITCH

#endif  // ESPHOME_UART_SWITCH_H
