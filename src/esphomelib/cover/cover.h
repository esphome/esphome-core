//
//  cover.h
//  esphomelib
//
//  Created by Otto Winter on 20.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_COVER_COVER_H
#define ESPHOMELIB_COVER_COVER_H

#include "esphomelib/component.h"
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"

#ifdef USE_COVER

ESPHOMELIB_NAMESPACE_BEGIN

namespace cover {

enum CoverState {
  COVER_OPEN = 0,
  COVER_CLOSED,
  COVER_MAX
};

class Cover : public Nameable {
 public:
  explicit Cover(const std::string &name);

  virtual void open() = 0;
  virtual void close() = 0;
  virtual void stop() = 0;

  void add_on_publish_state_callback(std::function<void(CoverState)> &&f);

  void publish_state(CoverState state);

 protected:
  CoverState last_state_{COVER_MAX};
  CallbackManager<void(CoverState)> state_callback_{};
};

} // namespace cover

ESPHOMELIB_NAMESPACE_END

#endif //USE_COVER

#endif //ESPHOMELIB_COVER_COVER_H
