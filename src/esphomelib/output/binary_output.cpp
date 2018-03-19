//
// Created by Otto Winter on 02.12.17.
//

#include "esphomelib/output/binary_output.h"

namespace esphomelib {

namespace output {

void BinaryOutput::set_value(bool value) {
  this->write_value(value != this->inverted_);
}

bool BinaryOutput::is_inverted() const {
  return this->inverted_;
}

void BinaryOutput::set_inverted(bool inverted) {
  this->inverted_ = inverted;
}
BinaryOutput::BinaryOutput() : inverted_(false) {}

} // namespace output

} // namespace esphomelib