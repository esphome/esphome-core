//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_ATX_COMPONENT_H
#define ESPHOMELIB_ATX_COMPONENT_H

#include "component.h"

namespace esphomelib {

/** ATXComponent - This class represents an ATX power supply.
 *
 * The power supply will automatically be turned on if a component requests high power and will automatically be
 * turned off again keep_on_time (ms) after the last high-power request. Aditionally, an enable_time (ms) can be
 * specified because many ATX power supplies only actually provide high-power output after a few milliseconds.
 */
class ATXComponent : public Component {
 public:
  /** Creates the ATXComponent
   *
   * @param pin The pin of the ATX green control wire.
   * @param enable_time The time in milliseconds the power supply requires for power up.
   * @param keep_on_time The time in milliseconds the power supply should be kept on after the last high-power request.
   */
  explicit ATXComponent(uint8_t pin, uint32_t enable_time = 20, uint32_t keep_on_time = 10000);

  uint8_t get_pin() const;
  void set_pin(uint8_t pin);
  uint32_t get_enable_time() const;
  void set_enable_time(uint32_t enable_time);
  uint32_t get_keep_on_time() const;
  void set_keep_on_time(uint32_t keep_on_time);

  bool is_enabled() const;

  /// Request high-power mode, this should be called in every loop() iteration of your component to keep it powered on.
  void enable();

  void setup() override;
  float get_setup_priority() const override;

 private:
  uint8_t pin_;
  bool enabled_;
  uint32_t enable_time_;
  uint32_t keep_on_time_;
};

} // namespace esphomelib

#endif //ESPHOMELIB_ATX_COMPONENT_H
