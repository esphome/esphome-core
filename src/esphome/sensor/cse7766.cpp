#include "esphome/defines.h"

#ifdef USE_CSE7766

#include "esphome/sensor/cse7766.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.cse7766";

void CSE7766Component::loop() {
  const uint32_t now = millis();
  if (now - this->last_transmission_ >= 500) {
    // last transmission too long ago. Reset RX index.
    this->raw_data_index_ = 0;
  }

  if (this->available() == 0)
    return;

  this->last_transmission_ = now;
  while (this->available() != 0) {
    this->read_byte(&this->raw_data_[this->raw_data_index_]);
    if (!this->check_byte_()) {
      this->raw_data_index_ = 0;
      this->status_set_warning();
      continue;
    }

    if (this->raw_data_index_ == 23) {
      this->parse_data_();
      this->status_clear_warning();
    }

    this->raw_data_index_ = (this->raw_data_index_ + 1) % 24;
  }
}
float CSE7766Component::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
bool CSE7766Component::check_byte_() {
  uint8_t index = this->raw_data_index_;
  uint8_t byte = this->raw_data_[index];
  if (index == 0) {
    // Header - 0x55
    // These messages are verbose because the CSE7766
    // reports these when no load is attached. If the checksum doesn't match
    // though, then we should print a warning.
    if (byte == 0xAA) {
      ESP_LOGV(TAG, "CSE7766 not calibrated!");
      return false;
    }
    if ((byte & 0xF0) == 0xF0) {
      ESP_LOGV(TAG, "CSE7766 reports abnormal hardware: (0x%02X)", byte);
      if ((byte >> 3) & 1) {
        ESP_LOGV(TAG, "  Voltage cycle exceeds range.");
      }
      if ((byte >> 2) & 1) {
        ESP_LOGV(TAG, "  Current cycle exceeds range.");
      }
      if ((byte >> 1) & 1) {
        ESP_LOGV(TAG, "  Power cycle exceeds range.");
      }
      if ((byte >> 0) & 1) {
        ESP_LOGV(TAG, "  Coefficient storage area is abnormal.");
      }
      return false;
    }
    if (byte != 0x55) {
      ESP_LOGV(TAG, "Invalid Header Start from CSE7766: 0x%02X!", byte);
      return false;
    }
    return true;
  }

  if (index == 1) {
    if (byte != 0x5A) {
      ESP_LOGV(TAG, "Invalid Header 2 Start: 0x%02X!", byte);
      return false;
    }

    return true;
  }

  if (index < 23)
    return true;

  uint8_t checksum = 0;
  for (uint8_t i = 2; i < 23; i++)
    checksum += this->raw_data_[i];

  if (checksum != this->raw_data_[23]) {
    ESP_LOGW(TAG, "Invalid checksum from CSE7766: 0x%02X != 0x%02X", checksum, this->raw_data_[23]);
    return false;
  }

  return true;
}
void CSE7766Component::parse_data_() {
  ESP_LOGVV(TAG, "CSE7766 Data: ");
  for (uint8_t i = 0; i < 23; i++) {
    ESP_LOGVV(TAG, "  i=%u: 0b" BYTE_TO_BINARY_PATTERN " (0x%02X)",
        i, BYTE_TO_BINARY(this->raw_data_[i]), this->raw_data_[i]);
  }
  const uint32_t now = micros();
  const float d = (now - this->last_reading_) / 1000.0f;
  this->last_reading_ = now;

  uint32_t voltage_calib = this->get_24_bit_uint(2);
  uint32_t voltage_cycle = this->get_24_bit_uint(5);
  uint32_t current_calib = this->get_24_bit_uint(8);
  uint32_t current_cycle = this->get_24_bit_uint(11);
  uint32_t power_calib = this->get_24_bit_uint(14);
  uint32_t power_cycle = this->get_24_bit_uint(17);

  uint8_t adj = this->raw_data_[20];

  if ((adj >> 6 != 0) && voltage_cycle != 0) {
    // voltage cycle of serial port outputted is a complete cycle;
    float voltage = voltage_calib / float(voltage_cycle);
    this->voltage_acc_ += voltage * d;
  }

  if ((adj >> 5 != 0) && power_cycle != 0 && current_cycle != 0) {
    // indicates current cycle of serial port outputted is a complete cycle;
    float current = current_calib / float(current_cycle);
    this->current_acc_ += current * d;
  }

  if ((adj >> 4 != 0) && power_cycle != 0) {
    // power cycle of serial port outputted is a complete cycle;
    float active_power = power_calib / float(power_cycle);
    this->power_acc_ += active_power * d;
  }
}
void CSE7766Component::update() {
  const uint32_t now = millis();
  uint32_t d = now - this->last_update_;
  this->last_update_ = now;
  float voltage = this->voltage_acc_ / d;
  float current = this->current_acc_ / d;
  float power = this->power_acc_ / d;
  ESP_LOGD(TAG, "Got voltage=%.1fV current=%.1fA power=%.1fW",
      voltage, current, power);

  if (this->voltage_ != nullptr)
    this->voltage_->publish_state(voltage);
  if (this->current_ != nullptr)
    this->current_->publish_state(current);
  if (this->power_ != nullptr)
    this->power_->publish_state(power);

  this->voltage_acc_ = this->current_acc_ = this->power_acc_ = 0;
}
void CSE7766Component::setup() {
  this->last_reading_ = micros();
  this->last_update_ = millis();
}
uint32_t CSE7766Component::get_24_bit_uint(uint8_t start_index) {
  return (uint32_t(this->raw_data_[start_index]) << 16) |
      (uint32_t(this->raw_data_[start_index + 1]) << 8) |
      uint32_t(this->raw_data_[start_index + 2]);
}

CSE7766Component::CSE7766Component(UARTComponent *parent, uint32_t update_interval)
    : UARTDevice(parent), PollingComponent(update_interval) {

}
CSE7766VoltageSensor *CSE7766Component::make_voltage_sensor(const std::string &name) {
  return this->voltage_ = new CSE7766VoltageSensor(name);
}
CSE7766CurrentSensor *CSE7766Component::make_current_sensor(const std::string &name) {
  return this->current_ = new CSE7766CurrentSensor(name);
}
CSE7766PowerSensor *CSE7766Component::make_power_sensor(const std::string &name) {
  return this->power_ = new CSE7766PowerSensor(name);
}
void CSE7766Component::dump_config() {
  ESP_LOGCONFIG(TAG, "CSE7766:");
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Voltage", this->voltage_);
  LOG_SENSOR("  ", "Current", this->current_);
  LOG_SENSOR("  ", "Power", this->power_);
}

} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_CSE7766
