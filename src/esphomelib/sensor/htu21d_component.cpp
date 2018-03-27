//
//  htu21d_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 27.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/sensor/htu21d_component.h"

#include "esphomelib/log.h"
#include "esphomelib/application.h"

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor::htu21d";

HTU21DTemperatureSensor::HTU21DTemperatureSensor(HTU21DComponent *parent)
    : parent_(parent) { }
std::string HTU21DTemperatureSensor::unit_of_measurement() {
  return UNIT_OF_MEASUREMENT_CELSIUS;
}
std::string HTU21DTemperatureSensor::icon() {
  return ICON_TEMPERATURE;
}
uint32_t HTU21DTemperatureSensor::update_interval() {
  return this->parent_->get_update_interval();
}
int8_t HTU21DTemperatureSensor::accuracy_decimals() {
  return 0;
}

HTU21DHumiditySensor::HTU21DHumiditySensor(HTU21DComponent *parent)
    : parent_(parent) { }
std::string HTU21DHumiditySensor::unit_of_measurement() {
  return UNIT_OF_MEASUREMENT_PERCENT;
}
std::string HTU21DHumiditySensor::icon() {
  return ICON_HUMIDITY;
}
uint32_t HTU21DHumiditySensor::update_interval() {
  return this->parent_->get_update_interval();
}
int8_t HTU21DHumiditySensor::accuracy_decimals() {
  return 0;
}

HTU21DComponent::HTU21DComponent(uint32_t update_interval)
    : PollingComponent(update_interval) { }
void HTU21DComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up HTU21D...");
  App.assert_i2c_initialized();
  // first initialize, then test connection for this device.
  this->htu21d_.initialize();

  if (!this->htu21d_.testConnection()) {
    ESP_LOGE(TAG, "Connection to HTU21D failed.");
    return;
  }
}
void HTU21DComponent::update() {
  float temperature = this->htu21d_.getTemperature();
  float humidity = this->htu21d_.getHumidity();

  ESP_LOGD(TAG, "Got Temperature=%.1f°C Humidity=%.1f%%", temperature, humidity);

  if (!isnan(temperature))
    this->temperature_->push_new_value(temperature);
  else
    ESP_LOGW(TAG, "Invalid Temperature: %f!C", temperature);

  if (!isnan(humidity))
    this->humidity_->push_new_value(humidity);
  else
    ESP_LOGW(TAG, "Invalid Humidity: %f%%", humidity);
}
HTU21D &HTU21DComponent::get_htu21d() {
  return this->htu21d_;
}
HTU21DTemperatureSensor *HTU21DComponent::get_temperature_sensor() const {
  return this->temperature_;
}
HTU21DHumiditySensor *HTU21DComponent::get_humidity_sensor() const {
  return this->humidity_;
}

} // namespace sensor

} // namespace esphomelib
