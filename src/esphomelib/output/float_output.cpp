//
// Created by Otto Winter on 26.11.17.
//

#include "esphomelib/output/float_output.h"

#include "esphomelib/helpers.h"
#include "esphomelib/log.h"

namespace esphomelib {

namespace output {

static const char *TAG = "output::float_output";

void FloatOutput::set_value_f(float value) {
  float adjusted_value = clamp(0.0f, 1.0f, value) * this->max_power_;
  this->write_value_f(adjusted_value);
}

void FloatOutput::set_max_power(float max_power) {
  this->max_power_ = clamp(0.0f, 1.0f, max_power);
}

void FloatOutput::write_value(bool value) {
    // This will never be called.
}

FloatOutput::FloatOutput() : BinaryOutput(), max_power_(1.0f) {
}

void FloatOutput::set_value(bool value) {
  this->set_value_f(value ? 1.0f : 0.0f);
}

float FloatOutput::get_max_power() const {
  return this->max_power_;
}

} // namespace output

} // namespace esphomelib