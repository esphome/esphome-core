//
// Created by Otto Winter on 02.12.17.
//

#ifndef ESPHOMELIB_OUTPUT_BINARY_OUTPUT_H
#define ESPHOMELIB_OUTPUT_BINARY_OUTPUT_H

#include "esphomelib/switch_/switch.h"
#include "esphomelib/power_supply_component.h"

namespace esphomelib {

namespace output {

/** BinaryOutput - The base class for all binary outputs i.e. outputs that can only be switched on/off.
 *
 * This interface class provides one method you need to override in order to create a binary output
 * component yourself: write_value(). This method will be called for you by the MQTT Component through
 * set_state() to indicate that a new value should be written to hardware.
 *
 * Note that this class also allows the user to invert any state, but you don't need to worry about that
 * because the value will already be inverted (if specified by the user) within set_state_(). So write_state
 * will always receive the correctly inverted state.
 *
 * Additionally, this class provides high power mode capabilities using PowerSupplyComponent. Every time
 * the output is enabled (independent of inversion!), the power supply will automatically be turned on.
 */
class BinaryOutput {
 public:
  // ===== OVERRIDE THIS =====
  /// Write a binary state to hardware, inversion is already applied.
  virtual void write_enabled(bool enabled) = 0;
  // ===== OVERRIDE THIS =====

  /// Set the inversion state of this binary output.
  void set_inverted(bool inverted);

  /** Use this to connect up a power supply to this output.
   *
   * Whenever this output is enabled, the power supply will automatically be turned on.
   *
   * @param power_supply The PowerSupplyComponent, set this to nullptr to disable the power supply.
   */
  void set_power_supply(PowerSupplyComponent *power_supply);

  /// Enable this binary output.
  virtual void enable();

  /// Disable this binary output.
  virtual void disable();

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Return whether this binary output is inverted.
  bool is_inverted() const;

  /// Return the power supply assigned to this binary output.
  PowerSupplyComponent *get_power_supply() const;

 protected:
  bool inverted_{false};
  PowerSupplyComponent *power_supply_{nullptr};
  bool has_requested_high_power_{false};
};

} // namespace output

} // namespace esphomelib

#endif //ESPHOMELIB_OUTPUT_BINARY_OUTPUT_H
