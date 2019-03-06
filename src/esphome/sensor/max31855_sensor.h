#ifndef ESPHOME_SENSOR_MAX31855_H
#define ESPHOME_SENSOR_MAX31855_H

#include "esphome/defines.h"

#ifdef USE_MAX31855_SENSOR

#include "esphome/sensor/sensor.h"
#include "esphome/spi_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class MAX31855Sensor : public PollingSensorComponent, public SPIDevice {
 public:
  MAX31855Sensor(const std::string &name, SPIComponent *parent, GPIOPin *cs, uint32_t update_interval = 60000);

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void update() override;

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

 protected:
  bool is_device_msb_first() override;

  void read_data_();
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MAX31855_SENSOR

#endif  // ESPHOME_SENSOR_MAX31855_H
