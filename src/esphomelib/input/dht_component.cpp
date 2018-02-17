//
// Created by Otto Winter on 26.11.17.
//

#include <esphomelib/log.h>
#include <esphomelib/espmath.h>
#include <esphomelib/helpers.h>
#include "dht_component.h"
#include <esphomelib/espmath.h>

namespace esphomelib {

namespace input {

using namespace esphomelib::sensor;

static const char *TAG = "sensor::dht";

DHTComponent::DHTComponent(uint8_t pin, uint32_t check_interval)
    : temperature_sensor_(new TemperatureSensor()),
      humidity_sensor_(new HumiditySensor()) {
  this->set_pin(pin);
  this->set_check_interval(check_interval);
}

void DHTComponent::setup() {
  this->dht_.setup(this->pin_, this->model_);

  ESP_LOGD(TAG, "DHT status %d", this->dht_.getStatus());

  this->set_interval("check", this->check_interval_, [&]() {
    auto temp_hum = run_without_interrupts<std::pair<float, float>>([this] {
      return std::make_pair(this->dht_.getTemperature(), this->dht_.getHumidity());
    });
    float temperature = temp_hum.first;
    float humidity = temp_hum.second;

    ESP_LOGV(TAG, "Got Temperature=%.1f°C Humidity=%.1f%%", temperature, humidity);

    if (!isnan(temperature))
      this->temperature_sensor_->push_new_value(temperature, this->dht_.getNumberOfDecimalsTemperature());
    else
      ESP_LOGE(TAG, "Invalid Temperature: %f", temperature);

    if (!isnan(humidity))
      this->humidity_sensor_->push_new_value(humidity, this->dht_.getNumberOfDecimalsHumidity());
    else
      ESP_LOGE(TAG, "Invalid Humidity: %f", humidity);
  });
}

float DHTComponent::get_setup_priority() const {
  return setup_priority::HARDWARE;
}

TemperatureSensor *DHTComponent::get_temperature_sensor() const {
  return this->temperature_sensor_;
}

HumiditySensor *DHTComponent::get_humidity_sensor() const {
  return this->humidity_sensor_;
}
uint8_t DHTComponent::get_pin() const {
  return this->pin_;
}
void DHTComponent::set_pin(uint8_t pin) {
  assert_is_pin(pin);
  assert_construction_state(this);
  this->pin_ = pin;
}
uint32_t DHTComponent::get_check_interval() const {
  return this->check_interval_;
}
void DHTComponent::set_check_interval(uint32_t check_interval) {
  assert_positive(check_interval);
  assert_construction_state(this);
  this->check_interval_ = check_interval;
}
void DHTComponent::set_dht_model(DHT::DHT_MODEL_t model) {
  assert_construction_state(this);
  this->model_ = model;
}
const DHT &DHTComponent::get_dht() const {
  return this->dht_;
}

} // namespace input

} // namespace esphomelib
