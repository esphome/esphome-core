#ifndef ESPHOMELIB_OUTPUT_LEDC_OUTPUT_COMPONENT_H
#define ESPHOMELIB_OUTPUT_LEDC_OUTPUT_COMPONENT_H

#include "esphomelib/defines.h"

#ifdef USE_LEDC_OUTPUT

#include "esphomelib/output/float_output.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace output {

/// ESP32 LEDC output component.
class LEDCOutputComponent : public FloatOutput, public Component {
 public:
  /// Construct a LEDCOutputComponent. The channel will be set using the next_ledc_channel global variable.
  explicit LEDCOutputComponent(uint8_t pin, float frequency = 1000.0f, uint8_t bit_depth = 12);

  /// Manually set the pin used for this output.
  void set_pin(uint8_t pin);
  /// Manually set the ledc_channel used for this component.
  void set_channel(uint8_t channel);
  /// Manually set the bit depth. Defaults to 12.
  void set_bit_depth(uint8_t bit_depth);

  /** Manually set frequency the LEDC timer should operate on.
   *
   * Two adjacent LEDC channels will usually receive the same timer and so can only have the same
   * frequency.
   *
   * @param frequency The frequency in Hz.
   */
  void set_frequency(float frequency);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Setup LEDC.
  void setup() override;
  /// HARDWARE setup priority
  float get_setup_priority() const override;

  /// Override FloatOutput's write_state.
  void write_state(float adjusted_value) override;

  float get_frequency() const;
  uint8_t get_bit_depth() const;
  uint8_t get_channel() const;
  uint8_t get_pin() const;

 protected:
  uint8_t pin_;
  uint8_t channel_;
  uint8_t bit_depth_;
  float frequency_;
};

extern uint8_t next_ledc_channel;

} // namespace output

ESPHOMELIB_NAMESPACE_END

#endif //USE_LEDC_OUTPUT

#endif //ESPHOMELIB_OUTPUT_LEDC_OUTPUT_COMPONENT_H
