#ifndef ESPHOMELIB_DEBUG_COMPONENT_H
#define ESPHOMELIB_DEBUG_COMPONENT_H

#include "esphomelib/defines.h"

#ifdef USE_DEBUG_COMPONENT

#include "esphomelib/component.h"

ESPHOMELIB_NAMESPACE_BEGIN

/// The debug component prints out debug information like free heap size on startup.
class DebugComponent : public Component {
 public:
  void setup() override;
  void loop() override;
  float get_setup_priority() const override;
 protected:
  uint32_t free_heap_{};
};

ESPHOMELIB_NAMESPACE_END

#endif //USE_DEBUG_COMPONENT

#endif //ESPHOMELIB_DEBUG_COMPONENT_H
