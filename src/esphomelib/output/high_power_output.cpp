//
// Created by Otto Winter on 02.12.17.
//

#include "esphomelib/output/high_power_output.h"

namespace esphomelib {

namespace output {

esphomelib::PowerSupplyComponent *HighPowerOutput::get_power_supply() const {
  return this->power_supply_;
}

void HighPowerOutput::set_power_supply(esphomelib::PowerSupplyComponent *power_supply) {
  this->power_supply_ = power_supply;
}

void HighPowerOutput::enable_power_supply() {
  if (this->power_supply_ != nullptr)
    this->power_supply_->enable();
}
HighPowerOutput::HighPowerOutput() : power_supply_(nullptr) {}

} // namespace output

} // namespace esphomelib