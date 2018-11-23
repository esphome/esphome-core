#ifndef ESPHOMELIB_UART_SWITCH_H
#define ESPHOMELIB_UART_SWITCH_H

#include "esphomelib/defines.h"

#ifdef USE_UART_SWITCH

#include "esphomelib/switch_/switch.h"
#include "esphomelib/uart_component.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace switch_ {

class UARTSwitch : public Switch, public UARTDevice {
 public:
  UARTSwitch(UARTComponent *parent, const std::string &name, const std::vector<uint8_t> &data);

 protected:
  void write_state(bool state) override;
  std::vector<uint8_t> data_;
};

} // namespace switch_

ESPHOMELIB_NAMESPACE_END

#endif //USE_UART_SWITCH

#endif //ESPHOMELIB_UART_SWITCH_H
