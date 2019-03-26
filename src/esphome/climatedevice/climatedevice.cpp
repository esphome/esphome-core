#include "esphome/defines.h"

#ifdef USE_CLIMATEDEVICE

#include "esphome/climatedevice/climatedevice.h"
#include "esphome/esppreferences.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace climatedevice {

static const char *TAG = "climatedevice.base";

const ClimateDeviceTraits &ClimateDevice::get_traits() const { return this->traits_; }
void ClimateDevice::set_traits(const ClimateDeviceTraits &traits) { this->traits_ = traits; }
void ClimateDevice::add_on_state_callback(std::function<void(ClimateDeviceState)> &&callback) {
  this->state_callback_.add(std::move(callback));
}
void ClimateDevice::add_on_publish_state_callback(std::function<void()> &&callback) {
  this->publish_state_callback_.add(std::move(callback));
}
void ClimateDevice::add_on_current_temperature_callback(std::function<void(float)> &&callback) {
  this->current_temperature_callback_.add(std::move(callback));
}
ClimateDevice::ClimateDevice(const std::string &name, uint32_t update_interval)
    : Nameable(name), PollingComponent(update_interval) {}

ClimateDevice::StateCall ClimateDevice::make_call() { return ClimateDevice::StateCall(this); }

void ClimateDevice::setup() {
  this->state.mode = CLIMATEDEVICE_MODE_OFF;
  this->state.target_temperature = this->get_target_temperature_initial();
  this->rtc_ = global_preferences.make_preference<ClimateDeviceState>(this->get_object_id_hash());
  ClimateDeviceState recovered;
  auto call = this->make_call();
  if (this->rtc_.load(&recovered)) {
    call.set_mode(recovered.mode);
    call.set_target_temperature(recovered.target_temperature);
  }
  call.perform();
}
float ClimateDevice::get_setup_priority() const { return setup_priority::HARDWARE - 1.0f; }
uint32_t ClimateDevice::hash_base() { return 418001110UL; }  // TODO
void ClimateDevice::dump_config() {
  auto traits = this->get_traits();
  ESP_LOGCONFIG(TAG, "Climate device '%s'", this->get_name().c_str());
  std::string modes_s = "off";
  if (traits.supports_auto_mode()) {
    modes_s += ", auto";
  }
  if (traits.supports_cool_mode()) {
    modes_s += ", cool";
  }
  if (traits.supports_heat_mode()) {
    modes_s += ", heat";
  }
  ESP_LOGCONFIG(TAG, "  Supported modes: %s", modes_s.c_str());
  ESP_LOGCONFIG(TAG, "  Initial target temperature: %.1f°C", this->get_target_temperature_initial());
  ESP_LOGCONFIG(TAG, "  Minimal allowed target temperature: %.1f°C", traits.get_min_target_temperature());
  ESP_LOGCONFIG(TAG, "  Maximal allowed target temperature: %.1f°C", traits.get_max_target_temperature());
  ESP_LOGCONFIG(TAG, "  Step size of target temperature: %.1f°C", this->get_target_temperature_step());
  ESP_LOGCONFIG(TAG, "  Accuracy Decimals of target temperature: %d", this->get_target_temperature_accuracy_decimals());
  ESP_LOGCONFIG(TAG, "  Accuracy Decimals of current temperature: %d",
                this->get_current_temperature_accuracy_decimals());
  LOG_UPDATE_INTERVAL(this);
}
void ClimateDevice::set_current_temperature(float current_temperature) {
  if (this->get_traits().supports_current_temperature()) {
    ESP_LOGD(TAG, "'%s' Setting current temperature: %.1f°C", this->get_name().c_str(), current_temperature);
    float error_value = this->state.target_temperature - current_temperature;
    this->current_temperature = current_temperature;
    if (this->state.mode == CLIMATEDEVICE_MODE_OFF) {
      error_value = NAN;
    }
    this->current_temperature_callback_.call(error_value);
  }
}
void ClimateDevice::set_current_temperature_accuracy_decimals(int8_t accuracy_decimals) {
  this->current_temperature_accuracy_decimals_ = accuracy_decimals;
}
int8_t ClimateDevice::get_current_temperature_accuracy_decimals() const {
  return this->current_temperature_accuracy_decimals_;
}
void ClimateDevice::set_target_temperature_accuracy_decimals(int8_t accuracy_decimals) {
  this->target_temperature_accuracy_decimals_ = accuracy_decimals;
}
int8_t ClimateDevice::get_target_temperature_accuracy_decimals() const {
  return this->target_temperature_accuracy_decimals_;
}
void ClimateDevice::set_target_temperature_initial(float initial) { this->target_temperature_initial_ = initial; }
float ClimateDevice::get_target_temperature_initial() const { return this->target_temperature_initial_; }
void ClimateDevice::set_target_temperature_step(float step) { this->target_temperature_step_ = step; }
float ClimateDevice::get_target_temperature_step() const { return this->target_temperature_step_; }
void ClimateDevice::set_target_temperature_range(float min, float max) {
  ClimateDeviceTraits traits = this->get_traits();
  traits.set_min_target_temperature(min);
  traits.set_max_target_temperature(max);
  this->set_traits(traits);
}
void ClimateDevice::set_modes(std::vector<ClimateDeviceMode> modes) {
  ClimateDeviceTraits traits = this->get_traits();
  for (auto mode : modes) {
    switch (mode) {
      case CLIMATEDEVICE_MODE_AUTO: {
        traits.set_auto_mode(true);
        break;
      }
      case CLIMATEDEVICE_MODE_COOL: {
        traits.set_cool_mode(true);
        break;
      }
      case CLIMATEDEVICE_MODE_HEAT: {
        traits.set_heat_mode(true);
        break;
      }
      default: {}
    }
  }
  this->set_traits(traits);
}
#ifdef USE_MQTT_CLIMATEDEVICE
MQTTClimateDeviceComponent *ClimateDevice::get_mqtt() const { return this->mqtt_; }
void ClimateDevice::set_mqtt(MQTTClimateDeviceComponent *mqtt) { this->mqtt_ = mqtt; }
#endif

ClimateDevice::StateCall::StateCall(ClimateDevice *device) : device_(device) {}
ClimateDevice::StateCall &ClimateDevice::StateCall::set_state(ClimateDeviceState state) {
  this->set_mode(state.mode);
  this->set_target_temperature(state.target_temperature);
  return *this;
}
ClimateDevice::StateCall &ClimateDevice::StateCall::set_mode(ClimateDeviceMode mode) {
  this->mode_ = mode;
  return *this;
}
ClimateDevice::StateCall &ClimateDevice::StateCall::set_mode(optional<ClimateDeviceMode> mode) {
  this->mode_ = mode;
  return *this;
}
ClimateDevice::StateCall &ClimateDevice::StateCall::set_target_temperature(float target_temperature) {
  this->target_temperature_ = target_temperature;
  return *this;
}
ClimateDeviceState ClimateDevice::StateCall::validate_() const {
  ClimateDeviceState state = this->device_->state;
  auto traits = this->device_->get_traits();
  ESP_LOGD(TAG, "'%s' Setting", this->device_->get_name().c_str());
  if (this->mode_.has_value()) {
    if (this->mode_ == CLIMATEDEVICE_MODE_OFF) {
      ESP_LOGD(TAG, "  Mode: off");
      state.mode = *this->mode_;
    } else if (traits.supports_auto_mode() && this->mode_ == CLIMATEDEVICE_MODE_AUTO) {
      ESP_LOGD(TAG, "  Mode: auto");
      state.mode = *this->mode_;
    } else if (traits.supports_cool_mode() && this->mode_ == CLIMATEDEVICE_MODE_COOL) {
      ESP_LOGD(TAG, "  Mode: cool");
      state.mode = *this->mode_;
    } else if (traits.supports_heat_mode() && this->mode_ == CLIMATEDEVICE_MODE_HEAT) {
      ESP_LOGD(TAG, "  Mode: heat");
      state.mode = *this->mode_;
    } else {
      ESP_LOGW(TAG, "  Mode: not supported!");
    }
  }
  if (this->target_temperature_.has_value()) {
    if (*this->target_temperature_ < traits.get_min_target_temperature() ||
        *this->target_temperature_ > traits.get_max_target_temperature()) {
      ESP_LOGW(TAG, "  Target temperature %.1f°C is not in the allowed range (%.1f-%.1f°C)", *this->target_temperature_,
               traits.get_min_target_temperature(), traits.get_max_target_temperature());
    } else {
      ESP_LOGD(TAG, "  Target temperature: %.1f°C", *this->target_temperature_);
      state.target_temperature = *this->target_temperature_;
    }
  }
  return state;
}
void ClimateDevice::StateCall::perform() const {
  ClimateDeviceState state = this->validate_();
  this->device_->state_callback_.call(state);
}
void ClimateDevice::StateCall::publish() const {
  ClimateDeviceState state = this->validate_();
  this->device_->state = state;
  this->device_->rtc_.save(&state);
  this->device_->publish_state_callback_.call();
}

}  // namespace climatedevice

ESPHOME_NAMESPACE_END

#endif  // USE_CLIMATEDEVICE
