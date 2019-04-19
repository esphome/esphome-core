#ifndef ESPHOME_SENSOR_CCS811_COMPONENT_H
#define ESPHOME_SENSOR_CCS811_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_CCS811_SENSOR

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"
#include "esphome/i2c_component.h"
#include "esphome/switch_/switch.h"
#include "esphome/text_sensor/text_sensor.h"
#include "SparkFunCCS811.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

using CCS811eCO2Sensor = sensor::EmptyPollingParentSensor<0, ICON_GAS_CYLINDER, UNIT_PPM>;
using CCS811TVOCSensor = sensor::EmptyPollingParentSensor<0, ICON_RADIATOR, UNIT_PPB>;
constexpr uint8_t SENSOR_ADDR = 0x5A;

enum class CCS811Status {
  INITIALIZING,
  WARMING_UP,
  SEARCHING_FOR_BASELINE,
  WAINTING_FOR_BASELINE_SETTING,
  MEASURING
};

class CCS811Component : public switch_::Switch, public PollingComponent, public I2CDevice {
 public:
  struct InitStruct {
    const std::string &eco2_name;
    const std::string &tvoc_name;
    const std::string &switch_name;
    const std::string &status_name;
    const std::string &baseline_topic;
  };

  /// Construct the CCS811Component using the provided address and update interval.
  CCS811Component(I2CComponent *parent, InitStruct names, uint32_t update_interval, uint8_t address);

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
  CCS811eCO2Sensor *get_eco2_sensor();
  /// Get the internal pressure sensor used to expose the pressure as a sensor object.
  CCS811TVOCSensor *get_tvoc_sensor();
  text_sensor::TextSensor *get_status_label();

 protected:
  volatile CCS811Status status;
  CCS811 sensor_handle;
  CCS811eCO2Sensor eco2_;
  CCS811TVOCSensor tvoc_;
  text_sensor::TextSensor status_label;
  const std::string baseline_mqtt_topic;
  void setStatus(CCS811Status status);
  void publishBaseline();
};

} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_CCS811_SENSOR

#endif //ESPHOME_SENSOR_CCS811_COMPONENT_H
