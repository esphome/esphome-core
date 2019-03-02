#ifndef ESPHOME_SENSOR_MS5611_H
#define ESPHOME_SENSOR_MS5611_H

#include "esphome/defines.h"

#ifdef USE_MS5611

#include "esphome/sensor/sensor.h"
#include "esphome/i2c_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

using MS5611TemperatureSensor = sensor::EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C>;
using MS5611PressureSensor = sensor::EmptyPollingParentSensor<1, ICON_GAUGE, UNIT_HPA>;

class MS5611Component : public PollingComponent, public I2CDevice {
 public:
  MS5611Component(I2CComponent *parent, const std::string &temperature_name, const std::string &pressure_name,
                  uint32_t update_interval = 60000);

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;

  MS5611TemperatureSensor *get_temperature_sensor() const;
  MS5611PressureSensor *get_pressure_sensor() const;

 protected:
  void read_temperature_();
  void read_pressure_(uint32_t raw_temperature);
  void calculate_values_(uint32_t raw_temperature, uint32_t raw_pressure);

  MS5611TemperatureSensor *temperature_sensor_;
  MS5611PressureSensor *pressure_sensor_;
  uint16_t prom_[6];
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MS5611

#endif  // ESPHOME_SENSOR_MS5611_H
