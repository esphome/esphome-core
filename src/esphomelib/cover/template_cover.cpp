//
//  template_cover.cpp
//  esphomelib
//
//  Created by Otto Winter on 20.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/cover/template_cover.h"

#ifdef USE_TEMPLATE_COVER

ESPHOMELIB_NAMESPACE_BEGIN

namespace cover {

TemplateCover::TemplateCover(const std::string &name)
    : Cover(name) {

}
void TemplateCover::open() {
  this->open_action_.play(false);
  if (this->optimistic_)
    this->publish_state(COVER_OPEN);
}
void TemplateCover::close() {
  this->close_action_.play(false);
  if (this->optimistic_)
    this->publish_state(COVER_CLOSED);
}
void TemplateCover::stop() {
  this->stop_action_.play(false);
}
void TemplateCover::add_open_actions(const std::vector<Action<NoArg> *> &actions) {
  this->open_action_.add_actions(actions);
}
void TemplateCover::add_close_actions(const std::vector<Action<NoArg> *> &actions) {
  this->close_action_.add_actions(actions);
}
void TemplateCover::add_stop_actions(const std::vector<Action<NoArg> *> &actions) {
  this->stop_action_.add_actions(actions);
}
void TemplateCover::loop() {
  if (this->f_.has_value()) {
    auto s = (*this->f_)();
    if (s.has_value())
      this->publish_state(*s);
  }
}
void TemplateCover::set_optimistic(bool optimistic) {
  this->optimistic_ = optimistic;
}
bool TemplateCover::optimistic() {
  return this->optimistic_;
}
void TemplateCover::set_state_lambda(std::function<optional<CoverState>()> &&f) {
  this->f_ = f;
}

} // namespace cover

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_COVER
