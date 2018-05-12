//
// Created by Otto Winter on 26.11.17.
//
// Based on:
//   - https://github.com/markruys/arduino-DHT

#include "esphomelib/sensor/dht_component.h"

#include "esphomelib/log.h"
#include "esphomelib/espmath.h"
#include "esphomelib/helpers.h"
#include "esphomelib/espmath.h"

#ifdef USE_DHT_SENSOR

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor.dht";

static const uint8_t DHT_ERROR_TIMEOUT = 1;
static const uint8_t DHT_ERROR_CHECKSUM = 2;

DHTComponent::DHTComponent(const std::string &temperature_name, const std::string &humidity_name,
                           GPIOPin *pin, uint32_t update_interval)
    : PollingComponent(update_interval), pin_(pin),
      temperature_sensor_(new DHTTemperatureSensor(temperature_name, this)),
      humidity_sensor_(new DHTHumiditySensor(humidity_name, this)) {

}

void DHTComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up DHT...");

  if (this->model_ == DHT_MODEL_AUTO_DETECT) {
    this->model_ = DHT_MODEL_DHT22;
    float temp1, temp2;
    if (this->read_sensor_safe_(&temp1, &temp2) == DHT_ERROR_TIMEOUT)
      this->model_ = DHT_MODEL_DHT11;
  }

  ESP_LOGCONFIG(TAG, "    Model: %u", this->model_);
}

void DHTComponent::update() {
  float temperature, humidity;
  this->read_sensor_safe_(&temperature, &humidity);

  if (!isnan(temperature) && !isnan(humidity)) {
    ESP_LOGD(TAG, "Got Temperature=%.1f°C Humidity=%.1f%%", temperature, humidity);

    this->temperature_sensor_->push_new_value(temperature);
    this->humidity_sensor_->push_new_value(humidity);
  } else {
    ESP_LOGW(TAG, "Invalid readings!");
  }
}

float DHTComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
void DHTComponent::set_dht_model(DHTModel model) {
  this->model_ = model;
}
DHTTemperatureSensor *DHTComponent::get_temperature_sensor() const {
  return this->temperature_sensor_;
}
DHTHumiditySensor *DHTComponent::get_humidity_sensor() const {
  return this->humidity_sensor_;
}
uint8_t DHTComponent::read_sensor_(float *temperature, float *humidity) {
  *humidity = NAN;
  *temperature = NAN;

  this->pin_->digital_write(false);
  this->pin_->pin_mode(OUTPUT);

  if (this->model_ == DHT_MODEL_DHT11)
    delay(18);
  else
    delayMicroseconds(800);

  this->pin_->pin_mode(INPUT);
  this->pin_->digital_write(true);

  uint16_t raw_humidity = 0;
  uint16_t raw_temperature = 0;
  uint16_t data = 0;

  for (int8_t i = -3; i < 80; i++) {
    uint8_t age;
    uint32_t start_time = micros();

    do {
      age = micros() - start_time;
      if (age > 90)
        return DHT_ERROR_TIMEOUT;
    } while (this->pin_->digital_read() == ((i & 1) == 1));

    if (i >= 0 && (i & 1) == 1) {
      data <<= 1;

      if (age > 30)
        data |= 1;
    }

    switch (i) {
      case 31:
        raw_humidity = data;
        break;
      case 63:
        raw_temperature = data;
        data = 0;
        break;
      default:break;
    }
  }

  uint8_t checksum = (raw_humidity & 0xFF) + (raw_humidity >> 8)
      + (raw_temperature & 0xFF) + (raw_temperature >> 8);
  if (checksum != data)
    return DHT_ERROR_CHECKSUM;

  if (this->model_ == DHT_MODEL_DHT11) {
    *humidity = raw_humidity >> 8;
    *temperature = raw_temperature >> 8;
  } else {
    *humidity = raw_humidity * 0.1f;

    if ((raw_temperature & 0x8000) != 0)
      raw_temperature = ~(raw_temperature & 0x7FFF);

    *temperature = int16_t(raw_temperature) * 0.1f;
  }

  if (*temperature == 0.0f && *humidity == 1.0f) {
    *temperature = NAN;
    *humidity = NAN;
  }

  return 0;
}
uint8_t DHTComponent::read_sensor_safe_(float *temperature, float *humidity) {
  disable_interrupts();
  uint8_t ret = this->read_sensor_(temperature, humidity);
  enable_interrupts();
  return ret;
}

} // namespace sensor

} // namespace esphomelib

#endif //USE_DHT_SENSOR
