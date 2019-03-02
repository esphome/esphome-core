#ifndef ESPHOME_OUTPUT_MY9231_OUTPUT_COMPONENT_H
#define ESPHOME_OUTPUT_MY9231_OUTPUT_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_MY9231_OUTPUT

#include "esphome/output/float_output.h"

ESPHOME_NAMESPACE_BEGIN

namespace output {

/// MY9231 float output component.
class MY9231OutputComponent : public Component {
 public:
  class Channel;
  /** Construct the component.
   *
   * @param pin_di The pin which DI is connected to.
   * @param pin_dcki The pin which DCKI is connected to.
   * @param num_channels Total number of channels of the whole daisy chain.
   * @param num_chips Number of chips in the daisy chain.
   * @param bit_depth Bit depth of each channel
   * @param update Update/reset duty data at boot (driver will keep
   *               configuration after powercycle)
   */
  MY9231OutputComponent(GPIOPin *pin_di, GPIOPin *pin_dcki, uint16_t num_channels = 6, uint8_t num_chips = 2,
                        uint8_t bit_depth = 16, bool update = true);

  /** Get a MY9231 output channel.
   *
   * @param channel The channel number. (0 is the closest channel)
   * @param power_supply The power supply that should be set for this channel.
   *                     Default: nullptr.
   * @param max_power The maximum power output of this channel. Each value will
   *                  be multiplied by this.
   * @return The new channel output component.
   */
  Channel *create_channel(uint8_t channel, PowerSupplyComponent *power_supply = nullptr, float max_power = 1.0f);

  /// Manually set the total number of channels. Defaults to 6.
  void set_num_channels(uint16_t num_channels);
  /// Manually set the number of chips. Defaults to 2.
  void set_num_chips(uint8_t num_chips);
  /// Manually set the bit depth. Defaults to 16.
  void set_bit_depth(uint8_t bit_depth);
  /// Manually set duty data update on boot. Defaults is true.
  void set_update(bool update);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  uint16_t get_num_channels() const;
  uint8_t get_num_chips() const;
  uint8_t get_bit_depth() const;
  uint16_t get_max_amount() const;

  /// Setup the MY9231.
  void setup() override;
  void dump_config() override;
  /// HARDWARE setup_priority
  float get_setup_priority() const override;
  /// Send new values if they were updated.
  void loop() override;

  class Channel : public FloatOutput {
   public:
    Channel(MY9231OutputComponent *parent, uint8_t channel);

   protected:
    void write_state(float state) override;

    MY9231OutputComponent *parent_;
    uint8_t channel_;
  };

 protected:
  void set_channel_value_(uint8_t channel, uint16_t value);
  void init_chips_(uint8_t command);
  void write_word_(uint16_t value, uint8_t bits);
  void send_di_pulses_(uint8_t count);

  GPIOPin *pin_di_;
  GPIOPin *pin_dcki_;
  uint8_t bit_depth_;
  uint16_t num_channels_;
  uint8_t num_chips_;
  std::vector<uint16_t> pwm_amounts_;
  bool update_;
};

}  // namespace output

ESPHOME_NAMESPACE_END

#endif  // USE_MY9231_OUTPUT

#endif  // ESPHOME_OUTPUT_MY9231_OUTPUT_COMPONENT_H
