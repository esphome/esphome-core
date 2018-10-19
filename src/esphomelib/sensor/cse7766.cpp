#include "esphomelib/defines.h"

#ifdef USE_CSE7766

#include "esphomelib/sensor/cse7766.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.cse7766";

void CSE7766Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up CSE7766...");
}
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
    uint8_t data;
    if (!this->read_byte(&data) || !this->check_byte_(data, this->raw_data_index_)) {
      ESP_LOGW(TAG, "Error reading CSE7766 data.");
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
bool CSE7766Component::check_byte_(uint8_t byte, uint8_t index) {
  if (index == 0) {
    // Header - 0x55
    if (byte == 0xAA) {
      ESP_LOGW(TAG, "CSE7766 not calibrated!");
      return false;
    }
    if ((byte & 0xF0) == 0xF0) {
      ESP_LOGW(TAG, "CSE7766 reports abnormal hardware: ");
      if ((byte >> 3) & 1) {
        ESP_LOGW(TAG, "  Voltage cycle exceeds range.");
      }
      if ((byte >> 2) & 1) {
        ESP_LOGW(TAG, "  Current cycle exceeds range.");
      }
      if ((byte >> 1) & 1) {
        ESP_LOGW(TAG, "  Power cycle exceeds range.");
      }
      if ((byte >> 0) & 1) {
        ESP_LOGW(TAG, "  Coefficient storage area is abnormal.");
      }
      return false;
    }
    if (byte != 0x55) {
      ESP_LOGW(TAG, "Invalid Header Start from CSE7766: 0x%02X!", byte);
      return false;
    }
    return true;
  }

  if (index == 1) {
    if (byte != 0xFA) {
      ESP_LOGW(TAG, "Invalid Header 2 Start: 0x%02X!", byte);
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
    ESP_LOGD(TAG, "Got voltage=%.1fV", voltage);
    if (this->voltage_ != nullptr)
      this->voltage_->publish_state(voltage);
  }

  if ((adj >> 5 != 0) && power_cycle != 0 && current_cycle != 0) {
    // indicates current cycle of serial port outputted is a complete cycle;
    float current = current_calib / float(current_cycle);
    ESP_LOGD(TAG, "Got current=%.1fA", current);
    if (this->current_ != nullptr)
      this->current_->publish_state(current);
  }

  if ((adj >> 4 != 0) && power_cycle != 0) {
    // power cycle of serial port outputted is a complete cycle;
    float active_power = power_calib / float(power_cycle);
    ESP_LOGD(TAG, "Got power=%.1fW", active_power);
    if (this->power_ != nullptr)
      this->power_->publish_state(active_power);
  }
}
uint32_t CSE7766Component::get_24_bit_uint(uint8_t start_index) {
  return (uint32_t(this->raw_data_[start_index]) << 16) |
      (uint32_t(this->raw_data_[start_index + 1]) << 8) |
      uint32_t(this->raw_data_[start_index + 2]);
}

CSE7766Component::CSE7766Component(UARTComponent *parent) : UARTDevice(parent) {

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

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_CSE7766
