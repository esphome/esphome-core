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
bool Cover::assumed_state() { return false; }
bool Cover::has_state() const { return this->dedup_.has_value(); }

void Cover::open() { this->write_command(COVER_COMMAND_OPEN); }
void Cover::close() { this->write_command(COVER_COMMAND_CLOSE); }
void Cover::stop() { this->write_command(COVER_COMMAND_STOP); }
uint32_t Cover::hash_base() { return 1727367479UL; }
#ifdef USE_MQTT_COVER
MQTTCoverComponent *Cover::get_mqtt() const { return this->mqtt_; }
void Cover::set_mqtt(MQTTCoverComponent *mqtt) { this->mqtt_ = mqtt; }
#endif

}  // namespace cover

ESPHOME_NAMESPACE_END

#endif  // USE_COVER
