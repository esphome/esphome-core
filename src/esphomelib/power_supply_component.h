//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_POWER_SUPPLY_COMPONENT_H
#define ESPHOMELIB_POWER_SUPPLY_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/esphal.h"

namespace esphomelib {

/** PowerSupplyComponent - This class represents an power supply.
 *
 * The power supply will automatically be turned on if a component requests high power and will automatically be
 * turned off again keep_on_time (ms) after the last high-power request. Aditionally, an enable_time (ms) can be
 * specified because many power supplies only actually provide high-power output after a few milliseconds.
 *
 * Use the pin obtained by get_pin() to enable inverted mode. For example most ATX power supplies operate in inverted
 * mode, so to turn them on you have to pull the pin LOW.
 *
 * auto *power_supply = app.make_power_supply(12);
 * power_supply->get_pin().set_inverted(true);
 *
 * // or alternatively:
 *
 * auto *power_supply = app.make_power_supply(GPIOOutputPin(12, OUTPUT, true));
 */
class PowerSupplyComponent : public Component {
 public:
  /** Creates the PowerSupplyComponent
   *
   * @param pin The pin of the power supply control wire.
   * @param enable_time The time in milliseconds the power supply requires for power up.
   *                    The thread will block in the meantime
   * @param keep_on_time The time in milliseconds the power supply should be kept on after the last high-power request.
   */
  explicit PowerSupplyComponent(GPIOOutputPin pin, uint32_t enable_time = 20, uint32_t keep_on_time = 10000);

  /// Set the time in milliseconds the power supply should be kept on for after the last high-power request.
  void set_keep_on_time(uint32_t keep_on_time);
  /// Manually set the pin for the power supply control wire.
  void set_pin(const GPIOOutputPin &pin);

  /// Set the time in milliseconds the power supply needs for power-up.
  void set_enable_time(uint32_t enable_time);

  /// Is this power supply currently on?
  bool is_enabled() const;

  /// Request high-power mode, to prevent the power supply from shutting down again, call this periodically.
  void enable();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)

  /// Register callbacks.
  void setup() override;
  /// Hardware setup priority (+1).
  float get_setup_priority() const override;

  /// Get the power supply control wire pin, use this for inverted mode, etc.
  GPIOOutputPin &get_pin();

  /// Get the keep on time.
  uint32_t get_keep_on_time() const;

  /// Get the enable time.
  uint32_t get_enable_time() const;

 private:
  GPIOOutputPin pin_;
  bool enabled_;
  uint32_t enable_time_;
  uint32_t keep_on_time_;
};

} // namespace esphomelib

#endif //ESPHOMELIB_POWER_SUPPLY_COMPONENT_H
