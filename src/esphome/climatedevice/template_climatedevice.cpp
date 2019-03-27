#include "esphome/defines.h"

#ifdef USE_TEMPLATE_CLIMATEDEVICE

#include "esphome/climatedevice/template_climatedevice.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace climatedevice {

static const char *TAG = "climatedevice.template";

TemplateClimateDevice::TemplateClimateDevice(const std::string &name, uint32_t update_interval)
    : ClimateDevice(name, update_interval),
      control_trigger_(new Trigger<ClimateDeviceState>()),
      errro_value_trigger_(new Trigger<float>()) {
  ClimateDeviceTraits traits = this->get_traits();
  traits.set_current_temperature(true);
  this->set_traits(traits);
}
void TemplateClimateDevice::setup() {
  this->add_on_state_callback([this](ClimateDeviceState state) {
    this->control_trigger_->trigger(state);
    if (this->optimistic_) {
      this->make_call().set_state(state).publish();
    }
  });
  this->add_on_current_temperature_callback([this](float error) { this->errro_value_trigger_->trigger(error); });
}
void TemplateClimateDevice::update() {
  auto call = this->make_call();
  if (this->mode_f_.has_value()) {
    auto mode = (*this->mode_f_)();
    if (!mode.has_value()) {
      call.set_mode(*mode);
    }
  }
  if (this->target_temperature_f_.has_value()) {
    auto target_temperature = (*this->target_temperature_f_)();
    if (!target_temperature.has_value()) {
      call.set_target_temperature(*target_temperature);
    }
  }
  call.publish();
}
void TemplateClimateDevice::set_current_temperature_support(bool current_temperature_support) {
  ClimateDeviceTraits traits = this->get_traits();
  traits.set_current_temperature(current_temperature_support);
  this->set_traits(traits);
}
void TemplateClimateDevice::set_optimistic(bool optimistic) { this->optimistic_ = optimistic; }
void TemplateClimateDevice::set_mode_lambda(std::function<optional<ClimateDeviceMode>()> &&f) { this->mode_f_ = f; }
void TemplateClimateDevice::set_target_temperature_lambda(std::function<optional<float>()> &&f) {
  this->target_temperature_f_ = f;
}
float TemplateClimateDevice::get_setup_priority() const { return setup_priority::HARDWARE; }
Trigger<ClimateDeviceState> *TemplateClimateDevice::get_control_trigger() const { return this->control_trigger_; }
Trigger<float> *TemplateClimateDevice::get_error_value_trigger() const { return this->errro_value_trigger_; }
void TemplateClimateDevice::dump_config() {
  this->ClimateDevice::dump_config();
  ESP_LOGCONFIG(TAG, "  Optimistic mode: %s", ONOFF(this->optimistic_));
  ESP_LOGCONFIG(TAG, "  Current temperature support: %s", ONOFF(this->get_traits().supports_current_temperature()));
}

}  // namespace climatedevice

ESPHOME_NAMESPACE_END

#endif  // USE_TEMPLATE_CLIMATEDEVICE
