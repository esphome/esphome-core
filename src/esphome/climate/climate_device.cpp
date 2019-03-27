#include "esphome/defines.h"

#ifdef USE_CLIMATE

#include "esphome/climate/climate_device.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace climate {

static const char *TAG = "climate.climate";

void ClimateCall::perform() const {
  ESP_LOGD(TAG, "'%s' - Setting", this->parent_->get_name().c_str());
  if (this->mode_.has_value()) {
    const char *mode_s = climate_mode_to_string(*this->mode_);
    ESP_LOGD(TAG, "  Mode: %s", mode_s);
  }
  if (this->target_temperature_.has_value()) {
    ESP_LOGD(TAG, "  Target Temperature: %.2f", *this->target_temperature_);
  }
  if (this->target_temperature_low_.has_value()) {
    ESP_LOGD(TAG, "  Target Temperature Min: %.2f", *this->target_temperature_low_);
  }
  if (this->target_temperature_high_.has_value()) {
    ESP_LOGD(TAG, "  Target Temperature Max: %.2f", *this->target_temperature_high_);
  }
  if (this->away_.has_value()) {
    ESP_LOGD(TAG, "  Away Mode: %s", ONOFF(*this->away_));
  }
  this->parent_->control(*this);
}
ClimateCall &ClimateCall::set_mode(ClimateMode mode) {
  if (this->parent_->get_traits().supports_mode(mode)) {
    this->mode_ = mode;
  } else {
    ESP_LOGW(TAG, "'%s' - Mode %s is not supported", this->parent_->get_name().c_str(), climate_mode_to_string(mode));
  }
  return *this;
}
ClimateCall &ClimateCall::set_mode(const std::string &mode) {
  if (str_equals_case_insensitive(mode, "OFF")) {
    this->set_mode(CLIMATE_MODE_OFF);
  } else if (str_equals_case_insensitive(mode, "AUTO")) {
    this->set_mode(CLIMATE_MODE_AUTO);
  } else if (str_equals_case_insensitive(mode, "COOL")) {
    this->set_mode(CLIMATE_MODE_COOL);
  } else if (str_equals_case_insensitive(mode, "HEAT")) {
    this->set_mode(CLIMATE_MODE_HEAT);
  } else {
    ESP_LOGW(TAG, "'%s' - Unrecognized mode %s", this->parent_->get_name().c_str(), mode.c_str());
  }
  return *this;
}
ClimateCall &ClimateCall::set_target_temperature(float target_temperature) {
  if (!this->parent_->get_traits().get_supports_two_point_target_temperature()) {
    this->target_temperature_ = target_temperature;
  } else {
    ESP_LOGW(TAG, "'%s' - Climate device has two set point target temperature!", this->parent_->get_name().c_str());
  }
  return *this;
}
ClimateCall &ClimateCall::set_target_temperature_low(float target_temperature_low) {
  if (this->parent_->get_traits().get_supports_two_point_target_temperature()) {
    this->target_temperature_low_ = target_temperature_low;
  } else {
    ESP_LOGW(TAG, "'%s' - Climate device has single point target temperature!", this->parent_->get_name().c_str());
  }
  return *this;
}
ClimateCall &ClimateCall::set_target_temperature_high(float target_temperature_high) {
  if (this->parent_->get_traits().get_supports_two_point_target_temperature()) {
    this->target_temperature_high_ = target_temperature_high;
  } else {
    ESP_LOGW(TAG, "'%s' - Climate device has single point target temperature!", this->parent_->get_name().c_str());
  }
  return *this;
}
const optional<ClimateMode> &ClimateCall::get_mode() const { return this->mode_; }
const optional<float> &ClimateCall::get_target_temperature() const { return this->target_temperature_; }
const optional<float> &ClimateCall::get_target_temperature_low() const { return this->target_temperature_low_; }
const optional<float> &ClimateCall::get_target_temperature_high() const { return this->target_temperature_high_; }
const optional<bool> &ClimateCall::get_away() const { return this->away_; }
ClimateCall &ClimateCall::set_away(bool away) {
  if (this->parent_->get_traits().get_supports_away()) {
    this->away_ = away;
  } else {
    ESP_LOGW(TAG, "'%s' - Climate device does not support away mode!", this->parent_->get_name().c_str());
  }
  return *this;
}

void ClimateDevice::add_on_state_callback(std::function<void()> &&callback) {
  this->state_callback_.add(std::move(callback));
}

/// Struct used to save the state of the climate device in restore memory.
struct ClimateDeviceRestoreState {
  ClimateMode mode;
  union {
    float target_temperature;
    struct {
      float target_temperature_low;
      float target_temperature_high;
    };
  };
  bool away;
} __attribute__((packed));

bool ClimateDevice::restore_state_() {
  this->rtc_ = global_preferences.make_preference<ClimateDeviceRestoreState>(this->get_object_id_hash());
  ClimateDeviceRestoreState recovered;
  if (!this->rtc_.load(&recovered))
    return false;

  auto call = this->make_call();
  auto traits = this->get_traits();
  call.set_mode(recovered.mode);
  if (traits.get_supports_two_point_target_temperature()) {
    call.set_target_temperature_low(recovered.target_temperature_low);
    call.set_target_temperature_high(recovered.target_temperature_high);
  } else {
    call.set_target_temperature(recovered.target_temperature);
  }
  if (traits.get_supports_away()) {
    call.set_away(recovered.away);
  }
  call.perform();
  return true;
}
void ClimateDevice::save_state_() {
  ClimateDeviceRestoreState state{};
  // initialize as zero to prevent random data on stack triggering erase
  memset(&state, 0, sizeof(ClimateDeviceRestoreState));

  state.mode = this->mode;
  auto traits = this->get_traits();
  if (traits.get_supports_two_point_target_temperature()) {
    state.target_temperature_low = this->target_temperature_low;
    state.target_temperature_high = this->target_temperature_high;
  } else {
    state.target_temperature = this->target_temperature;
  }
  if (traits.get_supports_away()) {
    state.away = this->away;
  }

  this->rtc_.save(&state);
}
void ClimateDevice::publish_state() {
  ESP_LOGD(TAG, "'%s' - Sending state:", this->name_.c_str());
  auto traits = this->get_traits();

  ESP_LOGD(TAG, "  Mode: %s", climate_mode_to_string(this->mode));
  if (traits.get_supports_current_temperature()) {
    ESP_LOGD(TAG, "  Current Temperature: %.2f°C", this->current_temperature);
  }
  if (traits.get_supports_two_point_target_temperature()) {
    ESP_LOGD(TAG, "  Target Temperature: Low: %.2f°C High: %.2f°C", this->target_temperature_low,
             this->target_temperature_high);
  } else {
    ESP_LOGD(TAG, "  Target Temperature: %.2f°C", this->target_temperature);
  }
  if (traits.get_supports_away()) {
    ESP_LOGD(TAG, "  Away: %s", ONOFF(this->away));
  }

  // Send state to frontend
  this->state_callback_.call();
  // Save state
  this->save_state_();
}
uint32_t ClimateDevice::hash_base() { return 3104134496UL; }

ClimateTraits ClimateDevice::get_traits() {
  auto traits = this->traits();
  if (this->visual_min_temperature_override_.has_value()) {
    traits.set_visual_min_temperature(*this->visual_min_temperature_override_);
  }
  if (this->visual_max_temperature_override_.has_value()) {
    traits.set_visual_max_temperature(*this->visual_max_temperature_override_);
  }
  if (this->visual_temperature_step_override_.has_value()) {
    traits.set_visual_temperature_step(*this->visual_temperature_step_override_);
  }
  return traits;
}

#ifdef USE_MQTT_CLIMATE
MQTTClimateComponent *ClimateDevice::get_mqtt() const { return this->mqtt_; }
void ClimateDevice::set_mqtt(MQTTClimateComponent *mqtt) { this->mqtt_ = mqtt; }
#endif

void ClimateDevice::set_visual_min_temperature_override(const optional<float> &visual_min_temperature_override) {
  visual_min_temperature_override_ = visual_min_temperature_override;
}
void ClimateDevice::set_visual_max_temperature_override(const optional<float> &visual_max_temperature_override) {
  visual_max_temperature_override_ = visual_max_temperature_override;
}
void ClimateDevice::set_visual_temperature_step_override(const optional<float> &visual_temperature_step_override) {
  visual_temperature_step_override_ = visual_temperature_step_override;
}
ClimateDevice::ClimateDevice(const std::string &name) : Nameable(name) {}
ClimateDevice::ClimateDevice() : ClimateDevice("") {}

}  // namespace climate

ESPHOME_NAMESPACE_END

#endif  // USE_CLIMATE
