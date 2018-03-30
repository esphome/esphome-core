//
//  bmp085_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 27.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/sensor/bmp085_component.h"

#include "esphomelib/log.h"
#include "esphomelib/application.h"

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor::bmp085";

BMP085TemperatureSensor::BMP085TemperatureSensor(BMP085Component *parent)
  : parent_(parent) { }
std::string BMP085TemperatureSensor::unit_of_measurement() {
  return "°C";
}
std::string BMP085TemperatureSensor::icon() {
  return "";
}
uint32_t BMP085TemperatureSensor::update_interval() {
  return this->parent_->get_update_interval();
}
int8_t BMP085TemperatureSensor::accuracy_decimals() {
  return 1;
}

BMP085PressureSensor::BMP085PressureSensor(BMP085Component *parent)
  : parent_(parent) { }
std::string BMP085PressureSensor::unit_of_measurement() {
  return "hPa";
}
std::string BMP085PressureSensor::icon() {
  return "mdi:gauge";
}
uint32_t BMP085PressureSensor::update_interval() {
  return this->parent_->get_update_interval();
}
int8_t BMP085PressureSensor::accuracy_decimals() {
  return 1;
}

uint8_t BMP085Component::get_address() const {
  return this->address_;
}
void BMP085Component::set_address(uint8_t address) {
  this->address_ = address;
}
BMP085 &BMP085Component::get_bmp() {
  return this->bmp_;
}
void BMP085Component::update() {
  if (this->measurement_mode_ != IDLE) {
    ESP_LOGW(TAG, "BMP085 is not ready with measurements yet, is your update interval too low?");
    return;
  }
  this->measurement_mode_ = TEMPERATURE;
  this->bmp_.setControl(BMP085_MODE_TEMPERATURE);

  this->set_timeout("temperature", 5, std::bind(&BMP085Component::read_temperature_, this));
}
void BMP085Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up BMP085...");
  App.assert_i2c_initialized();
  ESP_LOGCONFIG(TAG, "    Address: 0x%02x", this->address_);
  this->bmp_ = BMP085(this->address_);
  if (!this->bmp_.testConnection()) {
    ESP_LOGE(TAG, "Connection to BMP085 with address 0x%02x failed.", this->address_);
    return;
  }
  this->bmp_.initialize();
}
BMP085Component::BMP085Component(uint32_t update_interval)
  : PollingComponent(update_interval), address_(BMP085_DEFAULT_ADDRESS),
    temperature_(new BMP085TemperatureSensor(this)), pressure_(new BMP085PressureSensor(this)) {

}

void BMP085Component::read_temperature_() {
  if (this->measurement_mode_ != TEMPERATURE) {
    ESP_LOGW(TAG, "BMP085 is not ready with measurements yet, is your update interval too low?");
    return;
  }

  float temperature = this->bmp_.getTemperatureC();
  ESP_LOGD(TAG, "Got Temperature=%.1f°C", temperature);

  if (!isnan(temperature))
    this->pressure_->push_new_value(temperature);
  else
    ESP_LOGW(TAG, "Invalid Temperature: %f°C", temperature);

  this->measurement_mode_ = PRESSURE;
  this->bmp_.setControl(BMP085_MODE_PRESSURE_3);

  this->set_timeout("pressure", 26, std::bind(&BMP085Component::read_pressure_, this));
}
void BMP085Component::read_pressure_() {
  if (this->measurement_mode_ != PRESSURE) {
    ESP_LOGW(TAG, "BMP085 is not ready with measurements yet, is your update interval too low?");
    return;
  }

  float pressure = this->bmp_.getPressure() / 100.0f;
  ESP_LOGD(TAG, "Got Pressure=%.1fhPa", pressure);

  if (!isnan(pressure))
    this->pressure_->push_new_value(pressure);
  else
    ESP_LOGW(TAG, "Invalid Pressure: %fhPa", pressure);

  this->measurement_mode_ = IDLE;
  // TODO: consider setting temperature measurement mode here already
  // might only trigger *one* measurement, and sit idle afterwards though.
}
BMP085TemperatureSensor *BMP085Component::get_temperature_sensor() const {
  return this->temperature_;
}
BMP085PressureSensor *BMP085Component::get_pressure_sensor() const {
  return this->pressure_;
}

} // namespace sensor

} // namespace esphomelib
