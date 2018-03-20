//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_POWER_SUPPLY_COMPONENT_H
#define ESPHOMELIB_POWER_SUPPLY_COMPONENT_H

#include "esphomelib/component.h"

namespace esphomelib {

/** PowerSupplyComponent - This class represents an power supply.
 *
 * The power supply will automatically be turned on if a component requests high power and will automatically be
 * turned off again keep_on_time (ms) after the last high-power request. Aditionally, an enable_time (ms) can be
 * specified because many power supplies only actually provide high-power output after a few milliseconds.
 */
class PowerSupplyComponent : public Component {
 public:
  /** Creates the PowerSupplyComponent
   *
   * @param pin The pin of the power supplycontrol wire.
   * @param enable_time The time in milliseconds the power supply requires for power up.
   * @param keep_on_time The time in milliseconds the power supply should be kept on after the last high-power request.
   */
  explicit PowerSupplyComponent(uint8_t pin, uint32_t enable_time = 20, uint32_t keep_on_time = 10000);

  uint8_t get_pin() const;
  void set_pin(uint8_t pin);
  uint32_t get_enable_time() const;
  void set_enable_time(uint32_t enable_time);
  uint32_t get_keep_on_time() const;
  void set_keep_on_time(uint32_t keep_on_time);
  bool is_inverted() const;
  /// Invert the output of the pin, essentially making it a active-low power supply.
  void set_inverted(bool inverted);

  bool is_enabled() const;

  /// Request high-power mode, this should be called in every loop() iteration of your component to keep it powered on.
  void enable();

  void setup() override;
  float get_setup_priority() const override;

 private:
  uint8_t pin_;
  bool inverted_{false};
  bool enabled_;
  uint32_t enable_time_;
  uint32_t keep_on_time_;
};

} // namespace esphomelib

#endif //ESPHOMELIB_POWER_SUPPLY_COMPONENT_H
