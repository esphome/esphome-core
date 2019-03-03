#include "esphome/defines.h"

#ifdef USE_HLW8012

#include "esphome/sensor/hlw8012.h"
#include "esphome/log.h"
#include "esphome/espmath.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.hlw8012";

static const uint32_t HLW8012_CLOCK_FREQUENCY = 3579000;
static const float HLW8012_REFERENCE_VOLTAGE = 2.43f;

HLW8012Component::HLW8012Component(GPIOPin *sel_pin, uint8_t cf_pin, uint8_t cf1_pin, uint32_t update_interval)
    : PollingComponent(update_interval),
      sel_pin_(sel_pin),
      cf_(GPIOInputPin(cf_pin, INPUT_PULLUP).copy()),
      cf1_(GPIOInputPin(cf1_pin, INPUT_PULLUP).copy()) {}
void HLW8012Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up HLW8012...");
  this->sel_pin_->setup();
  this->sel_pin_->digital_write(this->current_mode_);

  if (!this->cf_.pulse_counter_setup() || !this->cf1_.pulse_counter_setup()) {
    this->mark_failed();
    return;
  }
}
void HLW8012Component::dump_config() {
  ESP_LOGCONFIG(TAG, "HLW8012:");
  LOG_PIN("  SEL Pin: ", this->sel_pin_);
  LOG_PIN("  CF Pin: ", this->cf_.get_pin());
  LOG_PIN("  CF1 Pin: ", this->cf1_.get_pin());
  ESP_LOGCONFIG(TAG, "  Change measurement mode every %u", this->change_mode_every_);
  ESP_LOGCONFIG(TAG, "  Current resistor: %.1f mΩ", this->current_resistor_ * 1000.0f);
  ESP_LOGCONFIG(TAG, "  Voltage Divider: %.1f", this->voltage_divider_);
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Voltage", this->voltage_sensor_);
  LOG_SENSOR("  ", "Current", this->current_sensor_);
  LOG_SENSOR("  ", "Power", this->power_sensor_);
}
float HLW8012Component::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
void HLW8012Component::update() {
  pulse_counter_t raw_cf = this->cf_.read_raw_value();
  pulse_counter_t raw_cf1 = this->cf1_.read_raw_value();
  float cf_hz = raw_cf / (this->get_update_interval() / 1000.0f);
  if (raw_cf <= 1) {
    // don't count single pulse as power
    cf_hz = 0.0f;
  }
  float cf1_hz = raw_cf1 / (this->get_update_interval() / 1000.0f);
  if (cf1_hz <= 1) {
    // don't count single pulse as anything
    cf1_hz = 0.0f;
  }

  if (this->nth_value_++ < 2) {
    return;
  }

  const float v_ref_squared = HLW8012_REFERENCE_VOLTAGE * HLW8012_REFERENCE_VOLTAGE;
  const float power_multiplier_micros =
      64000000.0f * v_ref_squared * this->voltage_divider_ / this->current_resistor_ / 24.0f / HLW8012_CLOCK_FREQUENCY;
  float power = cf_hz * power_multiplier_micros / 1000000.0f;

  if (this->change_mode_at_ != 0) {
    // Only read cf1 after one cycle. Apparently it's quite unstable after being changed.
    if (this->current_mode_) {
      const float current_multiplier_micros =
          512000000.0f * HLW8012_REFERENCE_VOLTAGE / this->current_resistor_ / 24.0f / HLW8012_CLOCK_FREQUENCY;
      float current = cf1_hz * current_multiplier_micros / 1000000.0f;
      ESP_LOGD(TAG, "Got power=%.1fW, current=%.1fA", power, current);
      if (this->current_sensor_ != nullptr) {
        this->current_sensor_->publish_state(current);
      }
    } else {
      const float voltage_multiplier_micros =
          256000000.0f * HLW8012_REFERENCE_VOLTAGE * this->voltage_divider_ / HLW8012_CLOCK_FREQUENCY;
      float voltage = cf1_hz * voltage_multiplier_micros / 1000000.0f;
      ESP_LOGD(TAG, "Got power=%.1fW, voltage=%.1fV", power, voltage);
      if (this->voltage_sensor_ != nullptr) {
        this->voltage_sensor_->publish_state(voltage);
      }
    }
  }

  if (this->power_sensor_ != nullptr) {
    this->power_sensor_->publish_state(power);
  }

  if (this->change_mode_at_++ == this->change_mode_every_) {
    this->current_mode_ = !this->current_mode_;
    ESP_LOGV(TAG, "Changing mode to %s mode", this->current_mode_ ? "CURRENT" : "VOLTAGE");
    this->change_mode_at_ = 0;
    this->sel_pin_->digital_write(this->current_mode_);
  }
}
HLW8012VoltageSensor *HLW8012Component::make_voltage_sensor(const std::string &name) {
  return this->voltage_sensor_ = new HLW8012VoltageSensor(name, this);
}
HLW8012CurrentSensor *HLW8012Component::make_current_sensor(const std::string &name) {
  return this->current_sensor_ = new HLW8012CurrentSensor(name, this);
}
HLW8012PowerSensor *HLW8012Component::make_power_sensor(const std::string &name) {
  return this->power_sensor_ = new HLW8012PowerSensor(name, this);
}
void HLW8012Component::set_change_mode_every(uint32_t change_mode_every) {
  this->change_mode_every_ = change_mode_every;
}
void HLW8012Component::set_current_resistor(float current_resistor) { this->current_resistor_ = current_resistor; }
void HLW8012Component::set_voltage_divider(float voltage_divider) { this->voltage_divider_ = voltage_divider; }

uint32_t HLW8012CurrentSensor::update_interval() {
  return this->parent_->get_update_interval() * this->parent_->change_mode_every_;
}
HLW8012CurrentSensor::HLW8012CurrentSensor(const std::string &name, HLW8012Component *parent)
    : EmptySensor(name), parent_(parent) {}

uint32_t HLW8012VoltageSensor::update_interval() {
  return this->parent_->get_update_interval() * this->parent_->change_mode_every_;
}
HLW8012VoltageSensor::HLW8012VoltageSensor(const std::string &name, HLW8012Component *parent)
    : EmptySensor(name), parent_(parent) {}

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_HLW8012
