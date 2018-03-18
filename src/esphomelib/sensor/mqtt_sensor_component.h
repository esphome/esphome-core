//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_SENSOR_MQTT_SENSOR_COMPONENT_H
#define ESPHOMELIB_SENSOR_MQTT_SENSOR_COMPONENT_H

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/helpers.h"
#include "filter.h"

namespace esphomelib {

namespace sensor {

class MQTTSensorComponent : public mqtt::MQTTComponent {
 public:
  MQTTSensorComponent(std::string friendly_name, std::string unit_of_measurement,
                      Optional<uint32_t> expire_after = Optional<uint32_t>());

  void setup() override;

  sensor_callback_t create_new_data_callback();

  void push_out_value(float value, int8_t accuracy_decimals);

  const std::string &get_unit_of_measurement() const;
  void set_unit_of_measurement(const std::string &unit_of_measurement);

  const Optional<uint32_t> &get_expire_after() const;
  void set_expire_after(const Optional<uint32_t> &expire_after);
  void disable_expiry();

  Filter *get_filter() const;
  void set_filter(Filter *filter);
  void disable_filter();

  float get_offset() const;
  void set_offset(float offset);

 protected:
  std::string component_type() const override;

 private:
  std::string unit_of_measurement_;
  Optional<uint32_t> expire_after_;
  Filter *filter_{nullptr};
  float offset_;
};

} // namespace sensor

} // namespace esphomelib

#endif //ESPHOMELIB_SENSOR_MQTT_SENSOR_COMPONENT_H
