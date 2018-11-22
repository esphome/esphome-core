#ifndef ESPHOMELIB_SENSOR_APDS9960_H
#define ESPHOMELIB_SENSOR_APDS9960_H

#include "esphomelib/defines.h"

#ifdef USE_APDS9960

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/i2c_component.h"
#include "esphomelib/component.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

using APDS9960ColorChannelSensor = sensor::EmptyPollingParentSensor<1, ICON_LIGHTBULB, UNIT_PERCENT>;
// TODO: Icons
using APDS9960ProximitySensor = sensor::EmptyPollingParentSensor<1, ICON_LIGHTBULB, UNIT_PERCENT>;
class APDS9960GestureDirectionBinarySensor;

class APDS9960 : public PollingComponent, public I2CDevice {
 public:
  APDS9960(I2CComponent *parent, uint32_t update_interval = 15000);
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;
  void loop() override;

  APDS9960ColorChannelSensor *make_clear_channel(const std::string &name);
  APDS9960ColorChannelSensor *make_red_channel(const std::string &name);
  APDS9960ColorChannelSensor *make_green_channel(const std::string &name);
  APDS9960ColorChannelSensor *make_blue_channel(const std::string &name);
  APDS9960ColorChannelSensor *make_proximity(const std::string &name);
  APDS9960GestureDirectionBinarySensor *make_up_direction(const std::string &name);
  APDS9960GestureDirectionBinarySensor *make_down_direction(const std::string &name);
  APDS9960GestureDirectionBinarySensor *make_left_direction(const std::string &name);
  APDS9960GestureDirectionBinarySensor *make_right_direction(const std::string &name);

 protected:
  bool is_color_enabled_() const;
  bool is_proximity_enabled_() const;
  bool is_gesture_enabled_() const;
  void read_color_data_(uint8_t status);
  void read_proximity_data_(uint8_t status);
  void read_gesture_data_();
  void report_gesture_(int gesture);
  void process_dataset_(int up, int down, int left, int right);

  APDS9960ColorChannelSensor *red_channel_{nullptr};
  APDS9960ColorChannelSensor *green_channel_{nullptr};
  APDS9960ColorChannelSensor *blue_channel_{nullptr};
  APDS9960ColorChannelSensor *clear_channel_{nullptr};
  APDS9960GestureDirectionBinarySensor *up_direction_{nullptr};
  APDS9960GestureDirectionBinarySensor *right_direction_{nullptr};
  APDS9960GestureDirectionBinarySensor *down_direction_{nullptr};
  APDS9960GestureDirectionBinarySensor *left_direction_{nullptr};
  APDS9960ProximitySensor *proximity_{nullptr};
  enum ErrorCode {
    NONE = 0,
    COMMUNICATION_FAILED,
    WRONG_ID,
  } error_code_{NONE};
  bool gesture_up_started_{false};
  bool gesture_down_started_{false};
  bool gesture_left_started_{false};
  bool gesture_right_started_{false};
  uint32_t gesture_start_{0};
};

class APDS9960GestureDirectionBinarySensor : public binary_sensor::BinarySensor {
 public:
  APDS9960GestureDirectionBinarySensor(const std::string &name);
 protected:
  std::string device_class() override;
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_APDS9960

#endif //ESPHOMELIB_SENSOR_APDS9960_H
