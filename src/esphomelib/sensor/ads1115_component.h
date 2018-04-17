//
//  ads1115_component.h
//  esphomelib
//
//  Created by Otto Winter on 25.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_INPUT_ADS1115_COMPONENT_H
#define ESPHOMELIB_INPUT_ADS1115_COMPONENT_H

#include <cstdint>

#include "esphomelib/log.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/defines.h"

#ifdef USE_ADS1115_SENSOR

#include "ADS1115.h"

namespace esphomelib {

namespace sensor {

class ADS1115Sensor;

/** This class allows you to use your ADS1115 devices with esphomelib through i2c.
 *
 * It is built of like the DallasComponent: A central hub (can be multiple ones) and multiple
 * Sensor instances that all access this central hub.
 *
 * Note that for this component to work correctly you need to have i2c setup. Do so with
 *
 * ```cpp
 * App.init_i2c(SDA_PIN, SCL_PIN);
 * ```
 *
 * before the call to `App.setup()`.
 */
class ADS1115Component : public Component {
 public:
  /** Construct the component hub for this ADS1115.
   *
   * @see set_address() for possible addresses.
   *
   * @param address The i2c address for this sensor.
   */
  explicit ADS1115Component(uint8_t address);

  /** Get a sensor from this ADS1115 from the specified multiplexer and gain.
   *
   * You can have one ADS1115 create multiple sensors with different multiplexers and/or gains.
   *
   * @param multiplexer The multiplexer, one of `ADS1115_MUX_` then `P0_N1`, `P0_N3`, `P1_N3`, `P2_N3`,
   *              `P0_NG`, `P1_NG`, `P2_NG`, `P3_NG`.
   * @param gain The gain, one of ADS1115_PGA_` then `6P144`, `4P096`, `2P048`, `1P024`,
   *             `0P512`, `0P256` (B/C).
   * @param update_interval The interval in milliseconds the value for this sensor should be checked.
   * @return An ADS1115Sensor, use this for advanced options.
   */
  ADS1115Sensor *get_sensor(const std::string &name, uint8_t multiplexer, uint8_t gain,
                            uint32_t update_interval = 15000);

  /** Manually set the i2c address for this ADS1115.
   *
   * If the address pin is pulled to GND, the address is 0x48.
   * If the address pin is pulled to VCC, the address is 0x49.
   * If the address pin is tied to SDA, the address is 0x4A.
   * If the address pin is tied to SCL, the address is 0x4B.
   *
   * @param address The i2c address
   */
  void set_address(uint8_t address);
  /// Manually set whether this ADS1115 should operate in continuous mode, defaults to true.
  void set_continuous_mode(bool continuous_mode);
  /** Manually set the rate that the ADS1115 uses internally, defaults to `ADS1115_RATE_128`.
   *
   * Possible values are `ADS1115_RATE_` then 8, 16, 32, 64, 128, 250, 475 and 860.
   *
   * @param rate The rate the sensor uses for its measurements internally (not or poll rate!).
   */
  void set_rate(uint8_t rate);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Set up the internal sensor array.
  void setup() override;
  /// HARDWARE_LATE setup priority
  float get_setup_priority() const override;
  /// Get the rate at which this sensor should be checked.
  uint8_t get_rate() const;
  /// Get the i2c address for this ADS1115.
  uint8_t get_address() const;
  /// Is this ADS1115 is continuous (non-singleshot mode)?
  bool is_continuous_mode() const;
  /// Get the ADS1115 instance used internally. Be careful with it, using this incorrectly might break stuff.
  ADS1115 &get_adc();

 protected:
  /// Helper method to set a gain if it isn't already the selected one.
  void set_gain_(uint8_t gain, bool force = false);
  /// Helper method to set the multiplexer if it isn't already the selected one.
  void set_multiplexer_(uint8_t multiplexer, bool force = false);

  /// Helper method to request a measurement from a sensor.
  void request_measurement_(ADS1115Sensor *sensor);

  ADS1115 adc_;
  uint8_t address_;
  uint8_t current_gain_;
  uint8_t current_multiplexer_;
  bool continuous_mode_{true};
  uint8_t rate_{ADS1115_RATE_128};
  std::vector<ADS1115Sensor *> sensors_;
};

/// Internal holder class that is in instance of Sensor so that the hub can create individual sensors.
class ADS1115Sensor : public sensor::Sensor {
 public:
  ADS1115Sensor(const std::string &name, uint8_t multiplexer, uint8_t gain, uint32_t update_interval);

  void set_multiplexer(uint8_t multiplexer);
  void set_update_interval(uint32_t update_interval);
  void set_gain(uint8_t gain);

  std::string unit_of_measurement() override;
  std::string icon() override;
  uint32_t update_interval() override;
  int8_t accuracy_decimals() override;

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  uint8_t get_multiplexer() const;
  uint32_t get_update_interval() const;
  uint8_t get_gain() const;

 protected:
  uint8_t multiplexer_;
  uint8_t gain_;
  uint32_t update_interval_;
};

} // namespace sensor

} // namespace esphomelib

#endif //USE_ADS1115_SENSOR

#endif //ESPHOMELIB_INPUT_ADS1115_COMPONENT_H
