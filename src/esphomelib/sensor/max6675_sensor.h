#ifndef ESPHOMELIB_SENSOR_MAX6675_H
#define ESPHOMELIB_SENSOR_MAX6675_H

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/spi_component.h"
#include "esphomelib/defines.h"

#ifdef USE_MAX6675_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

class MAX6675Sensor : public PollingSensorComponent, public SPIDevice {
 public:
  MAX6675Sensor(const std::string &name, SPIComponent *parent, GPIOPin *cs, uint32_t update_interval = 15000);

  void setup() override;
  float get_setup_priority() const override;

  void update() override;

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

 protected:
  bool msb_first() override;

  void read_data_();
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_MAX6675_SENSOR

#endif //ESPHOMELIB_SENSOR_MAX6675_H
