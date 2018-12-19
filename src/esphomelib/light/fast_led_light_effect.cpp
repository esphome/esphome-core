#include "esphomelib/defines.h"

#ifdef USE_FAST_LED_LIGHT

#include "esphomelib/light/fast_led_light_effect.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

inline CRGB random_crgb_slow() {
  const float rF = random_float();
  const float gF = random_float();
  const float bF = random_float();
  const float maxRGB = std::max(rF, std::max(gF, bF));
  return CRGB(
      uint8_t((255 * rF / maxRGB)),
      uint8_t((255 * gF / maxRGB)),
      uint8_t((255 * bF / maxRGB))
  );
}

void BaseFastLEDLightEffect::start_() {
  this->get_fastled_output_()->prevent_writing_leds();
  for (int i = 0; i < this->get_fastled_output_()->size(); i++) {
    this->get_fastled_output_()->effect_data()[i] = 0;
  }
  this->high_freq_.start();
  this->start();
}

void BaseFastLEDLightEffect::stop() {
  this->get_fastled_output_()->unprevent_writing_leds();
  this->high_freq_.stop();
}
FastLEDLightOutputComponent *BaseFastLEDLightEffect::get_fastled_output_() const {
  return (FastLEDLightOutputComponent *) this->state_->get_output();
}

BaseFastLEDLightEffect::BaseFastLEDLightEffect(const std::string &name) : LightEffect(name) {

}
void BaseFastLEDLightEffect::apply() {
  float brightness_f, r_f, g_f, b_f;
  LightColorValues color = this->state_->get_remote_values();
  color.as_brightness(&brightness_f);
  color.as_rgb(&r_f, &g_f, &b_f);
  const auto brightness = uint8_t(brightness_f * 255);
  const auto r = uint8_t(r_f * 255);
  const auto g = uint8_t(g_f * 255);
  const auto b = uint8_t(b_f * 255);
  this->apply(*this->get_fastled_output_(), brightness, CRGB(r, g, b));
}

FastLEDRainbowLightEffect::FastLEDRainbowLightEffect(const std::string &name) : BaseFastLEDLightEffect(name) {

}
void FastLEDRainbowLightEffect::apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) {
  CHSV hsv;
  hsv.val = brightness;
  hsv.sat = 240;
  uint16_t hue = (millis() * this->speed_) % 0xFFFF;
  const uint16_t add = 0xFFFF / this->width_;
  for (CRGB &led : fastled) {
    hsv.hue = hue / 0xFF;
    led = hsv;
    hue += add;
  }
  fastled.schedule_show();
}
void FastLEDRainbowLightEffect::set_speed(uint32_t speed) {
  this->speed_ = speed;
}
void FastLEDRainbowLightEffect::set_width(uint32_t width) {
  this->width_ = width;
}
FastLEDLambdaLightEffect::FastLEDLambdaLightEffect(const std::string &name,
                                                   const std::function<void(FastLEDLightOutputComponent &)> &f,
                                                   uint32_t update_interval)
    : BaseFastLEDLightEffect(name), f_(f), update_interval_(update_interval) {

}
void FastLEDLambdaLightEffect::apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) {
  const uint32_t now = millis();
  if (now - this->last_run_ >= this->update_interval_) {
    this->last_run_ = now;
    this->f_(fastled);
  }
}

inline static uint8_t half_sin8(uint8_t v) {
  return sin16(uint16_t(v) * 128u) >> 8;
}

void FastLEDTwinkleEffect::apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) {
  const uint32_t now = millis();

  uint8_t pos_add = 0;
  if (now - this->last_progress_ > this->progress_interval_) {
    const uint32_t pos_add32 = (now - this->last_progress_) / this->progress_interval_;
    pos_add = pos_add32;
    this->last_progress_ += pos_add32 * this->progress_interval_;
  }

  for (int i = 0; i < fastled.size(); i++) {
    if (fastled.effect_data()[i] != 0) {
      const uint16_t sine = half_sin8(fastled.effect_data()[i]);
      fastled[i] = CRGB(
          (sine * rgb.r) / 255,
          (sine * rgb.g) / 255,
          (sine * rgb.b) / 255
      );
      const uint8_t new_pos = fastled.effect_data()[i] + pos_add;
      if (new_pos < fastled.effect_data()[i])
        fastled.effect_data()[i] = 0;
      else
        fastled.effect_data()[i] = new_pos;
    } else {
      fastled[i] = CRGB(0, 0, 0);
    }
  }

  while (random_float() < this->twinkle_probability_) {
    const size_t pos = random_uint32() % fastled.size();
    if (fastled.effect_data()[pos] != 0)
      continue;
    fastled.effect_data()[pos] = 1;
  }

  fastled.schedule_show();
}
FastLEDTwinkleEffect::FastLEDTwinkleEffect(const std::string &name) : BaseFastLEDLightEffect(name) {}
void FastLEDTwinkleEffect::set_twinkle_probability(float twinkle_probability) {
  this->twinkle_probability_ = twinkle_probability;
}
void FastLEDTwinkleEffect::set_progress_interval(uint32_t progress_interval) {
  this->progress_interval_ = progress_interval;
}

void FastLEDRandomTwinkleEffect::apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) {
  const uint32_t now = millis();

  uint8_t pos_add = 0;
  if (now - this->last_progress_ > this->progress_interval_) {
    pos_add = (now - this->last_progress_) / this->progress_interval_;
    this->last_progress_ = now;
  }
  uint8_t subsine = ((8 * (now - this->last_progress_)) / this->progress_interval_) & 0b111;

  for (int i = 0; i < fastled.size(); i++) {
    if (fastled.effect_data()[i] != 0) {
      const uint8_t x = (fastled.effect_data()[i] >> 3) & 0b11111;
      const uint8_t color = fastled.effect_data()[i] & 0b111;
      const uint16_t sine = half_sin8((x << 3) | subsine);
      if (color == 0) {
        fastled[i] = CRGB(
            (uint16_t(rgb.r) * sine) / 255u,
            (uint16_t(rgb.g) * sine) / 255u,
            (uint16_t(rgb.b) * sine) / 255u
        );
      } else {
        const uint8_t mult = (sine * uint16_t(brightness)) / 255u;
        fastled[i] = CRGB(
            ((color >> 2) & 1) * mult,
            ((color >> 1) & 1) * mult,
            ((color >> 0) & 1) * mult
        );
      }
      const uint8_t new_x = x + pos_add;
      if (new_x > 0b11111)
        fastled.effect_data()[i] = 0;
      else
        fastled.effect_data()[i] = (new_x << 3) | color;
    } else {
      fastled[i] = CRGB(0, 0, 0);
    }
  }

  while (random_float() < this->twinkle_probability_) {
    const size_t pos = random_uint32() % fastled.size();
    if (fastled.effect_data()[pos] != 0)
      continue;

    const uint8_t color = random_uint32() & 0b111;
    fastled.effect_data()[pos] = 0b1000 | color;
  }

  fastled.schedule_show();
}
FastLEDRandomTwinkleEffect::FastLEDRandomTwinkleEffect(const std::string &name) : BaseFastLEDLightEffect(name) {}
void FastLEDRandomTwinkleEffect::set_twinkle_probability(float twinkle_probability) {
  this->twinkle_probability_ = twinkle_probability;
}
void FastLEDRandomTwinkleEffect::set_progress_interval(uint32_t progress_interval) {
  this->progress_interval_ = progress_interval;
}

void FastLEDFireworksEffect::apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) {
  const uint32_t now = millis();
  if (now - this->last_update_ < this->update_interval_)
    return;
  this->last_update_ = now;

  // "invert" the fade out parameter so that higher values make fade out faster
  const uint16_t fade_out_mult = 255u - this->fade_out_rate_;
  for (auto &pix : fastled) {
    for (uint8_t j = 0; j < 3; j++) {
      pix[j] = (fade_out_mult * uint16_t(pix[j])) / 255u;
      if (pix[j] < 64)
        pix[j] = 2 * pix[j] / 3;
    }
  }

  for (uint8_t j = 0; j < 3; j++) {
    uint16_t new_value = uint16_t(fastled[0][j]) + uint16_t(fastled[1][j] / 2);
    fastled[0][j] = new_value > 255 ? 255 : new_value;
  }
  const uint32_t last = fastled.size() - 1UL;
  for (size_t i = 1; i < last; i++) {
    for (uint8_t j = 0; j < 3; j++) {
      uint16_t new_value = uint16_t(fastled[i - 1][j] / 4) + uint16_t(fastled[i][j]) + uint16_t(fastled[i + 1][j] / 4);
      fastled[i][j] = new_value > 255 ? 255 : new_value;
    }
  }
  for (uint8_t j = 0; j < 3; j++) {
    uint16_t new_value = uint16_t(fastled[last][j]) + uint16_t(fastled[last - 1][j] / 2);
    fastled[last][j] = new_value > 255 ? 255 : new_value;
  }

  if (random_float() < this->spark_probability_) {
    const size_t pos = random_uint32() % fastled.size();
    if (this->use_random_color_) {
      fastled[pos] = random_crgb_slow();
    } else {
      fastled[pos] = rgb;
    }
  }

  fastled.schedule_show();
}

void FastLEDFireworksEffect::set_fade_out_rate(uint8_t fade_out_rate) {
  this->fade_out_rate_ = fade_out_rate;
}
void FastLEDFireworksEffect::set_update_interval(uint32_t update_interval) {
  this->update_interval_ = update_interval;
}
void FastLEDFireworksEffect::set_spark_probability(float spark_probability) {
  this->spark_probability_ = spark_probability;
}
void FastLEDFireworksEffect::set_use_random_color(bool random_color) {
  this->use_random_color_ = random_color;
}
FastLEDFireworksEffect::FastLEDFireworksEffect(const std::string &name) : BaseFastLEDLightEffect(name) {}

void FastLEDFireworksEffect::init() {
  for (auto &pix : *this->get_fastled_output_())
    pix = CRGB(0, 0, 0);
}

void FastLEDFlickerEffect::apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) {
  const uint32_t now = millis();
  if (now - this->last_update_ < this->update_interval_)
    return;
  this->last_update_ = now;
  random16_add_entropy(random_uint32());

  const auto max_flicker = static_cast<uint8_t>(brightness * this->intensity_);
  for (auto &pix : fastled) {
    const uint8_t flicker = random8() % max_flicker;
    for (uint8_t i = 0; i < 3; i++) {
      pix[i] = (15u * uint16_t(pix[i])) / 16 + rgb[i] / 16u;
      if (flicker > pix[i]) pix[i] = 0;
      else pix[i] = pix[i] - flicker;
    }
  }

  fastled.schedule_show();
}
FastLEDFlickerEffect::FastLEDFlickerEffect(const std::string &name) : BaseFastLEDLightEffect(name) {}
void FastLEDFlickerEffect::set_update_interval(uint32_t update_interval) {
  this->update_interval_ = update_interval;
}
void FastLEDFlickerEffect::set_intensity(float intensity) {
  this->intensity_ = intensity;
}
FastLEDColorWipeEffect::FastLEDColorWipeEffect(const std::string &name) : BaseFastLEDLightEffect(name) {
  this->colors_.reserve(1);
  this->colors_.push_back(FastLEDColorWipeEffectColor {
      .r = 1.0f,
      .g = 1.0f,
      .b = 1.0f,
      .random = true,
      .num_leds = 1
  });
}
void FastLEDColorWipeEffect::set_colors(const std::vector<FastLEDColorWipeEffectColor> &colors) {
  this->colors_ = colors;
}
void FastLEDColorWipeEffect::set_add_led_interval(uint32_t add_led_interval) {
  this->add_led_interval_ = add_led_interval;
}
void FastLEDColorWipeEffect::set_reverse(bool reverse) {
  this->reverse_ = reverse;
}
void FastLEDColorWipeEffect::apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) {
  const uint32_t now = millis();
  if (now - this->last_add_ < this->add_led_interval_)
    return;
  this->last_add_ = now;

  if (!this->reverse_) {
    for (size_t i = 0; i < fastled.size() - 1UL; i++)
      fastled[i] = fastled[i + 1];
  } else {
    for (size_t i = fastled.size() - 1UL; i > 0; i++)
      fastled[i] = fastled[i - 1];
  }

  FastLEDColorWipeEffectColor &color = this->colors_[this->at_color_];
  const CRGB crgb = CRGB(
      uint8_t(color.r * brightness),
      uint8_t(color.g * brightness),
      uint8_t(color.b * brightness)
  );
  if (!this->reverse_) {
    fastled[fastled.size() - 1] = crgb;
  } else {
    fastled[0] = crgb;
  }

  if (++this->leds_added_ >= color.num_leds) {
    this->leds_added_ = 0;
    this->at_color_ = (this->at_color_ + 1) % this->colors_.size();
    FastLEDColorWipeEffectColor &new_color = this->colors_[this->at_color_];
    if (new_color.random) {
      CRGB c = random_crgb_slow();
      new_color.r = c.r;
      new_color.g = c.g;
      new_color.b = c.b;
    }
  }

  fastled.schedule_show();
}
FastLEDScanEffect::FastLEDScanEffect(const std::string &name) : BaseFastLEDLightEffect(name) {}
void FastLEDScanEffect::set_move_interval(uint32_t move_interval) {
  this->move_interval_ = move_interval;
}
void FastLEDScanEffect::apply(FastLEDLightOutputComponent &fastled, uint8_t brightness, CRGB rgb) {
  for (int i = 0; i < fastled.size(); i++) {
    if (i == this->at_led_)
      fastled[i] = rgb;
    else
      fastled[i] = CRGB(0, 0, 0);
  }

  const uint32_t now = millis();
  if (now - this->last_move_ > this->move_interval_) {
    if (direction_) {
      this->at_led_++;
      if (this->at_led_ == fastled.size() - 1)
        this->direction_ = false;
    } else {
      this->at_led_--;
      if (this->at_led_ == 0)
        this->direction_ = true;
    }
    this->last_move_ = now;
  }

  fastled.schedule_show();
}

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_FAST_LED_LIGHT
