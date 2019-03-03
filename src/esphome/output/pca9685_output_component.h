#ifndef ESPHOME_OUTPUT_PCA9685_OUTPUT_COMPONENT_H
#define ESPHOME_OUTPUT_PCA9685_OUTPUT_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_PCA9685_OUTPUT

#include "esphome/output/float_output.h"
#include "esphome/i2c_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace output {

extern const uint8_t PCA9685_MODE_INVERTED;  // Inverts polarity of channel output signal
extern const uint8_t
    PCA9685_MODE_OUTPUT_ONACK;  // Channel update happens upon ACK (post-set) rather than on STOP (endTransmission)
extern const uint8_t
    PCA9685_MODE_OUTPUT_TOTEM_POLE;  // Use a totem-pole (push-pull) style output rather than an open-drain structure.
extern const uint8_t
    PCA9685_MODE_OUTNE_HIGHZ;  // For active low output enable, sets channel output to high-impedance state
extern const uint8_t PCA9685_MODE_OUTNE_LOW;  // Similarly, sets channel output to high if in totem-pole mode, otherwise
                                              // high-impedance state

/// PCA9685 float output component.
class PCA9685OutputComponent : public Component, public I2CDevice {
 public:
  class Channel;
  /** Construct the component.
   *
   * @param frequency The frequency of the PCA9685.
   * @param phase_balancer How to balance phases.
   * @param mode The output mode. For example, PCA9685_MODE_OUTPUT_ONACK or PCA9685_MODE_OUTPUT_TOTEM_POLE.
   */
  PCA9685OutputComponent(I2CComponent *parent, float frequency,
                         uint8_t mode = PCA9685_MODE_OUTPUT_ONACK | PCA9685_MODE_OUTPUT_TOTEM_POLE);

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
  /// Manually set the PCA9685 output mode, see constructor for more details.
  void set_mode(uint8_t mode);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  float get_frequency() const;
  uint8_t get_mode() const;

  /// Setup the PCA9685.
  void setup() override;
  void dump_config() override;
  /// HARDWARE setup_priority
  float get_setup_priority() const override;
  /// Send new values if they were updated.
  void loop() override;

  class Channel : public FloatOutput {
   public:
    Channel(PCA9685OutputComponent *parent, uint8_t channel);

   protected:
    void write_state(float state) override;

    PCA9685OutputComponent *parent_;
    uint8_t channel_;
  };

 protected:
  void set_channel_value_(uint8_t channel, uint16_t value);

  float frequency_;
  uint8_t mode_;

  uint8_t min_channel_;
  uint8_t max_channel_;
  uint16_t pwm_amounts_[16];
  bool update_;
};

}  // namespace output

ESPHOME_NAMESPACE_END

#endif  // USE_PCA9685_OUTPUT

#endif  // ESPHOME_OUTPUT_PCA9685_OUTPUT_COMPONENT_H
