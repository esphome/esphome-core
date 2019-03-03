#ifndef ESPHOME_BINARY_SENSOR_ESP32_TOUCH_BINARY_SENSOR_H
#define ESPHOME_BINARY_SENSOR_ESP32_TOUCH_BINARY_SENSOR_H

#include "esphome/defines.h"

#ifdef USE_ESP32_TOUCH_BINARY_SENSOR

#include "esphome/binary_sensor/binary_sensor.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

class ESP32TouchBinarySensor;

/** This class is a hub for all touch pads on the ESP32 and only one of these can exist at once.
 *
 * With this component you can use any of the touch-enabled pins of the ESP32 to detected touches
 * using the internal touch peripheral. It works by rapidly charging the touch pads and measuring
 * how long it takes for them to discharge again. This process is done many times on all touch pins
 * and the result is a 16-bit unsigned value. This value is usually around 0-800 when a touch has
 * been detected and around 1000-2000 when no touch has been detected. So the smaller the value,
 * the more likely a touch is happening. Note that this value varies greatly between all touch pins
 * and also between boards.
 *
 * This component uses the measured touch value and applies a simple threshold. If the measured value
 * is below the threshold, the binary sensor for a touch pad will go ON, and if it's above, the binary
 * sensor will report an OFF state again.
 *
 * If you notice the values have a lot of noise for your device and cause many false-positive touch events,
 * you can optionally setup an IIR Filter for globally across all touch pads. If this filter value is large
 * (100 ms+) the touch pad will become less responsive and only trigger if the user touches it for a longer time,
 * but fewer ghost touch events will happen. The IIR Filter is off by default.
 *
 * Additionally, if you want to tinker around with the internal values for esp-idf in order to improve
 * performance and decrease power usage, you can use the set_sleep_cycle, `set_meas_cycle()`,
 * `set_low_voltage_reference()`, `set_high_voltage_reference()` and `set_voltage_attenuation()` methods.
 *
 * When setting up the threshold values, it can be useful to get the "real" measured touch values
 * and not just the binary ON/OFF state. For this there's the `set_setup_mode()` method which can be
 * used to enable, as the name implies, a setup mode. In this mode the component will regularly send out
 * the measured touch values in the logs using the DEBUG log level. Be sure to turn it off again afterwards!
 */
class ESP32TouchComponent : public Component {
 public:
  /** Create a single touch pad binary sensor using the provided threshold value.
   *
   * Values for touch_pad can be:
   *
   *  - `TOUCH_PAD_NUM0` (GPIO 4)
   *  - `TOUCH_PAD_NUM1` (GPIO 0)
   *  - `TOUCH_PAD_NUM2` (GPIO 2)
   *  - `TOUCH_PAD_NUM3` (GPIO 15)
   *  - `TOUCH_PAD_NUM4` (GPIO 13)
   *  - `TOUCH_PAD_NUM5` (GPIO 12)
   *  - `TOUCH_PAD_NUM6` (GPIO 14)
   *  - `TOUCH_PAD_NUM7` (GPIO 27)
   *  - `TOUCH_PAD_NUM8` (GPIO 33)
   *  - `TOUCH_PAD_NUM9` (GPIO 32)
   *
   * Additionally, you need to register the touch pad in the application instance like this:
   *
   * ```cpp
   * App.register_binary_sensor(
   *     touch_hub->make_touch_pad("ESP32 Touch Pad 9", TOUCH_PAD_NUM9, 1000)
   * );
   * ```
   *
   * @param name The name of the binary sensor.
   * @param touch_pad The touch pad to use.
   * @param threshold The threshold touch value for detecting touches, smaller means higher probability
   * that a touch is active.
   * @see set_setup_mode for finding values for the threshold parameter.
   * @return An ESP32TouchBinarySensor that needs to be registered in the Application instance.
   */
  ESP32TouchBinarySensor *make_touch_pad(const std::string &name, touch_pad_t touch_pad, uint16_t threshold);

  /** Put this hub into a setup mode in which all touch pad readings will be fed into the debug logs.
   *
   * Be sure to turn this off again after determining the required threshold values for each touch pad!
   *
   * @param setup_mode The new setup mode, default is OFF (false).
   */
  void set_setup_mode(bool setup_mode);

  /** Setup an infinite impulse response filter to improve accuracy of the touch readings.
   *
   * See [Filtering
   * Pulses](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/touch_pad.html#filtering-of-measurements
   * for more information). Good values for this can be 20ms or so, but often the default of no filter works
   * just fine.
   *
   * @param iir_filter The new IIR Filter period in ms. Default is OFF (0).
   */
  void set_iir_filter(uint32_t iir_filter);

  /** Set how many RTC SLOW clock cycles (150kHz) the touch peripheral should sleep between measurements.
   *
   * Smaller values can improve power usage, but negatively affect response times.
   *
   * @see set_measurement_duration
   * @param sleep_duration The sleep length. Default is 4096 (27.3 ms)
   */
  void set_sleep_duration(uint16_t sleep_duration);

  /** Set how many APB clock cycles (8MHz) the touch pad peripheral should stay active for measuring.
   *
   * Higher values can improve response times, but negatively affect power usage.
   *
   * @see set_sleep_duration
   * @param meas_cycle The measurement cycle length. Default is the maximum of 65535 (8ms).
   */
  void set_measurement_duration(uint16_t meas_cycle);

  /** Set the touch sensor low voltage reference for discharging.
   *
   * Use this together with set_high_voltage_reference to improve touch detection.
   * A higher difference between the high and low voltage means that more discharge/charge cycles
   * can be made and thus increase the sensitivity. However, a smaller gap usually also means
   * that more noise will be generated, but that can be alleviated with the IIR software filter.
   *
   * Possible values are:
   *
   *  - `TOUCH_LVOLT_0V5` (default)
   *  - `TOUCH_LVOLT_0V6`
   *  - `TOUCH_LVOLT_0V7`
   *  - `TOUCH_LVOLT_0V8`
   *
   * @see set_high_voltage_reference and set_voltage_attenuation
   * @param low_voltage_reference The new low voltage reference.
   */
  void set_low_voltage_reference(touch_low_volt_t low_voltage_reference);

  /** Set the touch sensor high voltage reference for charging.
   *
   * Use this together with set_low_voltage_reference to improve touch detection.
   * A higher difference between the high and low voltage means that more discharge/charge cycles
   * can be made and thus increase the sensitivity. However, a smaller gap usually also means
   * that more noise will be generated, but that can be alleviated with the IIR software filter.
   *
   * Possible values are:
   *
   *  - `TOUCH_HVOLT_2V4`
   *  - `TOUCH_HVOLT_2V5`
   *  - `TOUCH_HVOLT_2V6`
   *  - `TOUCH_HVOLT_2V7` (default)
   *
   * @see set_low_voltage_reference and set_voltage_attenuation
   * @param high_voltage_reference The new high voltage reference.
   */
  void set_high_voltage_reference(touch_high_volt_t high_voltage_reference);

  /** Set the voltage attenuation of the touch peripheral.
   *
   * Possible values are:
   *
   *  - `TOUCH_HVOLT_ATTEN_1V5`
   *  - `TOUCH_HVOLT_ATTEN_1V`
   *  - `TOUCH_HVOLT_ATTEN_0V5`
   *  - `TOUCH_HVOLT_ATTEN_0V` (default)
   *
   * @see set_low_voltage_reference and set_high_voltage_reference
   * @param voltage_attenuation The new voltage attenuation
   */
  void set_voltage_attenuation(touch_volt_atten_t voltage_attenuation);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override;

 protected:
  /// Is the IIR filter enabled?
  bool iir_filter_enabled_() const;

  uint16_t sleep_cycle_{4096};
  uint16_t meas_cycle_{65535};
  touch_low_volt_t low_voltage_reference_{TOUCH_LVOLT_0V5};
  touch_high_volt_t high_voltage_reference_{TOUCH_HVOLT_2V7};
  touch_volt_atten_t voltage_attenuation_{TOUCH_HVOLT_ATTEN_0V};
  std::vector<ESP32TouchBinarySensor *> children_;
  bool setup_mode_{false};
  uint32_t iir_filter_{0};
};

/// Simple helper class to expose a touch pad value as a binary sensor.
class ESP32TouchBinarySensor : public BinarySensor {
 public:
  ESP32TouchBinarySensor(const std::string &name, touch_pad_t touch_pad, uint16_t threshold);

  touch_pad_t get_touch_pad() const;
  uint16_t get_threshold() const;

 protected:
  friend ESP32TouchComponent;

  touch_pad_t touch_pad_;
  uint16_t threshold_;
};

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_ESP32_TOUCH_BINARY_SENSOR

#endif  // ESPHOME_BINARY_SENSOR_ESP32_TOUCH_BINARY_SENSOR_H
