//
// Created by Otto Winter on 02.12.17.
//

#include "high_power_output.h"

namespace esphomelib {

namespace output {

esphomelib::ATXComponent *HighPowerOutput::get_atx() const {
  return this->atx_;
}

void HighPowerOutput::set_atx(esphomelib::ATXComponent *atx) {
  this->atx_ = atx;
}

void HighPowerOutput::enable_atx() {
  if (this->atx_ != nullptr)
    this->atx_->enable();
}
HighPowerOutput::HighPowerOutput() : atx_(nullptr) {}

} // namespace output

} // namespace esphomelib