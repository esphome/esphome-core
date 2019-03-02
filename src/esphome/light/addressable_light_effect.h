#ifndef ESPHOME_LIGHT_ADDRESSABLE_LIGHT_EFFECT_H
#define ESPHOME_LIGHT_ADDRESSABLE_LIGHT_EFFECT_H

#include "esphome/defines.h"

#ifdef USE_LIGHT

#include "esphome/light/light_effect.h"
#include "esphome/light/addressable_light.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

class AddressableLightEffect : public LightEffect {
 public:
  explicit AddressableLightEffect(const std::string &name);
  void start_internal() override;
  void stop() override;
  virtual void apply(AddressableLight &it, const ESPColor &current_color) = 0;
  void apply() override;

 protected:
  AddressableLight *get_addressable_() const;

  HighFrequencyLoopRequester high_freq_;
};

class AddressableLambdaLightEffect : public AddressableLightEffect {
 public:
  AddressableLambdaLightEffect(const std::string &name, const std::function<void(AddressableLight &)> &f,
                               uint32_t update_interval);
  void apply(AddressableLight &it, const ESPColor &current_color) override;

 protected:
  std::function<void(AddressableLight &)> f_;
  uint32_t update_interval_;
  uint32_t last_run_{0};
};

class AddressableRainbowLightEffect : public AddressableLightEffect {
 public:
  explicit AddressableRainbowLightEffect(const std::string &name);
  void apply(AddressableLight &it, const ESPColor &current_color) override;
  void set_speed(uint32_t speed);
  void set_width(uint32_t width);

 protected:
  uint32_t speed_{10};
  uint16_t width_{50};
};

struct AddressableColorWipeEffectColor {
  uint8_t r, g, b, w;
  bool random;
  size_t num_leds;
};

class AddressableColorWipeEffect : public AddressableLightEffect {
 public:
  explicit AddressableColorWipeEffect(const std::string &name);
  void set_colors(const std::vector<AddressableColorWipeEffectColor> &colors);
  void set_add_led_interval(uint32_t add_led_interval);
  void set_reverse(bool reverse);
  void apply(AddressableLight &it, const ESPColor &current_color) override;

 protected:
  std::vector<AddressableColorWipeEffectColor> colors_;
  size_t at_color_{0};
  uint32_t last_add_{0};
  uint32_t add_led_interval_{100};
  size_t leds_added_{0};
  bool reverse_{false};
};

class AddressableScanEffect : public AddressableLightEffect {
 public:
  explicit AddressableScanEffect(const std::string &name);
  void set_move_interval(uint32_t move_interval);
  void apply(AddressableLight &addressable, const ESPColor &current_color) override;

 protected:
  uint32_t move_interval_{100};
  uint32_t last_move_{0};
  int at_led_{0};
  bool direction_{true};
};

class AddressableTwinkleEffect : public AddressableLightEffect {
 public:
  explicit AddressableTwinkleEffect(const std::string &name);
  void apply(AddressableLight &addressable, const ESPColor &current_color) override;
  void set_twinkle_probability(float twinkle_probability);
  void set_progress_interval(uint32_t progress_interval);

 protected:
  float twinkle_probability_{0.05f};
  uint32_t progress_interval_{4};
  uint32_t last_progress_{0};
};

class AddressableRandomTwinkleEffect : public AddressableLightEffect {
 public:
  explicit AddressableRandomTwinkleEffect(const std::string &name);
  void apply(AddressableLight &it, const ESPColor &current_color) override;
  void set_twinkle_probability(float twinkle_probability);
  void set_progress_interval(uint32_t progress_interval);

 protected:
  float twinkle_probability_{0.05f};
  uint32_t progress_interval_{32};
  uint32_t last_progress_{0};
};

class AddressableFireworksEffect : public AddressableLightEffect {
 public:
  explicit AddressableFireworksEffect(const std::string &name);
  void start() override;
  void apply(AddressableLight &it, const ESPColor &current_color) override;
  void set_update_interval(uint32_t update_interval);
  void set_spark_probability(float spark_probability);
  void set_use_random_color(bool random_color);
  void set_fade_out_rate(uint8_t fade_out_rate);

 protected:
  uint8_t fade_out_rate_{120};
  uint32_t update_interval_{32};
  uint32_t last_update_{0};
  float spark_probability_{0.10f};
  bool use_random_color_{false};
};

class AddressableFlickerEffect : public AddressableLightEffect {
 public:
  explicit AddressableFlickerEffect(const std::string &name);
  void apply(AddressableLight &it, const ESPColor &current_color) override;
  void set_update_interval(uint32_t update_interval);
  void set_intensity(float intensity);

 protected:
  uint32_t update_interval_{16};
  uint32_t last_update_{0};
  uint8_t intensity_{13};
};

}  // namespace light

ESPHOME_NAMESPACE_END

#endif  // USE_LIGHT

#endif  // ESPHOME_LIGHT_ADDRESSABLE_LIGHT_EFFECT_H
