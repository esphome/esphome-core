//
//  bh1750_sensor.cpp
//  esphomelib
//
//  Created by Otto Winter on 10.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//
// Based on:
//  - http://www.mouser.com/ds/2/348/bh1750fvi-e-186247.pdf
//  - https://github.com/claws/BH1750
//  - https://github.com/mysensors/MySensorsArduinoExamples/tree/master/libraries/BH1750

#include "esphomelib/sensor/bh1750_sensor.h"
#include "esphomelib/log.h"

#ifdef USE_BH1750

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.bh1750";

static const uint8_t BH1750_COMMAND_POWER_ON = 0b00000001;

BH1750Sensor::BH1750Sensor(I2CComponent *parent, const std::string &name,
                           uint8_t address, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval), I2CDevice(parent, address) {}

void BH1750Sensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up BS1750...");
  ESP_LOGCONFIG(TAG, "    Address: 0x%02X", this->address_);
  if (!this->write_bytes(BH1750_COMMAND_POWER_ON, nullptr, 0)) {
    ESP_LOGE(TAG, "Communication with BH1750 failed!");
    this->mark_failed();
    return;
  }

  if_config {
    const char *resolution_s;
    switch (this->resolution_) {
      case BH1750_RESOLUTION_0P5_LX: resolution_s = "0.5"; break;
      case BH1750_RESOLUTION_1P0_LX: resolution_s = "1"; break;
      case BH1750_RESOLUTION_4P0_LX: resolution_s = "4"; break;
      default: resolution_s = "Unknown"; break;
    }
    ESP_LOGCONFIG(TAG, "    Resolution: %s", resolution_s);
  }
}

void BH1750Sensor::update() {
  if (!this->write_bytes(this->resolution_, nullptr, 0))
    return;

  uint32_t wait = 0;
  // use max conversion times
  switch (this->resolution_) {
    case BH1750_RESOLUTION_0P5_LX:
    case BH1750_RESOLUTION_1P0_LX:
      wait = 180;
      break;
    case BH1750_RESOLUTION_4P0_LX:
      wait = 24;
      break;
  }

  this->set_timeout("illuminance", wait, [this]() {
    this->read_data_();
  });
}
std::string BH1750Sensor::unit_of_measurement() {
  return UNIT_LX;
}
std::string BH1750Sensor::icon() {
  return ICON_BRIGHTNESS_5;
}
int8_t BH1750Sensor::accuracy_decimals() {
  return 1;
}
float BH1750Sensor::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
void BH1750Sensor::read_data_() {
  uint16_t raw_value;
  if (!this->parent_->receive_16_(this->address_, &raw_value, 1))
    return;

  float lx = float(raw_value) / 1.2f;
  ESP_LOGD(TAG, "'%s': Got illuminance=%.1flx", this->get_name().c_str(), lx);
  this->push_new_value(lx);
}
void BH1750Sensor::set_resolution(BH1750Resolution resolution) {
  this->resolution_ = resolution;
}

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_BH1750
