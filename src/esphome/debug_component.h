#ifndef ESPHOME_DEBUG_COMPONENT_H
#define ESPHOME_DEBUG_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_DEBUG_COMPONENT

#include "esphome/component.h"

ESPHOME_NAMESPACE_BEGIN

/// The debug component prints out debug information like free heap size on startup.
class DebugComponent : public Component {
 public:
  void setup() override;
  void loop() override;
  float get_setup_priority() const override;
  void dump_config() override;

 protected:
  uint32_t free_heap_{};
};

ESPHOME_NAMESPACE_END

#endif  // USE_DEBUG_COMPONENT

#endif  // ESPHOME_DEBUG_COMPONENT_H
