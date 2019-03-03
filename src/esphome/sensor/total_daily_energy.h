#ifndef ESPHOME_SENSOR_TOTAL_DAILY_ENERGY_H
#define ESPHOME_SENSOR_TOTAL_DAILY_ENERGY_H

#include "esphome/defines.h"

#ifdef USE_TOTAL_DAILY_ENERGY_SENSOR

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"
#include "esphome/time/rtc_component.h"
#include "esphome/esppreferences.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class TotalDailyEnergy : public Sensor, public Component {
 public:
  TotalDailyEnergy(const std::string &name, time::RealTimeClockComponent *time, Sensor *parent);
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  uint32_t update_interval() override;
  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;
  void loop() override;

  void publish_state_and_save(float state);

 protected:
  void process_new_state_(float state);

  ESPPreferenceObject pref_;
  time::RealTimeClockComponent *time_;
  Sensor *parent_;
  uint16_t last_day_of_year_{};
  uint32_t last_update_{0};
  float total_energy_{0.0f};
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TOTAL_DAILY_ENERGY_SENSOR

#endif  // ESPHOME_SENSOR_TOTAL_DAILY_ENERGY_H
