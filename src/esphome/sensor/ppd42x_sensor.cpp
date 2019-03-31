#include "esphome/defines.h"

#ifdef USE_PPD42X_SENSOR

#include "esphome/sensor/ppd42x_sensor.h"

#include "esphome/log.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.ppd42x";

Ppd42xSensorComponent::Ppd42xSensorComponent(const std::string &name, GPIOPin *pm_10_0_pin, GPIOPin *pm_02_5_pin,
                                             uint32_t update_interval)
    : PollingSensorComponent(name, update_interval), pm_10_0_pin_(pm_10_0_pin), pm_02_5_pin_(pm_02_5_pin) {}
void Ppd42xSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up PPD42X Sensor...");
  this->pm_02_5_pin_->setup();
  this->pm_10_0_pin_->setup();
}
void Ppd42xSensorComponent::update() {
  uint32_t time_pm_10_0 =
      pulseIn(this->pm_10_0_pin_->get_pin(), uint8_t(!this->pm_10_0_pin_->is_inverted()), this->timeout_us_);
  uint32_t time_pm_02_5 =
      pulseIn(this->pm_02_5_pin_->get_pin(), uint8_t(!this->pm_02_5_pin_->is_inverted()), this->timeout_us_);

  ESP_LOGV(TAG, "PM10.0 took %uµs and PM 2.5 %uµs ", time_pm_10_0, time_pm_02_5);

  if (time_pm_10_0 == 0) {
    ESP_LOGD(TAG, "'%s' - PM10.0 measurement timed out!", this->name_.c_str());
    this->publish_state(NAN);
  } else {
    float result = Ppd42xSensorComponent::us_to_pm(this->timeout_us_, time_pm_10_0);
    this->publish_state(result);
    ESP_LOGD(TAG, "'%s' - Got PM10.0 Concentration: %.1f µg/m³", this->name_.c_str(), result);
    this->publish_state(result);
  }
  if (time_pm_02_5 == 0) {
    ESP_LOGD(TAG, "'%s' - PM 2.5 measurement timed out!", this->name_.c_str());
    this->publish_state(NAN);
  } else {
    float result = Ppd42xSensorComponent::us_to_pm(this->timeout_us_, time_pm_02_5);
    this->publish_state(result);
    ESP_LOGD(TAG, "'%s' - Got PM 2.5 Concentration: %.1f µg/m³", this->name_.c_str(), result);
    this->publish_state(result);
  }
}
void Ppd42xSensorComponent::dump_config() {
  LOG_SENSOR("", "PPD42X Sensor", this);
  LOG_PIN("  PM10.0  Pin: ", this->pm_10_0_pin_);
  LOG_PIN("  PM 2.5  Pin: ", this->pm_02_5_pin_);
  ESP_LOGCONFIG(TAG, "  Timeout: %u µs", this->timeout_us_);
  LOG_UPDATE_INTERVAL(this);
}
float Ppd42xSensorComponent::us_to_pm(uint32_t sample_length, uint32_t time_pm) {
  float ratio = time_pm / (sample_length * 10.0);
  return 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;
}

float Ppd42xSensorComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
std::string Ppd42xSensorComponent::unit_of_measurement() { return "µg/m³"; }
std::string Ppd42xSensorComponent::icon() { return "mdi:arrow-expand-vertical"; }

int8_t Ppd42xSensorComponent::accuracy_decimals() {
  return 2;  // cm precision
}
void Ppd42xSensorComponent::set_timeout_us(uint32_t timeout_us) { this->timeout_us_ = timeout_us; }

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PPD42X_SENSOR
