//
// Created by Otto Winter on 26.11.17.
//
// Based on:
//   - https://github.com/markruys/arduino-DHT

#include "esphomelib/defines.h"

#ifdef USE_DHT_SENSOR

#include "esphomelib/sensor/dht_component.h"

#include "esphomelib/log.h"
#include "esphomelib/espmath.h"
#include "esphomelib/helpers.h"
#include "esphomelib/espmath.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.dht";

DHTComponent::DHTComponent(const std::string &temperature_name, const std::string &humidity_name,
                           GPIOPin *pin, uint32_t update_interval)
    : PollingComponent(update_interval), pin_(pin),
      temperature_sensor_(new DHTTemperatureSensor(temperature_name, this)),
      humidity_sensor_(new DHTHumiditySensor(humidity_name, this)) {

}

void DHTComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up DHT...");
  this->pin_->setup();
  this->pin_->digital_write(true);
}

void DHTComponent::update() {
  float temperature, humidity;
  if (this->model_ == DHT_MODEL_AUTO_DETECT) {
    this->model_ = DHT_MODEL_DHT22;
    bool error = this->read_sensor_(&temperature, &humidity, false);
    if (error) {
      this->model_ = DHT_MODEL_DHT11;
      ESP_LOGCONFIG(TAG, "Auto-detected model DHT11. Override with the 'model:' configuration option");
    } else {
      ESP_LOGCONFIG(TAG, "Auto-detected model DHT22. Override with the 'model:' configuration option");
    }
    return;
  }

  bool error = this->read_sensor_(&temperature, &humidity, true);
  if (error) {
    ESP_LOGD(TAG, "Got Temperature=%.1f°C Humidity=%.1f%%", temperature, humidity);

    this->temperature_sensor_->push_new_value(temperature);
    this->humidity_sensor_->push_new_value(humidity);
    this->status_clear_warning();
  } else {
    ESP_LOGW(TAG, "Invalid readings! Please check your wiring (pull-up resistor, pin_ number) and "
                  "consider manually specifying the DHT model using the model option. Error code: %d", error);
    this->status_set_warning();
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
bool DHTComponent::read_sensor_(float *temperature, float *humidity, bool report_errors) {
  *humidity = NAN;
  *temperature = NAN;

  disable_interrupts();
  this->pin_->pin_mode(OUTPUT);
  this->pin_->digital_write(false);

  if (this->model_ == DHT_MODEL_DHT11)
    delayMicroseconds(18000);
  else
    delayMicroseconds(800);

  this->pin_->pin_mode(INPUT_PULLUP);
  delayMicroseconds(40);

  uint8_t data[5] = {0, 0, 0, 0, 0};
  uint8_t bit = 7;
  uint8_t byte = 0;

  for (int8_t i = -1; i < 40; i++) {
    uint32_t start_time = micros();

    // Wait for rising edge
    while (!this->pin_->digital_read()) {
      if (micros() - start_time > 90) {
        enable_interrupts();
        if (report_errors) {
          ESP_LOGW(TAG, "Rising edge for bit %d failed!", i);
        }
        return false;
      }
    }

    start_time = micros();
    uint32_t end_time = start_time;

    // Wait for falling edge
    while (this->pin_->digital_read()) {
      if ((end_time = micros()) - start_time > 90) {
        enable_interrupts();
        if (report_errors) {
          ESP_LOGW(TAG, "Falling edge for bit %d failed!", i);
        }
        return false;
      }
    }

    if (i < 0)
      continue;

    if (end_time - start_time >= 40) {
      data[byte] |= 1 << bit;
    }
    if (bit == 0) {
      bit = 7;
      byte++;
    }
    else bit--;
  }
  enable_interrupts();

  ESP_LOGVV(TAG, "Data: Hum=0b" BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN ", Temp=0b" BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN ", Checksum=0b" BYTE_TO_BINARY_PATTERN,
            BYTE_TO_BINARY(data[0]), BYTE_TO_BINARY(data[1]),
            BYTE_TO_BINARY(data[2]), BYTE_TO_BINARY(data[3]),
            BYTE_TO_BINARY(data[4]));

  uint8_t checksum;
  if (this->model_ == DHT_MODEL_DHT11)
    checksum = data[0] + data[2];
  else
    checksum = data[0] + data[1] + data[2] + data[3];

  if (checksum != data[4]) {
    if (report_errors) {
      ESP_LOGE(TAG, "Checksum invalid: %u!=%u", checksum, data[4]);
    }
    return false;
  }

  if (this->model_ == DHT_MODEL_DHT11) {
    *humidity = data[0];
    *temperature = data[2];
  } else {
    uint16_t raw_humidity = (uint16_t(data[0] & 0xFF) << 8) | (data[1] & 0xFF);
    uint16_t raw_temperature = (uint16_t(data[2] & 0xFF) << 8) | (data[3] & 0xFF);
    *humidity = raw_humidity * 0.1f;

    if ((raw_temperature & 0x8000) != 0)
      raw_temperature = ~(raw_temperature & 0x7FFF);

    *temperature = int16_t(raw_temperature) * 0.1f;
  }

  if (*temperature == 0.0f && *humidity == 1.0f) {
    *temperature = NAN;
    *humidity = NAN;
  }

  return true;
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_DHT_SENSOR
