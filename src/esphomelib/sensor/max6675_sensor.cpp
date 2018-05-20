//
//  max6675_sensor.cpp
//  esphomelib
//
//  Created by Otto Winter on 19.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//
// Implementation based on https://github.com/adafruit/MAX6675-library/blob/master/max6675.cpp

#include "esphomelib/sensor/max6675_sensor.h"
#include "esphomelib/log.h"

#ifdef USE_MAX6675_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.max6675";

MAX6675Sensor::MAX6675Sensor(const std::string &name, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval) {

}

MAX6675Sensor::MAX6675Sensor(const std::string &name,
                             GPIOPin *cs,
                             GPIOPin *clock,
                             GPIOPin *miso,
                             uint32_t update_interval)
    : PollingSensorComponent(name, update_interval), cs_(cs), clock_(clock), miso_(miso) {

}
void MAX6675Sensor::update() {
  this->cs_->digital_write(false);
  delay(1);
  // conversion initiated by positiveedge
  this->cs_->digital_write(true);

  // Conversion time typ: 170ms, max: 220ms
  this->set_timeout("value", 220, [this]() {
    this->read_data_();
  });
}

void MAX6675Sensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MAX6675Sensor '%s'...", this->get_name().c_str());
  this->cs_->setup();
  this->cs_->digital_write(true);

  this->clock_->setup();
  this->clock_->digital_write(true);

  this->miso_->setup();
}
float MAX6675Sensor::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
uint8_t MAX6675Sensor::read_spi_() {
  uint8_t res = 0;
  for (int i = 7; i >= 0; i--) {
    this->clock_->digital_write(false);
    delay(1);
    if (this->miso_->digital_read())
      res |= (1 << i);
    this->clock_->digital_write(true);
    delay(1);
  }

  return res;
}
std::string MAX6675Sensor::unit_of_measurement() {
  return UNIT_C;
}
std::string MAX6675Sensor::icon() {
  return ICON_EMPTY;
}
int8_t MAX6675Sensor::accuracy_decimals() {
  return 1;
}
void MAX6675Sensor::read_data_() {
  this->cs_->digital_write(false);
  delay(1);

  uint16_t val = this->read_spi_();
  val |= uint16_t(this->read_spi_()) << 8;

  this->cs_->digital_write(true);

  if ((val & 0x04) != 0) {
    // Thermocouple open
    ESP_LOGW(TAG, "Got invalid value from MAX6675Sensor (0x%04X)", val);
    return;
  }

  float temperature = float(val >> 3) / 4.0f;
  ESP_LOGD(TAG, "'%s': Got temperature=%.1°C", temperature);
  this->push_new_value(temperature);
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_MAX6675_SENSOR
