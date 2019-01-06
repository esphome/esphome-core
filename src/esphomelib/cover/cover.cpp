#include "esphomelib/defines.h"

#ifdef USE_COVER

#include "esphomelib/cover/cover.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace cover {

Cover::Cover(const std::string &name) : Nameable(name) {

}

void Cover::add_on_publish_state_callback(std::function<void(CoverState)> &&f) {
  this->state_callback_.add(std::move(f));
}
void Cover::publish_state(CoverState state) {
  this->has_state_ = true;
  this->state = state;
  this->state_callback_.call(state);
}
bool Cover::optimistic() {
  return false;
}
bool Cover::has_state() const {
  return this->has_state_;
}

void Cover::open() {
  this->write_command(COVER_COMMAND_OPEN);
}
void Cover::close() {
  this->write_command(COVER_COMMAND_CLOSE);
}
void Cover::stop() {
  this->write_command(COVER_COMMAND_STOP);
}
uint32_t Cover::hash_base_() {
  return 1727367479UL;
}

} // namespace cover

ESPHOMELIB_NAMESPACE_END

#endif //USE_COVER
