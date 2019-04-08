#ifndef ESPHOME_CORE_CLIMATE_AUTOMATION_H
#define ESPHOME_CORE_CLIMATE_AUTOMATION_H

#include "esphome/defines.h"

#ifdef USE_CLIMATE

#include "esphome/climate/climate_mode.h"
#include "esphome/climate/climate_device.h"
#include "esphome/automation.h"

ESPHOME_NAMESPACE_BEGIN

namespace climate {

class ClimateDevice;

template<typename... Ts> class ControlAction : public Action<Ts...> {
 public:
  explicit ControlAction(ClimateDevice *climate) : climate_(climate) {}

  TEMPLATABLE_VALUE(ClimateMode, mode)
  TEMPLATABLE_VALUE(float, target_temperature)
  TEMPLATABLE_VALUE(float, target_temperature_low)
  TEMPLATABLE_VALUE(float, target_temperature_high)
  TEMPLATABLE_VALUE(bool, away)

  void play(Ts... x) override {
    auto call = this->climate_->make_call();
    call.set_target_temperature(this->mode_.optional_value(x...));
    call.set_target_temperature_low(this->target_temperature_low_.optional_value(x...));
    call.set_target_temperature_high(this->target_temperature_high_.optional_value(x...));
    call.set_away(this->away_.optional_value(x...));
    call.perform();
  }

 protected:
  ClimateDevice *climate_;
};

}  // namespace climate

ESPHOME_NAMESPACE_END

#endif  // USE_CLIMATE

#endif  // ESPHOME_CORE_CLIMATE_AUTOMATION_H
