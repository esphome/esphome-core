#ifndef ESPHOME_SENSOR_PULSE_COUNTER_H
#define ESPHOME_SENSOR_PULSE_COUNTER_H

#include "esphome/defines.h"

#ifdef USE_PULSE_COUNTER_SENSOR

#include "esphome/sensor/sensor.h"
#include "esphome/esphal.h"

#ifdef ARDUINO_ARCH_ESP32
#include <driver/pcnt.h>
#endif

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

enum PulseCounterCountMode {
  PULSE_COUNTER_DISABLE = 0,
  PULSE_COUNTER_INCREMENT,
  PULSE_COUNTER_DECREMENT,
};

#ifdef ARDUINO_ARCH_ESP32
using pulse_counter_t = int16_t;
#endif
#ifdef ARDUINO_ARCH_ESP8266
using pulse_counter_t = int32_t;
#endif

class PulseCounterBase {
 public:
  PulseCounterBase(GPIOPin *pin);
  bool pulse_counter_setup();
  pulse_counter_t read_raw_value();

  GPIOPin *get_pin();

 protected:
#ifdef ARDUINO_ARCH_ESP8266
  static void gpio_intr(PulseCounterBase *arg);
  volatile pulse_counter_t counter_{0};
  volatile uint32_t last_pulse_{0};
#endif

  GPIOPin *pin_;
#ifdef ARDUINO_ARCH_ESP32
  pcnt_unit_t pcnt_unit_;
#endif
#ifdef ARDUINO_ARCH_ESP8266
  ISRInternalGPIOPin *isr_pin_;
#endif
  PulseCounterCountMode rising_edge_mode_{PULSE_COUNTER_INCREMENT};
  PulseCounterCountMode falling_edge_mode_{PULSE_COUNTER_DISABLE};
  uint32_t filter_us_{13};
  pulse_counter_t last_value_{0};
};

/** Pulse Counter - This is the sensor component for the ESP32 integrated pulse counter peripheral.
 *
 * It offers 8 pulse counter units that can be setup in several ways to count pulses on a pin.
 * Also allows for some simple filtering of short pulses using set_filter(), any pulse shorter than
 * the value provided to that function will be discarded. The time is given in APB clock cycles,
 * which usually amount to 12.5 ns per clock. Defaults to the max possible (about 13 µs).
 * See https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/pcnt.html for more information.
 *
 * The pulse counter defaults to reporting a value of the measurement unit "pulses/min". To
 * modify this behavior, use filters in MQTTSensor.
 */
class PulseCounterSensorComponent : public PollingSensorComponent, public PulseCounterBase {
 public:
  /** Construct the Pulse Counter instance with the provided pin and update interval.
   *
   * The pulse counter unit will automatically be set and the pulse counter is set up
   * to increment the counter on rising edges by default.
   *
   * @param pin The pin.
   * @param update_interval The update interval in ms.
   */
  explicit PulseCounterSensorComponent(const std::string &name, GPIOPin *pin, uint32_t update_interval = 60000);

  /// Set the PulseCounterCountMode for the rising and falling edges. can be disable, increment and decrement.
  void set_edge_mode(PulseCounterCountMode rising_edge_mode, PulseCounterCountMode falling_edge_mode);

  void set_filter_us(uint32_t filter_us);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Unit of measurement is "pulses/min".
  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;
  void setup() override;
  void update() override;
  float get_setup_priority() const override;
  void dump_config() override;
};

#ifdef ARDUINO_ARCH_ESP32
extern pcnt_unit_t next_pcnt_unit;
#endif

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PULSE_COUNTER_SENSOR

#endif  // ESPHOME_SENSOR_PULSE_COUNTER_H
