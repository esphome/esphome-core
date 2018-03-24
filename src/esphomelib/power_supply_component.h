//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_POWER_SUPPLY_COMPONENT_H
#define ESPHOMELIB_POWER_SUPPLY_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/esphal.h"

namespace esphomelib {

/** This class represents an power supply.
 *
 * The power supply will automatically be turned on if a component requests high power and will automatically be
 * turned off again keep_on_time (ms) after the last high-power request is cancelled. Aditionally, an
 * enable_time (ms) can be specified because many power supplies only actually provide high-power output
 * after a few milliseconds.
 *
 * Use the pin argument of the Application helper to enable inverted mode. For example most ATX power supplies
 * operate in inverted mode, so to turn them on you have to pull the pin LOW.
 *
 * To request high power mode, a component must use the request_high_power() function to register itself as
 * needing high power mode. Once the high power mode is no longer required the component can use unrequest_high_power()
 * to unregister its high power mode. IMPORTANT: An component should NOT hold multiple requests to the same
 * power supply, as the PowerSupplyComponent only holds an internal counter of how many high power requests have been
 * made.
 *
 * Usually though, all this should actually be handled by BinaryOutput and FloatOutput, since using this class
 * correctly is not too easy.
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

  /// Request high power mode. Use unrequest_high_power() to remove this request.
  void request_high_power();

  /// Un-request high power mode.
  void unrequest_high_power();

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

 protected:

  GPIOOutputPin pin_;
  bool enabled_{false};
  uint32_t enable_time_;
  uint32_t keep_on_time_;
  int16_t active_requests_{0};  // use signed integer to make catching negative requests easier.
};

} // namespace esphomelib

#endif //ESPHOMELIB_POWER_SUPPLY_COMPONENT_H
