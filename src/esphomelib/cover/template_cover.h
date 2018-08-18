//
//  template_cover.h
//  esphomelib
//
//  Created by Otto Winter on 20.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_COVER_TEMPLATE_COVER_H
#define ESPHOMELIB_COVER_TEMPLATE_COVER_H

#include "esphomelib/cover/cover.h"
#include "esphomelib/automation.h"
#include "esphomelib/defines.h"

#ifdef USE_TEMPLATE_COVER

ESPHOMELIB_NAMESPACE_BEGIN

namespace cover {

class TemplateCover : public Cover, public Component {
 public:
  explicit TemplateCover(const std::string &name);

  void set_state_lambda(std::function<optional<CoverState>()> &&f);
  Trigger<NoArg> *get_open_trigger() const;
  Trigger<NoArg> *get_close_trigger() const;
  Trigger<NoArg> *get_stop_trigger() const;
  void set_optimistic(bool optimistic);

  void loop() override;

  void open() override;
  void close() override;
  void stop() override;

  float get_setup_priority() const override;

 protected:
  bool optimistic() override;

  optional<std::function<optional<CoverState>()>> f_;
  bool optimistic_{false};
  Trigger<NoArg> *open_trigger_;
  Trigger<NoArg> *close_trigger_;
  Trigger<NoArg> *stop_trigger_;
};

} // namespace cover

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_COVER

#endif //ESPHOMELIB_COVER_TEMPLATE_COVER_H
