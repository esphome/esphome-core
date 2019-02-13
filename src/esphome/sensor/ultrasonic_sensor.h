#ifndef ESPHOME_SENSOR_ULTRASONIC_SENSOR_H
#define ESPHOME_SENSOR_ULTRASONIC_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_ULTRASONIC_SENSOR

#include "esphome/sensor/sensor.h"
#include "esphome/esphal.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

/// The speed of sound in air in meters per µs.
const float SPEED_OF_SOUND_M_PER_US = 0.000343f;

/** This sensor component allows you to use your ultrasonic distance sensors.
 *
 * Instances of this component periodically send out a short pulse to an ultrasonic sensor, thus creating an ultrasonic
 * sound, and check how long it takes until an "echo" comes back. With this we can measure the distance to an object.
 *
 * Sometimes it can happen that we send a short ultrasonic pulse, but never hear the echo. In order to not wait for that
 * signal indefinitely, this component has an additional parameter that allows setting a timeout is microseconds (or
 * meters). If this timeout is reached, the sensor reports a "nan" (not a number) float value. The timeout defaults to
 * 11662µs or 2m.
 *
 * Usually these would be like HC-SR04 ultrasonic sensors: one trigger pin for sending the signal and another one echo
 * pin for receiving the signal. Be very careful with that sensor though: it's made for 5v VCC and doesn't work
 * very well with the ESP's 3.3V, so you need to create a voltage divider in order to not damage your ESP.
 *
 * Note: The MQTTSenorComponent will create a moving average over these values by default, to disable this behavior,
 *       call ultrasonic->mqtt->clear_filters() or similar like above.
 */
class UltrasonicSensorComponent : public PollingSensorComponent {
 public:
  /** Construct the ultrasonic sensor with the specified trigger pin and echo pin.
   *
   * @param trigger_pin The trigger pin where pulses are sent to.
   * @param echo_pin The echo pin where the echo is listened for.
   * @param update_interval The interval in ms the sensor should check for new values.
   */
  UltrasonicSensorComponent(const std::string &name,
                            GPIOPin *trigger_pin, GPIOPin *echo_pin,
                            uint32_t update_interval = 5000);

  /// Set the timeout for waiting for the echo in µs.
  void set_timeout_us(uint32_t timeout_us);
  /// Set the timeout for waiting for the echo in meter.
  void set_timeout_m(float timeout_m);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Get the timeout in meters for waiting for the echo.
  float get_timeout_m() const;
  /// Get the timeout in µs for waiting for the echo.
  uint32_t get_timeout_us() const;

  /// Set up pins and register interval.
  void setup() override;
  void dump_config() override;

  void update() override;

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;

  /// Hardware setup priority, before MQTT and WiFi.
  float get_setup_priority() const override;

  /// Set the time in µs the trigger pin should be enabled for in µs, defaults to 10µs (for HC-SR04)
  void set_pulse_time_us(uint32_t pulse_time_us);
  /// Return the time in µs the trigger pin should be enabled for.
  uint32_t get_pulse_time_us() const;

 protected:
  /// Helper function to convert the specified echo duration in µs to meters.
  static float us_to_m(uint32_t us);
  /// Helper function to convert the specified distance in meters to the echo duration in µs.
  static uint32_t m_to_us(float m);

  GPIOPin *trigger_pin_;
  GPIOPin *echo_pin_;
  uint32_t timeout_us_{11662}; /// 2 meters.
  uint32_t pulse_time_us_{10};
};

} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_ULTRASONIC_SENSOR

#endif //ESPHOME_SENSOR_ULTRASONIC_SENSOR_H
