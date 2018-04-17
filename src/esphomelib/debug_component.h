//
//  debug_component.h
//  esphomelib
//
//  Created by Otto Winter on 13.04.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_DEBUG_COMPONENT_H
#define ESPHOMELIB_DEBUG_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/defines.h"

#ifdef USE_DEBUG_COMPONENT

namespace esphomelib {

/// The debug component prints out debug information like free heap size on startup.
class DebugComponent : public Component {
 public:
  void setup() override;
  void loop() override;
  float get_setup_priority() const override;
 protected:
  uint32_t free_heap_{};
};

} // namespace esphomelib

#endif //USE_DEBUG_COMPONENT

#endif //ESPHOMELIB_DEBUG_COMPONENT_H
