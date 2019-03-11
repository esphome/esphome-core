#include "esphome/defines.h"

#ifdef USE_SERVO

#include "esphome/servo.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

static const char *TAG = "servo";

void Servo::write(float value) {
  if (value < -1.0)
    value = -1.0;
  else if (value > 1.0)
    value = 1.0;

  float level;
  if (value < 0.0)
    level = lerp(this->idle_level_, this->min_level_, -value);
  else
    level = lerp(this->idle_level_, this->max_level_, value);

  this->output_->set_level(level);
}
void Servo::setup() { this->write(0.0f); }
void Servo::dump_config() {
  ESP_LOGCONFIG(TAG, "Servo:");
  ESP_LOGCONFIG(TAG, "  Idle Level: %.1f%%", this->idle_level_ * 100.0f);
  ESP_LOGCONFIG(TAG, "  Min Level: %.1f%%", this->min_level_ * 100.0f);
  ESP_LOGCONFIG(TAG, "  Max Level: %.1f%%", this->max_level_ * 100.0f);
}
float Servo::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
void Servo::set_min_level(float min_level) { this->min_level_ = min_level; }
void Servo::set_idle_level(float idle_level) { this->idle_level_ = idle_level; }
void Servo::set_max_level(float max_level) { this->max_level_ = max_level; }
Servo::Servo(output::FloatOutput *output) : output_(output) {}

ESPHOME_NAMESPACE_END

#endif  // USE_SERVO
