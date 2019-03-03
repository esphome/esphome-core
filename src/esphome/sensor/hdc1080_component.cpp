// Based on:
//   - https://github.com/closedcube/ClosedCube_HDC1080_Arduino
//   - http://www.ti.com/lit/ds/symlink/hdc1080.pdf

#include "esphome/defines.h"

#ifdef USE_HDC1080_SENSOR

#include "esphome/sensor/hdc1080_component.h"

#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.hdc1080";
static const uint8_t HDC1080_ADDRESS = 0x40;  // 0b1000000 from datasheet
static const uint8_t HDC1080_CMD_CONFIGURATION = 0x02;
static const uint8_t HDC1080_CMD_TEMPERATURE = 0x00;
static const uint8_t HDC1080_CMD_HUMIDITY = 0x01;

HDC1080Component::HDC1080Component(I2CComponent *parent, const std::string &temperature_name,
                                   const std::string &humidity_name, uint32_t update_interval)
    : PollingComponent(update_interval),
      I2CDevice(parent, HDC1080_ADDRESS),
      temperature_(new HDC1080TemperatureSensor(temperature_name, this)),
      humidity_(new HDC1080HumiditySensor(humidity_name, this)) {}
void HDC1080Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up HDC1080...");

  const uint8_t data[2] = {
      0b00000000,  // resolution 14bit for both humidity and temperature
      0b00000000   // reserved
  };

  if (this->write_bytes(HDC1080_CMD_CONFIGURATION, data, 2)) {
    this->mark_failed();
    return;
  }
}
void HDC1080Component::dump_config() {
  ESP_LOGCONFIG(TAG, "HDC1080:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with HDC1080 failed!");
  }
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Temperature", this->temperature_);
  LOG_SENSOR("  ", "Humidity", this->humidity_);
}
void HDC1080Component::update() {
  uint16_t raw_temp;
  if (!this->read_byte_16(HDC1080_CMD_TEMPERATURE, &raw_temp, 9)) {
    this->status_set_warning();
    return;
  }
  float temp = raw_temp * 0.0025177f - 40.0f;  // raw * 2^-16 * 165 - 40
  this->temperature_->publish_state(temp);

  uint16_t raw_humidity;
  if (!this->read_byte_16(HDC1080_CMD_HUMIDITY, &raw_humidity, 9)) {
    this->status_set_warning();
    return;
  }
  float humidity = raw_humidity * 0.001525879f;  // raw * 2^-16 * 100
  this->humidity_->publish_state(humidity);

  ESP_LOGD(TAG, "Got temperature=%.1f°C humidity=%.1f%%", temp, humidity);
  this->status_clear_warning();
}
HDC1080TemperatureSensor *HDC1080Component::get_temperature_sensor() const { return this->temperature_; }
HDC1080HumiditySensor *HDC1080Component::get_humidity_sensor() const { return this->humidity_; }
float HDC1080Component::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_HDC1080_SENSOR
