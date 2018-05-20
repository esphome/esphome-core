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
  TemplateCover(const std::string &name, std::function<optional<CoverState>()> &&f);

  void add_open_actions(const std::vector<Action<NoArg> *> &actions);
  void add_close_actions(const std::vector<Action<NoArg> *> &actions);
  void add_stop_actions(const std::vector<Action<NoArg> *> &actions);

  void loop() override;

  void open() override;
  void close() override;
  void stop() override;

 protected:
  std::function<optional<CoverState>()> f_;
  ActionList<NoArg> open_action_;
  ActionList<NoArg> close_action_;
  ActionList<NoArg> stop_action_;
};

} // namespace cover

ESPHOMELIB_NAMESPACE_END

#endif //USE_TEMPLATE_COVER

#endif //ESPHOMELIB_COVER_TEMPLATE_COVER_H
