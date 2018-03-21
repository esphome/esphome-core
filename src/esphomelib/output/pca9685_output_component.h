//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_OUTPUT_PCA9685_OUTPUT_COMPONENT_H
#define ESPHOMELIB_OUTPUT_PCA9685_OUTPUT_COMPONENT_H

#include <PCA9685.h>

#include "esphomelib/output/float_output.h"

namespace esphomelib {

namespace output {

/// PCA9685 float output component.
class PCA9685OutputComponent : public Component {
 public:
  class Channel : public FloatOutput {
   public:
    Channel(PCA9685OutputComponent *parent, uint8_t channel);

    void write_state(float state) override;

   private:
    PCA9685OutputComponent *parent_;
    uint8_t channel_;
  };

  /** Construct the component.
   *
   * @param frequency The frequency of the PCA9685.
   * @param i2c_wire The TwoWire (i2c) interface.
   * @param phase_balancer How to balance phases.
   * @param mode The output mode. For example, PCA9685_MODE_OUTPUT_ONACK or PCA9685_MODE_OUTPUT_TPOLE.
   */
  explicit PCA9685OutputComponent(float frequency,
                                  TwoWire &i2c_wire = Wire,
                                  PCA9685_PhaseBalancer phase_balancer = PCA9685_PhaseBalancer_Linear,
                                  uint8_t mode = PCA9685_MODE_OUTPUT_ONACK | PCA9685_MODE_OUTPUT_TPOLE);

  float get_frequency() const;
  void set_frequency(float frequency);
  TwoWire &get_i2c_wire() const;
  void set_i2c_wire(TwoWire &i2c_wire);
  PCA9685_PhaseBalancer get_phase_balancer() const;
  void set_phase_balancer(PCA9685_PhaseBalancer phase_balancer);
  uint8_t get_address() const;
  void set_address(uint8_t address);
  uint8_t get_mode() const;
  void set_mode(uint8_t mode);

  void setup() override;
  float get_setup_priority() const override;

  void loop() override;

  /** Get a PCA9685 output channel.
   *
   * @param channel The channel number.
   * @param power_supply The power supply that should be set for this channel. Default: nullptr.
   * @return The new channel output component.
   */
  Channel *create_channel(uint8_t channel, PowerSupplyComponent *power_supply = nullptr, float max_power = 1.0f);

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

#endif //ESPHOMELIB_OUTPUT_PCA9685_OUTPUT_COMPONENT_H
