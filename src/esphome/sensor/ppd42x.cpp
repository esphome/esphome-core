#include "esphome/defines.h"

#ifdef USE_PPD42X

#include "esphome/sensor/ppd42x.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.ppd42x";

void PPD42XComponent::loop() {
  const uint32_t now = millis();
  if (now - this->last_transmission_ >= 500) {
    // last transmission too long ago. Reset RX index.
    this->data_index_ = 0;
  }

  if (this->available() == 0)
    return;

  this->last_transmission_ = now;
  while (this->available() != 0) {
    this->read_byte(&this->data_[this->data_index_]);
    auto check = this->check_byte_();
    if (!check.has_value()) {
      // finished
      this->parse_data_();
      this->data_index_ = 0;
    } else if (!*check) {
      // wrong data
      this->data_index_ = 0;
    } else {
      // next byte
      this->data_index_++;
    }
  }
}
float PPD42XComponent::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
optional<bool> PPD42XComponent::check_byte_() {
  uint8_t index = this->data_index_;
  uint8_t byte = this->data_[index];

  if (index == 0)
    return byte == 0x42;

  if (index == 1)
    return byte == 0x4D;

  if (index == 2)
    return true;

  uint16_t payload_length = this->get_16_bit_uint_(2);
  if (index == 3) {
    bool length_matches = false;
    switch (this->type_) {
      case PPD42X_TYPE:
        length_matches = payload_length == 28 || payload_length == 20;
        break;
      case PPD42X_TYPE_NJ:
        length_matches = payload_length == 28;
        break;
      case PPD42X_TYPE_NS:
        length_matches = payload_length == 36;
        break;
    }

    if (!length_matches) {
      ESP_LOGW(TAG, "PPD42X length %u doesn't match. Are you using the correct PPD42X type?", payload_length);
      return false;
    }
    return true;
  }

  // start (16bit) + length (16bit) + DATA (payload_length-2 bytes) + checksum (16bit)
  uint8_t total_size = 4 + payload_length;

  if (index < total_size - 1)
    return true;

  // checksum is without checksum bytes
  uint16_t checksum = 0;
  for (uint8_t i = 0; i < total_size - 2; i++)
    checksum += this->data_[i];

  uint16_t check = this->get_16_bit_uint_(total_size - 2);
  if (checksum != check) {
    ESP_LOGW(TAG, "PPD42X checksum mismatch! 0x%02X!=0x%02X", checksum, check);
    return false;
  }

  return {};
}

void PPD42XComponent::parse_data_() {
  switch (this->type_) {
    case PPD42X_TYPE: {
      uint16_t pm_2_5_concentration = this->get_16_bit_uint_(12);
      uint16_t pm_10_0_concentration = this->get_16_bit_uint_(14);
      ESP_LOGD(TAG, "Got PM2.5 Concentration %u pcs/L, PM10.0 Concentration: %u pcs/L", pm_2_5_concentration,
               pm_10_0_concentration);
      if (this->pm_2_5_sensor_ != nullptr)
        this->pm_2_5_sensor_->publish_state(pm_2_5_concentration);
      if (this->pm_10_0_sensor_ != nullptr)
        this->pm_10_0_sensor_->publish_state(pm_10_0_concentration);
      break;
    }
    case PPD42X_TYPE_NJ: {
      uint16_t pm_2_5_concentration = this->get_16_bit_uint_(12);
      ESP_LOGD(TAG, "Got PM2.5 Concentration: %u pcs/L ", pm_2_5_concentration);
      if (this->pm_2_5_sensor_ != nullptr)
        this->pm_2_5_sensor_->publish_state(pm_2_5_concentration);
      break;
    }
    case PPD42X_TYPE_NS: {
      uint16_t pm_2_5_concentration = this->get_16_bit_uint_(12);
      ESP_LOGD(TAG, "Got PM2.5 Concentration: %u pcs/L ", pm_2_5_concentration);
      if (this->pm_2_5_sensor_ != nullptr)
        this->pm_2_5_sensor_->publish_state(pm_2_5_concentration);
      break;
    }
  }

  this->status_clear_warning();
}
uint16_t PPD42XComponent::get_16_bit_uint_(uint8_t start_index) {
  return (uint16_t(this->data_[start_index]) << 8) | uint16_t(this->data_[start_index + 1]);
}
PPD42XSensor *PPD42XComponent::make_pm_2_5_sensor(const std::string &name) {
  return this->pm_2_5_sensor_ = new PPD42XSensor(name, PPD42X_SENSOR_TYPE_PM_2_5);
}
PPD42XSensor *PPD42XComponent::make_pm_10_0_sensor(const std::string &name) {
  return this->pm_10_0_sensor_ = new PPD42XSensor(name, PPD42X_SENSOR_TYPE_PM_10_0);
}
PPD42XComponent::PPD42XComponent(UARTComponent *parent, PPD42XType type) : UARTDevice(parent), type_(type) {}
void PPD42XComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "PPD42X:");
  LOG_SENSOR("  ", "PM2.5", this->pm_2_5_sensor_);
  LOG_SENSOR("  ", "PM10.0", this->pm_10_0_sensor_);

}

std::string PPD42XSensor::unit_of_measurement() {
  switch (this->type_) {
    case PPD42X_SENSOR_TYPE_PM_2_5:
    case PPD42X_SENSOR_TYPE_PM_10_0:
      return UNIT_MICROGRAMS_PER_CUBIC_METER;
  }
  return "";
}
std::string PPD42XSensor::icon() {
  switch (this->type_) {
    case PPD42X_SENSOR_TYPE_PM_2_5:
    case PPD42X_SENSOR_TYPE_PM_10_0:
      // Not the ideal icon, but I can't find a better one.
      return ICON_CHEMICAL_WEAPON;
  }
  return "";
}
int8_t PPD42XSensor::accuracy_decimals() {
  switch (this->type_) {
    case PPD42X_SENSOR_TYPE_PM_2_5:
    case PPD42X_SENSOR_TYPE_PM_10_0:
      return 0;
  }

  return 0;
}
PPD42XSensor::PPD42XSensor(const std::string &name, PPD42XSensorType type) : Sensor(name), type_(type) {}

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PPD42X
