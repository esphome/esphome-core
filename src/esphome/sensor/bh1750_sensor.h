#ifndef ESPHOME_SENSOR_BH1750_SENSOR_H
#define ESPHOME_SENSOR_BH1750_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_BH1750

#include "esphome/sensor/sensor.h"
#include "esphome/i2c_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

/// Enum listing all resolutions that can be used with the BH1750
enum BH1750Resolution {
  BH1750_RESOLUTION_4P0_LX = 0b00100011,  // one-time low resolution mode
  BH1750_RESOLUTION_1P0_LX = 0b00100000,  // one-time high resolution mode 1
  BH1750_RESOLUTION_0P5_LX = 0b00100001,  // one-time high resolution mode 2
};

/// This class implements support for the i2c-based BH1750 ambient light sensor.
class BH1750Sensor : public PollingSensorComponent, public I2CDevice {
 public:
  BH1750Sensor(I2CComponent *parent, const std::string &name, uint8_t address = 0x23, uint32_t update_interval = 60000);

  /** Set the resolution of this sensor.
   *
   * Possible values are:
   *
   *  - `BH1750_RESOLUTION_4P0_LX`
   *  - `BH1750_RESOLUTION_1P0_LX`
   *  - `BH1750_RESOLUTION_0P5_LX` (default)
   *
   * @param resolution The new resolution of the sensor.
   */
  void set_resolution(BH1750Resolution resolution);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void setup() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override;
  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

 protected:
  void read_data_();

  BH1750Resolution resolution_{BH1750_RESOLUTION_0P5_LX};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_BH1750

#endif  // ESPHOME_SENSOR_BH1750_SENSOR_H
