#include "esphome/defines.h"

#ifdef USE_PULSE_COUNTER_SENSOR

#include "esphome/sensor/pulse_counter.h"

#include "esphome/log.h"
#include "esphome/esphal.h"
#include "esphome/espmath.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.pulse_counter";

PulseCounterBase::PulseCounterBase(GPIOPin *pin) : pin_(pin) {
#ifdef ARDUINO_ARCH_ESP32
  this->pcnt_unit_ = next_pcnt_unit;
  next_pcnt_unit = pcnt_unit_t(int(next_pcnt_unit) + 1);  // NOLINT
#endif
}

PulseCounterSensorComponent::PulseCounterSensorComponent(const std::string &name, GPIOPin *pin,
                                                         uint32_t update_interval)
    : PollingSensorComponent(name, update_interval), PulseCounterBase(pin) {}
void PulseCounterSensorComponent::set_edge_mode(PulseCounterCountMode rising_edge_mode,
                                                PulseCounterCountMode falling_edge_mode) {
  this->rising_edge_mode_ = rising_edge_mode;
  this->falling_edge_mode_ = falling_edge_mode;
}

const char *EDGE_MODE_TO_STRING[] = {"DISABLE", "INCREMENT", "DECREMENT"};

GPIOPin *PulseCounterBase::get_pin() { return this->pin_; }

#ifdef ARDUINO_ARCH_ESP8266
void ICACHE_RAM_ATTR PulseCounterBase::gpio_intr(PulseCounterBase *arg) {
  const uint32_t now = micros();
  const bool discard = now - arg->last_pulse_ < arg->filter_us_;
  arg->last_pulse_ = now;
  if (discard)
    return;

  PulseCounterCountMode mode = arg->isr_pin_->digital_read() ? arg->rising_edge_mode_ : arg->falling_edge_mode_;
  switch (mode) {
    case PULSE_COUNTER_DISABLE:
      break;
    case PULSE_COUNTER_INCREMENT:
      arg->counter_++;
      break;
    case PULSE_COUNTER_DECREMENT:
      arg->counter_--;
      break;
  }
}
bool PulseCounterBase::pulse_counter_setup() {
  this->pin_->setup();
  this->isr_pin_ = this->pin_->to_isr();
  this->pin_->attach_interrupt(PulseCounterBase::gpio_intr, this, CHANGE);
  return true;
}
pulse_counter_t PulseCounterBase::read_raw_value() {
  pulse_counter_t counter = this->counter_;
  pulse_counter_t ret = counter - this->last_value_;
  this->last_value_ = counter;
  return ret;
}
#endif

#ifdef ARDUINO_ARCH_ESP32
bool PulseCounterBase::pulse_counter_setup() {
  ESP_LOGCONFIG(TAG, "    PCNT Unit Number: %u", this->pcnt_unit_);

  pcnt_count_mode_t rising = PCNT_COUNT_DIS, falling = PCNT_COUNT_DIS;
  switch (this->rising_edge_mode_) {
    case PULSE_COUNTER_DISABLE:
      rising = PCNT_COUNT_DIS;
      break;
    case PULSE_COUNTER_INCREMENT:
      rising = PCNT_COUNT_INC;
      break;
    case PULSE_COUNTER_DECREMENT:
      rising = PCNT_COUNT_DEC;
      break;
  }
  switch (this->falling_edge_mode_) {
    case PULSE_COUNTER_DISABLE:
      falling = PCNT_COUNT_DIS;
      break;
    case PULSE_COUNTER_INCREMENT:
      falling = PCNT_COUNT_INC;
      break;
    case PULSE_COUNTER_DECREMENT:
      falling = PCNT_COUNT_DEC;
      break;
  }

  pcnt_config_t pcnt_config = {
      .pulse_gpio_num = this->pin_->get_pin(),
      .ctrl_gpio_num = PCNT_PIN_NOT_USED,
      .lctrl_mode = PCNT_MODE_KEEP,
      .hctrl_mode = PCNT_MODE_KEEP,
      .pos_mode = rising,
      .neg_mode = falling,
      .counter_h_lim = 0,
      .counter_l_lim = 0,
      .unit = this->pcnt_unit_,
      .channel = PCNT_CHANNEL_0,
  };
  esp_err_t error = pcnt_unit_config(&pcnt_config);
  if (error != ESP_OK) {
    ESP_LOGE(TAG, "Configuring Pulse Counter failed: %s", esp_err_to_name(error));
    return false;
  }

  if (this->filter_us_ != 0) {
    uint16_t filter_val = std::min(this->filter_us_ * 80u, 1023u);
    ESP_LOGCONFIG(TAG, "    Filter Value: %uus (val=%u)", this->filter_us_, filter_val);
    error = pcnt_set_filter_value(this->pcnt_unit_, filter_val);
    if (error != ESP_OK) {
      ESP_LOGE(TAG, "Setting filter value failed: %s", esp_err_to_name(error));
      return false;
    }
    error = pcnt_filter_enable(this->pcnt_unit_);
    if (error != ESP_OK) {
      ESP_LOGE(TAG, "Enabling filter failed: %s", esp_err_to_name(error));
      return false;
    }
  }

  error = pcnt_counter_pause(this->pcnt_unit_);
  if (error != ESP_OK) {
    ESP_LOGE(TAG, "Pausing pulse counter failed: %s", esp_err_to_name(error));
    return false;
  }
  error = pcnt_counter_clear(this->pcnt_unit_);
  if (error != ESP_OK) {
    ESP_LOGE(TAG, "Clearing pulse counter failed: %s", esp_err_to_name(error));
    return false;
  }
  error = pcnt_counter_resume(this->pcnt_unit_);
  if (error != ESP_OK) {
    ESP_LOGE(TAG, "Resuming pulse counter failed: %s", esp_err_to_name(error));
    return false;
  }
  return true;
}
pulse_counter_t PulseCounterBase::read_raw_value() {
  pulse_counter_t counter;
  pcnt_get_counter_value(this->pcnt_unit_, &counter);
  pulse_counter_t ret = counter - this->last_value_;
  this->last_value_ = counter;
  return ret;
}
#endif

void PulseCounterSensorComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up pulse counter '%s'...", this->name_.c_str());
  if (!this->pulse_counter_setup()) {
    this->mark_failed();
    return;
  }
}

void PulseCounterSensorComponent::dump_config() {
  LOG_SENSOR("", "Pulse Counter", this);
  LOG_PIN("  Pin: ", this->pin_);
  ESP_LOGCONFIG(TAG, "  Rising Edge: %s", EDGE_MODE_TO_STRING[this->rising_edge_mode_]);
  ESP_LOGCONFIG(TAG, "  Falling Edge: %s", EDGE_MODE_TO_STRING[this->falling_edge_mode_]);
  ESP_LOGCONFIG(TAG, "  Filtering pulses shorter than %u µs", this->filter_us_);
}

void PulseCounterSensorComponent::update() {
  pulse_counter_t raw = this->read_raw_value();
  float value = (60000.0f * raw) / float(this->get_update_interval());  // per minute

  ESP_LOGD(TAG, "'%s': Retrieved counter: %0.2f pulses/min", this->get_name().c_str(), value);
  this->publish_state(value);
}

float PulseCounterSensorComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
std::string PulseCounterSensorComponent::unit_of_measurement() { return "pulses/min"; }
std::string PulseCounterSensorComponent::icon() { return "mdi:pulse"; }
int8_t PulseCounterSensorComponent::accuracy_decimals() { return 2; }
void PulseCounterSensorComponent::set_filter_us(uint32_t filter_us) { this->filter_us_ = filter_us; }

#ifdef ARDUINO_ARCH_ESP32
pcnt_unit_t next_pcnt_unit = PCNT_UNIT_0;
#endif

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PULSE_COUNTER_SENSOR
