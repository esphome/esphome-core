#ifndef ESPHOME_SENSOR_PPD42X_SENSOR_H
#define ESPHOME_SENSOR_PPD42X_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_PPD42X_SENSOR

#include "esphome/sensor/sensor.h"
#include "esphome/esphal.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class Ppd42xSensorComponent : public PollingSensorComponent {
 public:
  /** Construct the PPD42X sensor with the specified 2.5ppm pin and 10.0ppm pin.
   *
   * @param pm_2_5_pin The pm_2_5 pin where pulses are sent to.
   * @param pm_10_0_pin The pm_10_0 pin where the pm_10_0 is listened for.
   * @param update_interval The interval in ms the sensor should check for new values.
   */
  Ppd42xSensorComponent(const std::string &name, GPIOPin *pm_10_0_pin, GPIOPin *pm_02_5_pin,
                        uint32_t update_interval = 60000);

  /// Set the timeout for waiting for the pm_10_0 in µs.
  void set_timeout_us(uint32_t timeout_us);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Set up pins and register interval.
  void setup() override;
  void dump_config() override;

  void update() override;

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

  float get_setup_priority() const override;

 
 protected:
  /// Helper function to convert the specified pm_10_0 duration in µg/m³ to meters.
  static float us_to_pm(uint32_t sample_length, uint32_t time_pm);
  /// Helper function to convert the specified distance in meters to the pm_10_0 duration in µs.

  GPIOPin *pm_02_5_pin_;
  GPIOPin *pm_10_0_pin_;
  uint32_t timeout_us_{30000};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PPD42X_SENSOR

#endif  // ESPHOME_SENSOR_PPD42X_SENSOR_H
