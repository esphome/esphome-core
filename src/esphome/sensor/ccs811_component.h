#ifndef ESPHOME_SENSOR_CCS811_COMPONENT_H
#define ESPHOME_SENSOR_CCS811_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_CCS811_SENSOR

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"
#include "esphome/i2c_component.h"
#include "esphome/switch_/switch.h"
#include "esphome/time/sntp_component.h"
#include "SparkFunCCS811.h"

ESPHOME_NAMESPACE_BEGIN

using namespace switch_;
namespace sensor {

using CCS811eCO2Sensor = sensor::EmptyPollingParentSensor<0, ICON_GAS_CYLINDER, UNIT_PPM>;
using CCS811TVOCSensor = sensor::EmptyPollingParentSensor<0, ICON_RADIATOR, UNIT_PPB>;
constexpr uint8_t SENSOR_ADDR = 0x5A;

enum class CCS811State {
  INITIALIZING,
  WARMING_UP,
  SEARCHING_FOR_BASELINE,
  WAINTING_FOR_BASELINE_SETTING,
  MEASURING
};

class CCS811Component : public Switch, public PollingComponent, public I2CDevice {
 public:
  /// Construct the CCS811Component using the provided address and update interval.
  CCS811Component(I2CComponent *parent,
                  const std::string &eco2_name, const std::string &tvoc_name,
                  uint32_t update_interval = 30000);

  void write_state(bool state) override;
  
  /// Setup the sensor and test for a connection.
  void setup() override;
  /// Schedule temperature+pressure readings.
  void update() override;

  void dump_config() override;
  float get_setup_priority() const override;

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Get the internal temperature sensor used to expose the temperature as a sensor object.
  CCS811eCO2Sensor *get_eco2_sensor() const;
  /// Get the internal pressure sensor used to expose the pressure as a sensor object.
  CCS811TVOCSensor *get_tvoc_sensor() const;

 protected:
  CCS811eCO2Sensor *eco2_{nullptr};
  CCS811TVOCSensor *tvoc_{nullptr};
  CCS811 sensor = CCS811(SENSOR_ADDR);
  void setState(CCS811State state);
  void publishBaseline();

  volatile CCS811State state;
};

} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_CCS811_SENSOR

#endif //ESPHOME_SENSOR_CCS811_COMPONENT_H
