//
// Created by Otto Winter on 26.11.17.
//

#include "esphomelib/output/float_output.h"

#include "esphomelib/helpers.h"
#include "esphomelib/log.h"

#ifdef USE_OUTPUT

ESPHOMELIB_NAMESPACE_BEGIN

namespace output {

void FloatOutput::set_max_power(float max_power) {
  this->max_power_ = clamp(0.0f, 1.0f, max_power);
}

float FloatOutput::get_max_power() const {
  return this->max_power_;
}

void FloatOutput::set_state_(float state) {
  state = clamp(0.0f, 1.0f, state);

  if (state > 0.0f) { // ON
    // maybe refactor this
    if (this->power_supply_ != nullptr && !this->has_requested_high_power_) {
      this->power_supply_->request_high_power();
      this->has_requested_high_power_ = true;
    }
  } else { // OFF
    if (this->power_supply_ != nullptr && this->has_requested_high_power_) {
      this->power_supply_->unrequest_high_power();
      this->has_requested_high_power_ = false;
    }
  }

  float adjusted_value =  state * this->max_power_;
  if (this->is_inverted())
    adjusted_value = 1 - adjusted_value;
  this->write_state(adjusted_value);
}

void FloatOutput::enable() {
  this->set_state_(1.0f);
}
void FloatOutput::disable() {
  this->set_state_(0.0f);
}
void FloatOutput::write_enabled(bool value) {
  // This should never be called, as we override enable & disable
  assert(false);
}

} // namespace output

ESPHOMELIB_NAMESPACE_END

#endif //USE_OUTPUT
