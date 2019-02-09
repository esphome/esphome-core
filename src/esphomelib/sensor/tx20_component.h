#include "esphomelib/defines.h"

#ifdef USE_TX20

#include "esphomelib/sensor/sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

using TX20WindSpeedSensor = sensor::EmptySensor<1, ICON_WIND_SPEED, UNIT_KMH>;
using TX20WindDirectionDegreesSensor = sensor::EmptySensor<1, ICON_WIND_DIRECTION, EMPTY>;

/// This class implements support for the TX20 WindSpeed+WindDirection sensor.
class TX20Component : public Component {
 public:
  TX20Component(const std::string &wind_speed_name, const std::string &wind_direction_degrees_name,
                GPIOPin *pin);
  /// Get the textual representation of the wind direction ('N', 'SSE', ..).
  std::string get_wind_cardinal_direction() const;

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  TX20WindSpeedSensor *get_wind_speed_sensor() const;
  TX20WindDirectionDegreesSensor *get_wind_direction_degrees_sensor() const;

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void loop() override;

 protected:
  static void pin_change_();
  void decode_and_publish_();

  TX20WindSpeedSensor *wind_speed_sensor_;
  TX20WindDirectionDegreesSensor *wind_direction_degrees_sensor_;
  GPIOPin *pin_;
  std::string wind_cardinal_direction_;

  static uint16_t *buffer_;
  static uint32_t start_time_;
  static uint8_t buffer_index_;
  static uint32_t spent_time_;
  static bool tx20_available_;
};

}  // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif  // USE_TX20
