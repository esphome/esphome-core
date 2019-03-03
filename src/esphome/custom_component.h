#ifndef ESPHOME_CUSTOM_COMPONENT_H
#define ESPHOME_CUSTOM_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_CUSTOM_COMPONENT

#include "esphome/component.h"

ESPHOME_NAMESPACE_BEGIN

class CustomComponentConstructor {
 public:
  CustomComponentConstructor(const std::function<std::vector<Component *>()> &init);

  Component *get_component(int i);

 protected:
  std::vector<Component *> components_;
};

ESPHOME_NAMESPACE_END

#endif  // USE_CUSTOM_COMPONENT

#endif  // ESPHOME_CUSTOM_COMPONENT_H
