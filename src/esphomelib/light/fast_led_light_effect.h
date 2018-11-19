#ifndef ESPHOMELIB_FAST_LED_LIGHT_EFFECT_H
#define ESPHOMELIB_FAST_LED_LIGHT_EFFECT_H

#include "esphomelib/defines.h"

#ifdef USE_FAST_LED_LIGHT

#include "esphomelib/light/light_effect.h"
#include "esphomelib/light/fast_led_light_output.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

class BaseFastLEDLightEffect : public LightEffect {
 public:
  explicit BaseFastLEDLightEffect(const std::string &name);
  void start_() override;
  void stop() override;

  virtual void apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) = 0;

  void apply() override;

 protected:
  FastLEDLightOutputComponent *get_fastled_output_() const;

  HighFrequencyLoopRequester high_freq_;
};

class FastLEDLambdaLightEffect : public BaseFastLEDLightEffect {
 public:
  FastLEDLambdaLightEffect(const std::string &name,
                           const std::function<void(FastLEDLightOutputComponent &)> &f,
                           uint32_t update_interval);

  void apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) override;
 protected:
  std::function<void(FastLEDLightOutputComponent &)> f_;
  uint32_t update_interval_;
  uint32_t last_run_{0};
};

/// Rainbow effect for FastLED
class FastLEDRainbowLightEffect : public BaseFastLEDLightEffect {
 public:
  explicit FastLEDRainbowLightEffect(const std::string &name);

  void apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) override;

  void set_speed(uint32_t speed);
  void set_width(uint32_t width);

 protected:
  uint32_t speed_{10};
  uint16_t width_{50};
};

struct FastLEDColorWipeEffectColor {
  float r, g, b;
  bool random;
  size_t num_leds;
};

class FastLEDColorWipeEffect : public BaseFastLEDLightEffect {
 public:
  explicit FastLEDColorWipeEffect(const std::string &name);

  void set_colors(const std::vector<FastLEDColorWipeEffectColor> &colors);
  void set_add_led_interval(uint32_t add_led_interval);
  void set_reverse(bool reverse);

  void apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) override;

 protected:
  std::vector<FastLEDColorWipeEffectColor> colors_;
  size_t at_color_{0};
  uint32_t last_add_{0};
  uint32_t add_led_interval_{100};
  size_t leds_added_{0};
  bool reverse_{false};
};

class FastLEDScanEffect : public BaseFastLEDLightEffect {
 public:
  explicit FastLEDScanEffect(const std::string &name);

  void set_move_interval(uint32_t move_interval);

  void apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) override;
 protected:
  uint32_t move_interval_{100};
  uint32_t last_move_{0};
  int at_led_{0};
  bool direction_{true};
};

class FastLEDTwinkleEffect : public BaseFastLEDLightEffect {
 public:
  explicit FastLEDTwinkleEffect(const std::string &name);
  void apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) override;

  void set_twinkle_probability(float twinkle_probability);
  void set_progress_interval(uint32_t progress_interval);

 protected:
  float twinkle_probability_{0.05f};
  uint32_t progress_interval_{4};
  uint32_t last_progress_{0};
};

class FastLEDRandomTwinkleEffect : public BaseFastLEDLightEffect {
 public:
  explicit FastLEDRandomTwinkleEffect(const std::string &name);
  void apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) override;
  void set_twinkle_probability(float twinkle_probability);
  void set_progress_interval(uint32_t progress_interval);
 protected:
  float twinkle_probability_{0.05f};
  uint32_t progress_interval_{32};
  uint32_t last_progress_{0};
};

class FastLEDFireworksEffect : public BaseFastLEDLightEffect {
 public:
  explicit FastLEDFireworksEffect(const std::string &name);
  void init() override;
  void apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) override;

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

class FastLEDFlickerEffect : public BaseFastLEDLightEffect {
 public:
  explicit FastLEDFlickerEffect(const std::string &name);
  void apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) override;
  void set_update_interval(uint32_t update_interval);
  void set_intensity(float intensity);
 protected:
  uint32_t update_interval_{16};
  uint32_t last_update_{0};
  float intensity_{0.05f};
};

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_FAST_LED_LIGHT

#endif //ESPHOMELIB_FAST_LED_LIGHT_EFFECT_H
