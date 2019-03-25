#include "esphome/defines.h"

#ifdef USE_THERMOSTAT_CLIMATEDEVICE

#include "esphome/climatedevice/thermostat_climatedevice.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace climatedevice {

static const char *TAG = "climatedevice.thermostat";

ThermostatClimateDevice::ThermostatClimateDevice(const std::string &name, output::BinaryOutput *output,
                                                 uint32_t update_interval)
    : ClimateDevice(name, update_interval), output_(output), last_output_cycle_time_(millis()) {}
void ThermostatClimateDevice::setup() {
  ClimateDeviceTraits traits = this->get_traits();
  traits.set_current_temperature(true);
  traits.set_auto_mode(true);
  if (this->cooling_) {
    traits.set_cool_mode(true);
  } else {
    traits.set_heat_mode(true);
  }
  this->set_traits(traits);
  this->add_on_state_callback([this](ClimateDeviceState state) {
    this->state.target_temperature = state.target_temperature;
    if (state.mode == CLIMATEDEVICE_MODE_OFF) {
      this->state.mode = state.mode;
    }
    // trigger control_()
    this->set_current_temperature(this->current_temperature);
  });
  this->add_on_current_temperature_callback([this](float error) { this->control_(error); });
}
void ThermostatClimateDevice::update() { this->make_call().set_state(this->state).publish(); }
void ThermostatClimateDevice::set_cooling(bool cooling) { this->cooling_ = cooling; }
void ThermostatClimateDevice::set_hysteresis(float cold, float hot) {
  this->hysteresis_cold_ = abs(cold);
  this->hysteresis_hot_ = abs(hot);
}
void ThermostatClimateDevice::set_hysteresis(float hysteresis) { this->set_hysteresis(hysteresis, hysteresis); }
void ThermostatClimateDevice::set_min_cycle_duration(uint32_t min_cycle_duration) {
  this->min_cycle_duration_ = min_cycle_duration;
}
float ThermostatClimateDevice::get_setup_priority() const { return setup_priority::HARDWARE; }
void ThermostatClimateDevice::dump_config() {
  this->ClimateDevice::dump_config();
  ESP_LOGCONFIG(TAG, "  Mode: %s", this->cooling_ ? "cooling" : "heating");
  ESP_LOGCONFIG(TAG, "  Hysteresis hot: %.1f", this->hysteresis_hot_);
  ESP_LOGCONFIG(TAG, "  Hysteresis cold: %.1f°C", this->hysteresis_cold_);
  ESP_LOGCONFIG(TAG, "  Minimal cycle duration: %u ms", this->min_cycle_duration_);
}
void ThermostatClimateDevice::control_(float error_value) {
  uint32_t now = millis();
  if (isnan(error_value)) {
    // Thermostat is turned off
    this->output_->turn_off();
    this->output_state_ = false;
    last_output_cycle_time_ = now;
    return;
  }
  if ((now - last_output_cycle_time_) < this->min_cycle_duration_) {
    return;
  }
  if (this->cooling_) {
    error_value = -1 * error_value;
  }
  bool state = this->output_state_;
  bool too_hot = error_value > this->hysteresis_hot_;
  bool too_cold = error_value < (-1 * this->hysteresis_cold_);
  if ((!this->cooling_ && too_hot) || (this->cooling_ && too_cold)) {
    state = false;
  }
  if ((!this->cooling_ && too_cold) || (this->cooling_ && too_hot)) {
    state = true;
  }
  if (state != this->output_state_) {
    last_output_cycle_time_ = now;
    this->output_state_ = state;
    auto call = this->make_call();
    if (state) {
      this->output_->turn_on();
      if (this->cooling_) {
        call.set_mode(CLIMATEDEVICE_MODE_COOL);
      } else {
        call.set_mode(CLIMATEDEVICE_MODE_HEAT);
      }
    } else {
      this->output_->turn_off();
      call.set_mode(CLIMATEDEVICE_MODE_AUTO);
    }
    call.publish();
  }
}

}  // namespace climatedevice

ESPHOME_NAMESPACE_END

#endif  // USE_THERMOSTAT_CLIMATEDEVICE
