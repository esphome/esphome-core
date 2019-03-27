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

  template<typename V> void set_mode(V value) { this->mode_ = value; }
  template<typename V> void set_target_temperature(V value) { this->target_temperature_ = value; }
  template<typename V> void set_target_temperature_low(V value) { this->target_temperature_low_ = value; }
  template<typename V> void set_target_temperature_high(V value) { this->target_temperature_high_ = value; }
  template<typename V> void set_away(V value) { this->away_ = value; }

  void play(Ts... x) override {
    auto call = this->climate_->make_call();
    if (this->mode_.has_value()) {
      call.set_target_temperature(this->mode_.value(x...));
    }
    if (this->target_temperature_.has_value()) {
      call.set_target_temperature(this->target_temperature_.value(x...));
    }
    if (this->target_temperature_low_.has_value()) {
      call.set_target_temperature_low(this->target_temperature_low_.value(x...));
    }
    if (this->target_temperature_high_.has_value()) {
      call.set_target_temperature_high(this->target_temperature_high_.value(x...));
    }
    if (this->away_.has_value()) {
      call.set_away(this->away_.value(x...));
    }
    call.perform();
  }

 protected:
  ClimateDevice *climate_;
  TemplatableValue<ClimateMode, Ts...> mode_;
  TemplatableValue<float, Ts...> target_temperature_;
  TemplatableValue<float, Ts...> target_temperature_low_;
  TemplatableValue<float, Ts...> target_temperature_high_;
  TemplatableValue<bool, Ts...> away_;
};

}  // namespace climate

ESPHOME_NAMESPACE_END

#endif  // USE_CLIMATE

#endif  // ESPHOME_CORE_CLIMATE_AUTOMATION_H
