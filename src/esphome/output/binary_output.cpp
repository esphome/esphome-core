#include "esphome/defines.h"

#ifdef USE_OUTPUT

#include "esphome/output/binary_output.h"

ESPHOME_NAMESPACE_BEGIN

namespace output {

bool BinaryOutput::is_inverted() const { return this->inverted_; }
void BinaryOutput::set_inverted(bool inverted) { this->inverted_ = inverted; }
PowerSupplyComponent *BinaryOutput::get_power_supply() const { return this->power_supply_; }
void BinaryOutput::set_power_supply(PowerSupplyComponent *power_supply) { this->power_supply_ = power_supply; }
void BinaryOutput::turn_on() {
  if (this->power_supply_ != nullptr && !this->has_requested_high_power_) {
    this->power_supply_->request_high_power();
    this->has_requested_high_power_ = true;
  }
  this->write_state(!this->inverted_);
}
void BinaryOutput::turn_off() {
  if (this->power_supply_ != nullptr && this->has_requested_high_power_) {
    this->power_supply_->unrequest_high_power();
    this->has_requested_high_power_ = false;
  }
  this->write_state(this->inverted_);
}

}  // namespace output

ESPHOME_NAMESPACE_END

#endif  // USE_OUTPUT
