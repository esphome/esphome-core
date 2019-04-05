#include "esphome/defines.h"

#ifdef USE_LIGHT

#include "esphome/light/addressable_light.h"
#include "esphome/log.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace light {

ESPColor HOT ESPColor::random_color() {
  uint32_t rand = random_uint32();
  uint8_t w = rand >> 24;
  uint8_t r = rand >> 16;
  uint8_t g = rand >> 8;
  uint8_t b = rand >> 0;
  const uint16_t max_rgb = std::max(r, std::max(g, b));
  return ESPColor(uint8_t((uint16_t(r) * 255U / max_rgb)), uint8_t((uint16_t(g) * 255U / max_rgb)),
                  uint8_t((uint16_t(b) * 255U / max_rgb)), w);
}

// based on FastLED's hsv rainbow to rgb
ESPColor HOT ESPHSVColor::to_rgb() const {
  const uint8_t hue = this->hue;
  const uint8_t sat = this->saturation;
  const uint8_t val = this->value;
  // upper 3 hue bits are for branch selection, lower 5 are for values
  const uint8_t offset8 = (hue & 0x1F) << 3;  // 0..248
  // third of the offset, 255/3 = 85 (actually only up to 82; 164)
  const uint8_t third = esp_scale8(offset8, 85);
  const uint8_t two_thirds = esp_scale8(offset8, 170);
  ESPColor rgb(255, 255, 255, 0);
  switch (hue >> 5) {
    case 0b000:
      rgb.r = 255 - third;
      rgb.g = third;
      rgb.b = 0;
      break;
    case 0b001:
      rgb.r = 171;
      rgb.g = 85 + third;
      rgb.b = 0;
      break;
    case 0b010:
      rgb.r = 171 - two_thirds;
      rgb.g = 170 + third;
      rgb.b = 0;
      break;
    case 0b011:
      rgb.r = 0;
      rgb.g = 255 - third;
      rgb.b = third;
      break;
    case 0b100:
      rgb.r = 0;
      rgb.g = 171 - two_thirds;
      rgb.b = 85 + two_thirds;
      break;
    case 0b101:
      rgb.r = third;
      rgb.g = 0;
      rgb.b = 255 - third;
      break;
    case 0b110:
      rgb.r = 85 + third;
      rgb.g = 0;
      rgb.b = 171 - third;
      break;
    case 0b111:
      rgb.r = 170 + third;
      rgb.g = 0;
      rgb.b = 85 - third;
      break;
    default:
      break;
  }
  // low saturation -> add uniform color to orig. hue
  // high saturation -> use hue directly
  // scales with square of saturation
  // (r,g,b) = (r,g,b) * sat + (1 - sat)^2
  rgb *= sat;
  const uint8_t desat = 255 - sat;
  rgb += esp_scale8(desat, desat);
  // (r,g,b) = (r,g,b) * val
  rgb *= val;
  return rgb;
}

ESPColorCorrection::ESPColorCorrection() : max_brightness_(255, 255, 255, 255) {}

void ESPColorCorrection::set_local_brightness(uint8_t local_brightness) { this->local_brightness_ = local_brightness; }

void ESPColorCorrection::set_max_brightness(const ESPColor &max_brightness) { this->max_brightness_ = max_brightness; }

void ESPColorCorrection::calculate_gamma_table(float gamma) {
  for (uint16_t i = 0; i < 256; i++) {
    // corrected = val ^ gamma
    auto corrected = static_cast<uint8_t>(roundf(255.0f * gamma_correct(i / 255.0f, gamma)));
    this->gamma_table_[i] = corrected;
  }
  if (gamma == 0.0f) {
    for (uint16_t i = 0; i < 256; i++)
      this->gamma_reverse_table_[i] = i;
    return;
  }
  for (uint16_t i = 0; i < 256; i++) {
    // val = corrected ^ (1/gamma)
    auto uncorrected = static_cast<uint8_t>(roundf(255.0f * powf(i / 255.0f, 1.0f / gamma)));
    this->gamma_reverse_table_[i] = uncorrected;
  }
}

AddressableLight::AddressableLight() = default;

bool AddressableLight::is_effect_active() const { return this->effect_active_; }

void AddressableLight::set_effect_active(bool effect_active) { this->effect_active_ = effect_active; }
void AddressableLight::write_state(LightState *state) {
  auto val = state->current_values;
  auto max_brightness = static_cast<uint8_t>(roundf(val.get_brightness() * val.get_state() * 255.0f));
  this->correction_.set_local_brightness(max_brightness);

  if (this->is_effect_active())
    return;

  // don't use LightState helper, gamma correction+brightness is handled by ESPColorView
  ESPColor color = ESPColor(uint8_t(roundf(val.get_red() * 255.0f)), uint8_t(roundf(val.get_green() * 255.0f)),
                            uint8_t(roundf(val.get_blue() * 255.0f)),
                            // white is not affected by brightness; so manually scale by state
                            uint8_t(roundf(val.get_white() * val.get_state() * 255.0f)));

  for (int i = 0; i < this->size(); i++) {
    (*this)[i] = color;
  }

  this->schedule_show();
}
void AddressableLight::set_correction(float red, float green, float blue, float white) {
  this->correction_.set_max_brightness(ESPColor(uint8_t(roundf(red * 255.0f)), uint8_t(roundf(green * 255.0f)),
                                                uint8_t(roundf(blue * 255.0f)), uint8_t(roundf(white * 255.0f))));
}
void AddressableLight::setup_state(LightState *state) {
  this->correction_.calculate_gamma_table(state->get_gamma_correct());
}
void AddressableLight::schedule_show() { this->next_show_ = true; }
bool AddressableLight::should_show_() const { return this->effect_active_ || this->next_show_; }
void AddressableLight::mark_shown_() { this->next_show_ = false; }

int32_t PartitionLightOutput::size() const {
  auto &last_seg = this->segments_[this->segments_.size() - 1];
  return last_seg.get_dst_offset() + last_seg.get_size();
}
ESPColorView PartitionLightOutput::operator[](int32_t index) const {
  uint32_t lo = 0;
  uint32_t hi = this->segments_.size() - 1;
  while (lo < hi) {
    uint32_t mid = (lo + hi) / 2;
    int32_t begin = this->segments_[mid].get_dst_offset();
    int32_t end = begin + this->segments_[mid].get_size();
    if (index < begin) {
      hi = mid - 1;
    } else if (index >= end) {
      lo = mid + 1;
    } else {
      lo = hi = mid;
    }
  }
  auto &seg = this->segments_[lo];
  // offset within the segment
  int32_t seg_off = index - seg.get_dst_offset();
  // offset within the src
  int32_t src_off = seg.get_src_offset() + seg_off;
  auto view = (*seg.get_src())[src_off];
  view.raw_set_color_correction(&this->correction_);
  return view;
}
void PartitionLightOutput::clear_effect_data() {
  for (auto &seg : this->segments_) {
    seg.get_src()->clear_effect_data();
  }
}
LightTraits PartitionLightOutput::get_traits() { return this->segments_[0].get_src()->get_traits(); }
PartitionLightOutput::PartitionLightOutput(const std::vector<AddressableSegment> &segments) : segments_(segments) {
  int32_t off = 0;
  for (auto &seg : this->segments_) {
    seg.set_dst_offset(off);
    off += seg.get_size();
  }
}
void PartitionLightOutput::loop() {
  if (this->should_show_()) {
    for (auto seg : this->segments_) {
      seg.get_src()->schedule_show();
    }
    this->mark_shown_();
  }
}

AddressableSegment::AddressableSegment(LightState *src, int32_t src_offset, int32_t size)
    : src_(static_cast<AddressableLight *>(src->get_output())), src_offset_(src_offset), size_(size) {}
AddressableLight *AddressableSegment::get_src() const { return this->src_; }
int32_t AddressableSegment::get_src_offset() const { return this->src_offset_; }
int32_t AddressableSegment::get_size() const { return this->size_; }
int32_t AddressableSegment::get_dst_offset() const { return this->dst_offset_; }
void AddressableSegment::set_dst_offset(int32_t dst_offset) { this->dst_offset_ = dst_offset; }

}  // namespace light

ESPHOME_NAMESPACE_END

#endif  // USE_LIGHT
