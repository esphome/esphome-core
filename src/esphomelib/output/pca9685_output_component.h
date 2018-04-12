//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_OUTPUT_PCA9685_OUTPUT_COMPONENT_H
#define ESPHOMELIB_OUTPUT_PCA9685_OUTPUT_COMPONENT_H

#include "esphomelib/output/float_output.h"
#include "esphomelib/defines.h"

#ifdef USE_PCA9685_OUTPUT

#include <PCA9685.h>

namespace esphomelib {

namespace output {

/// PCA9685 float output component.
class PCA9685OutputComponent : public Component {
 public:
  class Channel;
  /** Construct the component.
   *
   * @param frequency The frequency of the PCA9685.
   * @param phase_balancer How to balance phases.
   * @param mode The output mode. For example, PCA9685_MODE_OUTPUT_ONACK or PCA9685_MODE_OUTPUT_TPOLE.
   */
  explicit PCA9685OutputComponent(float frequency,
                                  PCA9685_PhaseBalancer phase_balancer = PCA9685_PhaseBalancer_Linear,
                                  uint8_t mode = PCA9685_MODE_OUTPUT_ONACK | PCA9685_MODE_OUTPUT_TPOLE);

  /** Get a PCA9685 output channel.
   *
   * @param channel The channel number.
   * @param power_supply The power supply that should be set for this channel. Default: nullptr.
   * @param max_power The maximum power output of this channel. Each value will be multiplied by this.
   * @return The new channel output component.
   */
  Channel *create_channel(uint8_t channel, PowerSupplyComponent *power_supply = nullptr, float max_power = 1.0f);

  /// Manually set the frequency of this PCA9685.
  void set_frequency(float frequency);
  /// Manually set the i2c TwoWire instance used for communication.
  void set_i2c_wire(TwoWire &i2c_wire);
  /** Manually set the PCA9685 used.
   *
   * Either PCA9685_PhaseBalancer_None, PCA9685_PhaseBalancer_Linear or PCA9685_PhaseBalancer_Weaved
   */
  void set_phase_balancer(PCA9685_PhaseBalancer phase_balancer);
  /// Set the i2c address for this PCA9685.
  void set_address(uint8_t address);
  /// Manually set the PCA9685 output mode, see constructor for more details.
  void set_mode(uint8_t mode);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  float get_frequency() const;
  TwoWire &get_i2c_wire() const;
  PCA9685_PhaseBalancer get_phase_balancer() const;
  uint8_t get_address() const;
  uint8_t get_mode() const;

  /// Setup the PCA9685.
  void setup() override;
  /// HARDWARE setup_priority
  float get_setup_priority() const override;
  /// Send new values if they were updated.
  void loop() override;

  class Channel : public FloatOutput {
   public:
    Channel(PCA9685OutputComponent *parent, uint8_t channel);

    void write_state(float state) override;

   protected:
    PCA9685OutputComponent *parent_;
    uint8_t channel_;
  };

 protected:
  void set_channel_value(uint8_t channel, uint16_t value);

  float frequency_;
  TwoWire &i2c_wire_;
  PCA9685_PhaseBalancer phase_balancer_;
  uint8_t address_;
  uint8_t mode_;

  PCA9685 pwm_controller_;
  uint8_t min_channel_;
  uint8_t max_channel_;
  uint16_t pwm_amounts_[16];
  bool update_;
};

} // namespace output

} // namespace esphomelib

#endif //USE_PCA9685_OUTPUT

#endif //ESPHOMELIB_OUTPUT_PCA9685_OUTPUT_COMPONENT_H
