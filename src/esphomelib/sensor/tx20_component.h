#include "esphomelib/defines.h"

#ifdef USE_TX20

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/text_sensor/text_sensor.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

extern const char UNIT_KMH[];
extern const char ICON_WIND_SPEED[];
extern const char ICON_WIND_DIRECTION[];
extern const char EMPTY[];

using TX20WindSpeedSensor =
    sensor::EmptySensor<1, ICON_WIND_SPEED, UNIT_KMH>;
using TX20WindDirectionSensor =
    sensor::EmptySensor<1, ICON_WIND_DIRECTION, EMPTY>;
using TX20WindDirectionTextSensor = text_sensor::TextSensor;

/// This class implements support for the TX20 WindSpeed+WindDirection sensor.
class TX20Component : public Component {
public:
  TX20Component(const std::string &wind_speed_name,
                const std::string &wind_direction_name,
                const std::string &wind_direction_text_name, GPIOPin *pin);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  TX20WindSpeedSensor *get_wind_speed_sensor() const;
  TX20WindDirectionSensor *get_wind_direction_sensor() const;
  TX20WindDirectionTextSensor *get_wind_direction_text_sensor() const;

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void loop() override;

protected:
  static void start_read_();
  void start_read_internal_();

  TX20WindSpeedSensor *wind_speed_sensor_;
  TX20WindDirectionSensor *wind_direction_sensor_;
  TX20WindDirectionTextSensor *wind_direction_text_sensor_;
  GPIOPin *pin_;
};
/// Global storage for having multiple tx20
extern TX20Component *global_tx20_;

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif // USE_TX20
