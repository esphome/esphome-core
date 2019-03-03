#include "esphome/defines.h"

#ifdef USE_PMSX003

#include "esphome/sensor/pmsx003.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.pmsx003";

void PMSX003Component::loop() {
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
float PMSX003Component::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
optional<bool> PMSX003Component::check_byte_() {
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
      case PMSX003_TYPE_X003:
        length_matches = payload_length == 28 || payload_length == 20;
        break;
      case PMSX003_TYPE_5003T:
        length_matches = payload_length == 28;
        break;
      case PMSX003_TYPE_5003ST:
        length_matches = payload_length == 36;
        break;
    }

    if (!length_matches) {
      ESP_LOGW(TAG, "PMSX003 length %u doesn't match. Are you using the correct PMSX003 type?", payload_length);
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
    ESP_LOGW(TAG, "PMSX003 checksum mismatch! 0x%02X!=0x%02X", checksum, check);
    return false;
  }

  return {};
}

void PMSX003Component::parse_data_() {
  switch (this->type_) {
    case PMSX003_TYPE_X003: {
      uint16_t pm_1_0_concentration = this->get_16_bit_uint_(10);
      uint16_t pm_2_5_concentration = this->get_16_bit_uint_(12);
      uint16_t pm_10_0_concentration = this->get_16_bit_uint_(14);
      ESP_LOGD(TAG,
               "Got PM1.0 Concentration: %u µg/m^3, PM2.5 Concentration %u µg/m^3, PM10.0 Concentration: %u µg/m^3",
               pm_1_0_concentration, pm_2_5_concentration, pm_10_0_concentration);
      if (this->pm_1_0_sensor_ != nullptr)
        this->pm_1_0_sensor_->publish_state(pm_1_0_concentration);
      if (this->pm_2_5_sensor_ != nullptr)
        this->pm_2_5_sensor_->publish_state(pm_2_5_concentration);
      if (this->pm_10_0_sensor_ != nullptr)
        this->pm_10_0_sensor_->publish_state(pm_10_0_concentration);
      break;
    }
    case PMSX003_TYPE_5003T: {
      uint16_t pm_2_5_concentration = this->get_16_bit_uint_(12);
      float temperature = this->get_16_bit_uint_(24) / 10.0f;
      float humidity = this->get_16_bit_uint_(26) / 10.0f;
      ESP_LOGD(TAG, "Got PM2.5 Concentration: %u µg/m^3, Temperature: %.1f°C, Humidity: %.1f%%", pm_2_5_concentration,
               temperature, humidity);
      if (this->pm_2_5_sensor_ != nullptr)
        this->pm_2_5_sensor_->publish_state(pm_2_5_concentration);
      if (this->temperature_sensor_ != nullptr)
        this->temperature_sensor_->publish_state(temperature);
      if (this->humidity_sensor_ != nullptr)
        this->humidity_sensor_->publish_state(humidity);
      break;
    }
    case PMSX003_TYPE_5003ST: {
      uint16_t pm_2_5_concentration = this->get_16_bit_uint_(12);
      uint16_t formaldehyde = this->get_16_bit_uint_(28);
      float temperature = this->get_16_bit_uint_(30) / 10.0f;
      float humidity = this->get_16_bit_uint_(32) / 10.0f;
      ESP_LOGD(TAG, "Got PM2.5 Concentration: %u µg/m^3, Temperature: %.1f°C, Humidity: %.1f%% Formaldehyde: %u µg/m^3",
               pm_2_5_concentration, temperature, humidity, formaldehyde);
      if (this->pm_2_5_sensor_ != nullptr)
        this->pm_2_5_sensor_->publish_state(pm_2_5_concentration);
      if (this->temperature_sensor_ != nullptr)
        this->temperature_sensor_->publish_state(temperature);
      if (this->humidity_sensor_ != nullptr)
        this->humidity_sensor_->publish_state(humidity);
      if (this->formaldehyde_sensor_ != nullptr)
        this->formaldehyde_sensor_->publish_state(formaldehyde);
      break;
    }
  }

  this->status_clear_warning();
}
uint16_t PMSX003Component::get_16_bit_uint_(uint8_t start_index) {
  return (uint16_t(this->data_[start_index]) << 8) | uint16_t(this->data_[start_index + 1]);
}
PMSX003Sensor *PMSX003Component::make_pm_1_0_sensor(const std::string &name) {
  return this->pm_1_0_sensor_ = new PMSX003Sensor(name, PMSX003_SENSOR_TYPE_PM_1_0);
}
PMSX003Sensor *PMSX003Component::make_pm_2_5_sensor(const std::string &name) {
  return this->pm_2_5_sensor_ = new PMSX003Sensor(name, PMSX003_SENSOR_TYPE_PM_2_5);
}
PMSX003Sensor *PMSX003Component::make_pm_10_0_sensor(const std::string &name) {
  return this->pm_10_0_sensor_ = new PMSX003Sensor(name, PMSX003_SENSOR_TYPE_PM_10_0);
}
PMSX003Sensor *PMSX003Component::make_temperature_sensor(const std::string &name) {
  return this->temperature_sensor_ = new PMSX003Sensor(name, PMSX003_SENSOR_TYPE_TEMPERATURE);
}
PMSX003Sensor *PMSX003Component::make_humidity_sensor(const std::string &name) {
  return this->humidity_sensor_ = new PMSX003Sensor(name, PMSX003_SENSOR_TYPE_HUMIDITY);
}
PMSX003Sensor *PMSX003Component::make_formaldehyde_sensor(const std::string &name) {
  return this->formaldehyde_sensor_ = new PMSX003Sensor(name, PMSX003_SENSOR_TYPE_FORMALDEHYDE);
}
PMSX003Component::PMSX003Component(UARTComponent *parent, PMSX003Type type) : UARTDevice(parent), type_(type) {}
void PMSX003Component::dump_config() {
  ESP_LOGCONFIG(TAG, "PMSX003:");
  LOG_SENSOR("  ", "PM1.0", this->pm_1_0_sensor_);
  LOG_SENSOR("  ", "PM2.5", this->pm_2_5_sensor_);
  LOG_SENSOR("  ", "PM10.0", this->pm_10_0_sensor_);
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
  LOG_SENSOR("  ", "Humidity", this->humidity_sensor_);
  LOG_SENSOR("  ", "Formaldehyde", this->formaldehyde_sensor_);
}

std::string PMSX003Sensor::unit_of_measurement() {
  switch (this->type_) {
    case PMSX003_SENSOR_TYPE_PM_1_0:
    case PMSX003_SENSOR_TYPE_PM_2_5:
    case PMSX003_SENSOR_TYPE_PM_10_0:
    case PMSX003_SENSOR_TYPE_FORMALDEHYDE:
      return UNIT_MICROGRAMS_PER_CUBIC_METER;
    case PMSX003_SENSOR_TYPE_TEMPERATURE:
      return UNIT_C;
    case PMSX003_SENSOR_TYPE_HUMIDITY:
      return UNIT_PERCENT;
  }
  return "";
}
std::string PMSX003Sensor::icon() {
  switch (this->type_) {
    case PMSX003_SENSOR_TYPE_PM_1_0:
    case PMSX003_SENSOR_TYPE_PM_2_5:
    case PMSX003_SENSOR_TYPE_PM_10_0:
    case PMSX003_SENSOR_TYPE_FORMALDEHYDE:
      // Not the ideal icon, but I can't find a better one.
      return ICON_CHEMICAL_WEAPON;
    case PMSX003_SENSOR_TYPE_TEMPERATURE:
      return ICON_EMPTY;
    case PMSX003_SENSOR_TYPE_HUMIDITY:
      return ICON_WATER_PERCENT;
  }
  return "";
}
int8_t PMSX003Sensor::accuracy_decimals() {
  switch (this->type_) {
    case PMSX003_SENSOR_TYPE_PM_1_0:
    case PMSX003_SENSOR_TYPE_PM_2_5:
    case PMSX003_SENSOR_TYPE_PM_10_0:
    case PMSX003_SENSOR_TYPE_FORMALDEHYDE:
      return 0;
    case PMSX003_SENSOR_TYPE_TEMPERATURE:
    case PMSX003_SENSOR_TYPE_HUMIDITY:
      return 1;
  }

  return 0;
}
PMSX003Sensor::PMSX003Sensor(const std::string &name, PMSX003SensorType type) : Sensor(name), type_(type) {}

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PMSX003
