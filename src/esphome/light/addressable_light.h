#ifndef ESPHOME_LIGHT_ADDRESSABLE_LIGHT_H
#define ESPHOME_LIGHT_ADDRESSABLE_LIGHT_H

#include "esphome/defines.h"

#ifdef USE_LIGHT

#include "esphome/helpers.h"
#include "esphome/light/light_state.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

inline static uint8_t esp_scale8(uint8_t i, uint8_t scale) ALWAYS_INLINE;

struct ESPColor {
  union {
    struct {
      union {
        uint8_t r;
        uint8_t red;
      };
      union {
        uint8_t g;
        uint8_t green;
      };
      union {
        uint8_t b;
        uint8_t blue;
      };
      union {
        uint8_t w;
        uint8_t white;
      };
    };
    uint8_t raw[4];
  };
  inline ESPColor() ALWAYS_INLINE;
  inline ESPColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t white) ALWAYS_INLINE;
  inline ESPColor(uint8_t red, uint8_t green, uint8_t blue) ALWAYS_INLINE;
  inline ESPColor(uint32_t colorcode) ALWAYS_INLINE;
  inline ESPColor(const ESPColor &rhs) ALWAYS_INLINE;
  inline bool is_on() ALWAYS_INLINE;
  inline ESPColor &operator=(const ESPColor &rhs) ALWAYS_INLINE;
  inline ESPColor &operator=(uint32_t colorcode) ALWAYS_INLINE;
  inline uint8_t &operator[](uint8_t x) ALWAYS_INLINE;
  inline ESPColor operator*(uint8_t scale) const ALWAYS_INLINE;
  inline ESPColor &operator*=(uint8_t scale) ALWAYS_INLINE;
  inline ESPColor operator*(const ESPColor &scale) const ALWAYS_INLINE;
  inline ESPColor &operator*=(const ESPColor &scale) ALWAYS_INLINE;
  inline ESPColor operator+(const ESPColor &add) const ALWAYS_INLINE;
  inline ESPColor &operator+=(const ESPColor &add) ALWAYS_INLINE;
  inline ESPColor operator+(uint8_t add) const ALWAYS_INLINE;
  inline ESPColor &operator+=(uint8_t add) ALWAYS_INLINE;
  inline ESPColor operator-(const ESPColor &subtract) const ALWAYS_INLINE;
  inline ESPColor &operator-=(const ESPColor &subtract) ALWAYS_INLINE;
  inline ESPColor operator-(uint8_t subtract) const ALWAYS_INLINE;
  inline ESPColor &operator-=(uint8_t subtract) ALWAYS_INLINE;
  static ESPColor random_color();
};

struct ESPHSVColor {
  union {
    struct {
      union {
        uint8_t hue;
        uint8_t h;
      };
      union {
        uint8_t saturation;
        uint8_t s;
      };
      union {
        uint8_t value;
        uint8_t v;
      };
    };
    uint8_t raw[3];
  };
  inline ESPHSVColor() ALWAYS_INLINE;
  inline ESPHSVColor(uint8_t hue, uint8_t saturation, uint8_t value) ALWAYS_INLINE;
  ESPColor to_rgb() const;
};

class ESPColorCorrection {
 public:
  ESPColorCorrection();
  void set_max_brightness(const ESPColor &max_brightness);
  void set_local_brightness(uint8_t local_brightness);
  void calculate_gamma_table(float gamma);
  inline ESPColor color_correct(ESPColor color) const ALWAYS_INLINE;
  inline uint8_t color_correct_red(uint8_t red) const ALWAYS_INLINE;
  inline uint8_t color_correct_green(uint8_t green) const ALWAYS_INLINE;
  inline uint8_t color_correct_blue(uint8_t blue) const ALWAYS_INLINE;
  inline uint8_t color_correct_white(uint8_t white) const ALWAYS_INLINE;
  inline ESPColor color_uncorrect(ESPColor color) const ALWAYS_INLINE;
  inline uint8_t color_uncorrect_red(uint8_t red) const ALWAYS_INLINE;
  inline uint8_t color_uncorrect_green(uint8_t green) const ALWAYS_INLINE;
  inline uint8_t color_uncorrect_blue(uint8_t blue) const ALWAYS_INLINE;
  inline uint8_t color_uncorrect_white(uint8_t white) const ALWAYS_INLINE;

 protected:
  uint8_t gamma_table_[256];
  uint8_t gamma_reverse_table_[256];
  ESPColor max_brightness_;
  uint8_t local_brightness_{255};
};

class ESPColorView {
 public:
  inline ESPColorView(uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *white, uint8_t *effect_data,
                      const ESPColorCorrection *color_correction) ALWAYS_INLINE;
  inline const ESPColorView &operator=(const ESPColor &rhs) const ALWAYS_INLINE;
  inline const ESPColorView &operator=(const ESPHSVColor &rhs) const ALWAYS_INLINE;
  inline void set(const ESPColor &color) const ALWAYS_INLINE;
  inline void set(const ESPHSVColor &color) const ALWAYS_INLINE;
  inline void set_red(uint8_t red) const ALWAYS_INLINE;
  inline void set_green(uint8_t green) const ALWAYS_INLINE;
  inline void set_blue(uint8_t blue) const ALWAYS_INLINE;
  inline void set_white(uint8_t white) const ALWAYS_INLINE;
  inline void set_rgb(uint8_t red, uint8_t green, uint8_t blue) const ALWAYS_INLINE;
  inline void set_rgbw(uint8_t red, uint8_t green, uint8_t blue, uint8_t white) const ALWAYS_INLINE;
  inline void set_effect_data(uint8_t effect_data) const ALWAYS_INLINE;
  inline ESPColor get() const ALWAYS_INLINE;
  inline uint8_t get_red() const ALWAYS_INLINE;
  inline uint8_t get_green() const ALWAYS_INLINE;
  inline uint8_t get_blue() const ALWAYS_INLINE;
  inline uint8_t get_white() const ALWAYS_INLINE;
  inline uint8_t get_effect_data() const ALWAYS_INLINE;
  inline void raw_set_color_correction(const ESPColorCorrection *color_correction) ALWAYS_INLINE;

 protected:
  uint8_t *const red_;
  uint8_t *const green_;
  uint8_t *const blue_;
  uint8_t *const white_;
  uint8_t *const effect_data_;
  const ESPColorCorrection *color_correction_;
};

class AddressableLight : public LightOutput {
 public:
  AddressableLight();
  virtual int32_t size() const = 0;
  virtual ESPColorView operator[](int32_t index) const = 0;
  virtual void clear_effect_data() = 0;
  bool is_effect_active() const;
  void set_effect_active(bool effect_active);
  void write_state(LightState *state) override;
  void set_correction(float red, float green, float blue, float white = 1.0f);
  void setup_state(LightState *state) override;
  void schedule_show();

 protected:
  bool should_show_() const;
  void mark_shown_();

  bool effect_active_{false};
  bool next_show_{true};
  ESPColorCorrection correction_{};
};

class AddressableSegment {
 public:
  AddressableSegment(LightState *src, int32_t src_offset, int32_t size);

  AddressableLight *get_src() const;
  int32_t get_src_offset() const;
  int32_t get_size() const;
  int32_t get_dst_offset() const;
  void set_dst_offset(int32_t dst_offset);

 protected:
  AddressableLight *src_;
  int32_t src_offset_;
  int32_t size_;
  int32_t dst_offset_;
};

class PartitionLightOutput : public AddressableLight, public Component {
 public:
  PartitionLightOutput(const std::vector<AddressableSegment> &segments);
  int32_t size() const override;
  ESPColorView operator[](int32_t index) const override;
  void clear_effect_data() override;
  LightTraits get_traits() override;
  void loop() override;

 protected:
  std::vector<AddressableSegment> segments_;
};

}  // namespace light

ESPHOME_NAMESPACE_END

#include "esphome/light/addressable_light.tcc"

#endif  // USE_LIGHT

#endif  // ESPHOME_LIGHT_ADDRESSABLE_LIGHT_H
