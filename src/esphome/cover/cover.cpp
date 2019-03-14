#include "esphome/defines.h"

#ifdef USE_COVER

#include "esphome/cover/cover.h"

ESPHOME_NAMESPACE_BEGIN

namespace cover {

Cover::Cover(const std::string &name) : Nameable(name) {}

void Cover::add_on_publish_state_callback(std::function<void(CoverState)> &&f) {
  this->state_callback_.add(std::move(f));
}
void Cover::publish_state(CoverState state) {
  if (!this->dedup_.next(state))
    return;
  this->state = state;
  this->state_callback_.call(state);
}
void Cover::add_on_publish_position_callback(std::function<void(position_value_t)> &&p) {
  this->position_callback_.add(std::move(p));
}
void Cover::publish_position(position_value_t position_value) {
  this->position_value = position_value;
  this->position_callback_.call(position_value);
}
void Cover::add_on_publish_tilt_callback(std::function<void(tilt_value_t)> &&t) {
  this->tilt_callback_.add(std::move(t));
}
void Cover::publish_tilt(tilt_value_t tilt_value) {
  this->tilt_value = tilt_value;
  this->tilt_callback_.call(tilt_value);
}
bool Cover::assumed_state() { return false; }
void Cover::set_optimistic(bool optimistic) { this->optimistic_ = optimistic; }
bool Cover::optimistic() {return this->optimistic_; }
bool Cover::has_state() const { return this->dedup_.has_value(); }
void Cover::set_position_open(float position_open) { this->position_open_ = position_open; }
float Cover::position_open() { return this->position_open_; }
void Cover::set_position_closed(float position_closed) { this->position_closed_ = position_closed; }
float Cover::position_closed() { return this->position_closed_; }
void Cover::set_tilt_min(float tilt_min) { this->tilt_min_ = tilt_min; }
float Cover::tilt_min() { return this->tilt_min_; }
void Cover::set_tilt_max(float tilt_max) { this->tilt_max_ = tilt_max; }
float Cover::tilt_max() { return this->tilt_max_; }
void Cover::set_tilt_closed_value(float tilt_closed_value) { this->tilt_closed_value_ = tilt_closed_value; }
float Cover::tilt_closed_value() { return this->tilt_closed_value_; }
void Cover::set_tilt_opened_value(float tilt_opened_value) { this->tilt_opened_value_ = tilt_opened_value; }
float Cover::tilt_opened_value() { return this->tilt_opened_value_; }
void Cover::set_tilt_invert_state(bool tilt_invert_state) { this->tilt_invert_state_ = tilt_invert_state; }
bool Cover::tilt_invert_state() { return this->tilt_invert_state_; }

void Cover::open() { this->write_command(COVER_COMMAND_OPEN); }
void Cover::close() { this->write_command(COVER_COMMAND_CLOSE); }
void Cover::stop() { this->write_command(COVER_COMMAND_STOP); }

void Cover::set_position(float value) {
  this->position_value = value;
  this->write_command(COVER_COMMAND_POSITION, value);
}
void Cover::set_tilt(float value) {
  this->tilt_value = value;
  this->write_command(COVER_COMMAND_TILT, value);
}
uint32_t Cover::hash_base() { return 1727367479UL; }
#ifdef USE_MQTT_COVER
MQTTCoverComponent *Cover::get_mqtt() const { return this->mqtt_; }
void Cover::set_mqtt(MQTTCoverComponent *mqtt) { this->mqtt_ = mqtt; }
#endif

}  // namespace cover

ESPHOME_NAMESPACE_END

#endif  // USE_COVER
