//
//  htu21d_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 27.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/sensor/htu21d_component.h"

#include "esphomelib/log.h"

#ifdef USE_HTU21D_SENSOR

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor.htu21d";

HTU21DComponent::HTU21DComponent(const std::string &temperature_name, const std::string &humidity_name,
                                 uint32_t update_interval)
    : PollingComponent(update_interval), temperature_(new HTU21DTemperatureSensor(temperature_name, this)),
      humidity_(new HTU21DHumiditySensor(humidity_name, this)) {

}
void HTU21DComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up HTU21D...");
  this->htu21d_ = HTU21D();

  if (!this->htu21d_.testConnection()) {
    ESP_LOGE(TAG, "Connection to HTU21D failed.");
    this->mark_failed();
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
    ESP_LOGW(TAG, "Invalid Temperature: %f°C", temperature);

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

#endif //USE_HTU21D_SENSOR
