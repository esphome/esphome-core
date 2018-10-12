//
//  cover.cpp
//  esphomelib
//
//  Created by Otto Winter on 20.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/defines.h"

#ifdef USE_COVER

#include "esphomelib/cover/cover.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace cover {

Cover::Cover(const std::string &name) : Nameable(name) {}
void Cover::add_on_publish_state_callback(std::function<void(CoverState)> &&f) {
  this->state_callback_.add(std::move(f));
}
void Cover::publish_state(CoverState state) {
  if (this->state == state)
    return;
  this->has_value_ = true;
  this->state = state;
  this->state_callback_.call(state);
}
bool Cover::optimistic() {
  return false;
}
bool Cover::has_value() const {
  return this->has_value_;
}

} // namespace cover

ESPHOMELIB_NAMESPACE_END

#endif //USE_COVER
