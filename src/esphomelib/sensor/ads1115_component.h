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
#include "esphomelib/i2c_component.h"
#include "esphomelib/defines.h"

#ifdef USE_ADS1115_SENSOR

namespace esphomelib {

namespace sensor {

extern const uint8_t ADS1115_MULTIPLEXER_P0_N1;
extern const uint8_t ADS1115_MULTIPLEXER_P0_N3;
extern const uint8_t ADS1115_MULTIPLEXER_P1_N3;
extern const uint8_t ADS1115_MULTIPLEXER_P2_N3;
extern const uint8_t ADS1115_MULTIPLEXER_P0_NG;
extern const uint8_t ADS1115_MULTIPLEXER_P1_NG;
extern const uint8_t ADS1115_MULTIPLEXER_P2_NG;
extern const uint8_t ADS1115_MULTIPLEXER_P3_NG;

extern const uint8_t ADS1115_GAIN_6P144;
extern const uint8_t ADS1115_GAIN_4P096;
extern const uint8_t ADS1115_GAIN_2P048;
extern const uint8_t ADS1115_GAIN_1P024;
extern const uint8_t ADS1115_GAIN_0P512;
extern const uint8_t ADS1115_GAIN_0P256;
extern const uint8_t ADS1115_GAIN_0P256B;
extern const uint8_t ADS1115_GAIN_0P256C;

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
class ADS1115Component : public Component, public I2CDevice {
 public:
  /** Construct the component hub for this ADS1115.
   *
   * @param address The i2c address for this sensor.
   */
  ADS1115Component(I2CComponent *parent, uint8_t address);

  /** Get a sensor from this ADS1115 from the specified multiplexer and gain.
   *
   * You can have one ADS1115 create multiple sensors with different multiplexers and/or gains.
   *
   * @param multiplexer The multiplexer, one of `ADS1115_MULTIPLEXER_` then `P0_N1`, `P0_N3`, `P1_N3`, `P2_N3`,
   *              `P0_NG`, `P1_NG`, `P2_NG`, `P3_NG`.
   * @param gain The gain, one of ADS1115_GAIN_` then `6P144`, `4P096`, `2P048`, `1P024`,
   *             `0P512`, `0P256` (B/C).
   * @param update_interval The interval in milliseconds the value for this sensor should be checked.
   * @return An ADS1115Sensor, use this for advanced options.
   */
  ADS1115Sensor *get_sensor(const std::string &name, uint8_t multiplexer, uint8_t gain,
                            uint32_t update_interval = 15000);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Set up the internal sensor array.
  void setup() override;
  /// HARDWARE_LATE setup priority
  float get_setup_priority() const override;

 protected:
  /// Helper method to request a measurement from a sensor.
  void request_measurement_(ADS1115Sensor *sensor);

  std::vector<ADS1115Sensor *> sensors_;
};

/// Internal holder class that is in instance of Sensor so that the hub can create individual sensors.
class ADS1115Sensor : public sensor::EmptySensor<3, ICON_FLASH, UNIT_V> {
 public:
  ADS1115Sensor(const std::string &name, uint8_t multiplexer, uint8_t gain, uint32_t update_interval);

  void set_multiplexer(uint8_t multiplexer);
  void set_gain(uint8_t gain);

  uint32_t update_interval() override;

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  uint8_t get_multiplexer() const;
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
