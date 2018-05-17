//
//  dht12_component.h
//  esphomelib
//
//  Created by Otto Winter on 17.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_SENSOR_DHT12_COMPONENT_H
#define ESPHOMELIB_SENSOR_DHT12_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/i2c_component.h"
#include "esphomelib/defines.h"

#ifdef USE_DHT12_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

using DHT12TemperatureSensor = EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C>;
using DHT12HumiditySensor = EmptyPollingParentSensor<0, ICON_WATER_PERCENT, UNIT_PERCENT>;

class DHT12Component : public PollingComponent, public I2CDevice {
 public:
  DHT12Component(I2CComponent *parent,
                 const std::string &temperature_name, const std::string &humidity_name,
                 uint32_t update_interval = 15000);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void setup() override;
  float get_setup_priority() const override;
  void update() override;
  DHT12TemperatureSensor *get_temperature_sensor() const;
  DHT12HumiditySensor *get_humidity_sensor() const;

 protected:
  bool read_data_(uint8_t *data);

  DHT12TemperatureSensor *temperature_sensor_;
  DHT12HumiditySensor *humidity_sensor_;
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_DHT12_SENSOR

#endif //ESPHOMELIB_SENSOR_DHT12_COMPONENT_H
