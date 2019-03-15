#include "esphome/defines.h"

#ifdef USE_TOTAL_DAILY_ENERGY_SENSOR

#include "esphome/sensor/total_daily_energy.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.total_daily_energy";

void TotalDailyEnergy::setup() {
  this->pref_ = global_preferences.make_preference<float>(this->get_object_id_hash());

  float recovered;
  if (this->pref_.load(&recovered)) {
    this->publish_state_and_save(recovered);
  } else {
    this->publish_state_and_save(0);
  }
  this->last_update_ = millis();

  auto f = std::bind(&TotalDailyEnergy::process_new_state_, this, std::placeholders::_1);
  this->parent_->add_on_state_callback(f);
}
void TotalDailyEnergy::dump_config() { LOG_SENSOR("", "Total Daily Energy", this); }
float TotalDailyEnergy::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
uint32_t TotalDailyEnergy::update_interval() { return this->parent_->update_interval(); }
std::string TotalDailyEnergy::unit_of_measurement() { return this->parent_->get_unit_of_measurement() + "h"; }
std::string TotalDailyEnergy::icon() { return this->parent_->get_icon(); }
int8_t TotalDailyEnergy::accuracy_decimals() { return this->parent_->get_accuracy_decimals() + 2; }
void TotalDailyEnergy::process_new_state_(float state) {
  if (isnan(state))
    return;
  const uint32_t now = millis();
  float delta_hours = (now - this->last_update_) / 1000.0f / 60.0f / 60.0f;
  this->last_update_ = now;
  this->publish_state_and_save(this->total_energy_ + state * delta_hours);
}
void TotalDailyEnergy::loop() {
  auto t = this->time_->now();
  if (!t.is_valid())
    return;

  if (this->last_day_of_year_ == 0) {
    this->last_day_of_year_ = t.day_of_year;
    return;
  }

  if (t.day_of_year != this->last_day_of_year_) {
    this->last_day_of_year_ = t.day_of_year;
    this->total_energy_ = 0;
    this->publish_state_and_save(0);
  }
}
void TotalDailyEnergy::publish_state_and_save(float state) {
  this->pref_.save(&state);
  this->total_energy_ = state;
  this->publish_state(state);
}
TotalDailyEnergy::TotalDailyEnergy(const std::string &name, time::RealTimeClockComponent *time, Sensor *parent)
    : Sensor(name), time_(time), parent_(parent) {}

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TOTAL_DAILY_ENERGY_SENSOR
