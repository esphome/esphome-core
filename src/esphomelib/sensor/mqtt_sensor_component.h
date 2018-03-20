//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_SENSOR_MQTT_SENSOR_COMPONENT_H
#define ESPHOMELIB_SENSOR_MQTT_SENSOR_COMPONENT_H

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/helpers.h"
#include "esphomelib/sensor/filter.h"

namespace esphomelib {

namespace sensor {

class MQTTSensorComponent : public mqtt::MQTTComponent {
 public:
  /** Construct this MQTTSensorComponent instance with the provided friendly_name and sensor
   *
   * Note the sensor is never stored and is only used for initializing some values of this class.
   * If sensor is nullptr, then automatic initialization of these fields is disabled.
   *
   * @param friendly_name The friendly name that should be advertised to Home Assistant.
   * @param sensor The sensor, this can be null to disable automatic setup.
   */
  MQTTSensorComponent(std::string friendly_name, Sensor *sensor);

  /** Manually set the unit of measurement advertised to Home Assistant.
   *
   * This is automatically set by the constructor, but can later be overriden.
   *
   * @param unit_of_measurement The unit of measurement,
   */
  void set_unit_of_measurement(const std::string &unit_of_measurement);

  /** Override the accuracy in decimals that this value should use for reporting value.
   *
   * By default, the MQTTSensorComponent will use the accuracy in decimals provided
   * by the sensor. This method can be used to override the value. For example 0 means
   * 1.42 will be rounded to 1 and 2 means it will stay 1.42; also supports negative values.
   *
   * @param override_accuracy_decimals The accuracy decimal that shall be used.
   */
  void override_accuracy_decimals(int8_t override_accuracy_decimals);

  /// Setup an expiry
  void set_expire_after(uint32_t expire_after);
  void disable_expire_after();

  /// Add a filter to the filter chain. Will be appended to the back.
  void add_filter(Filter *filter);

  /** Add a lambda filter to the back of the filter chain.
   *
   * For example:
   * sensor->add_lambda_filter([](float value) -> Optional<float> {
   *   return value * 42;
   * });
   *
   * If you return an unset Optional, the value will be discarded and no
   * filters after this one will get the value.
   */
  void add_lambda_filter(lambda_filter_t filter);

  /** Helper to add a simple offset filter to the back of the filter chain.
   *
   * This can be used to easily correct for sensors that have a small offset
   * in their value reporting.
   *
   * @param offset The offset that will be added to each value.
   */
  void add_offset_filter(float offset);

  /** Helper to add a simple multiplier filter to the back of the filter chain.
   *
   * Each value will be multiplied by this multiplier. Can be used to convert units
   * easily. For example converting "pulses/min" to a more reasonable unit like kW.
   *
   * @param multiplier The multiplier each value will be multiplied with.
   */
  void add_multiplier_filter(float multiplier);

  /// Helper to make adding sliding window moving average filters a bit easier.
  void add_sliding_window_average_filter(size_t window_size, size_t send_every);

  /// Helper to make adding exponential decay average filters a bit easier.
  void add_exponential_moving_average_filter(float alpha, size_t send_every);

  /// Clear the entire filter chain.
  void clear_filters();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Override setup.
  void setup() override;

  /** This will create a callback that can be used to register this MQTTComponent
   * with the sensor.
   *
   * For example:
   *
   * sensor->set_new_value_callback(mqtt->create_new_data_callback());
   *
   * However, you normally don't need to worry about this as the Application instance
   * takes care of all of this.
   *
   * @return A new callback that can be registered to the sensor.
   */
  sensor_callback_t create_new_data_callback();

  const Optional<uint32_t> &get_expire_after() const;

  const Optional<int8_t> &get_override_accuracy_decimals() const;


  const std::string &get_unit_of_measurement() const;

  /** Return the vector of filters this component uses for its value calculations.
   *
   * Note that if you're using this method, you're probably doing something wrong.
   * The clear_filters() and add_filter() methods should be the only methods you need.
   *
   * @return Returns an std::vector<Filter *> of all filters in the filter chain.
   */
  std::vector<Filter *> get_filters() const;

 protected:
  /// Override for MQTTComponent, returns "sensor".
  std::string component_type() const override;

  /// Internal method, convert the value, accuracy pair to string
  /// and send them via MQTT.
  void push_out_value(float value, int8_t accuracy_decimals);

 private:
  std::string unit_of_measurement_;
  Optional<uint32_t> expire_after_;
  Optional<int8_t> override_accuracy_decimals_;
  std::vector<Filter *> filters_;
};

} // namespace sensor

} // namespace esphomelib

#endif //ESPHOMELIB_SENSOR_MQTT_SENSOR_COMPONENT_H
