#ifndef ESPHOMELIB_CUSTOM_COMPONENT_H
#define ESPHOMELIB_CUSTOM_COMPONENT_H

#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_COMPONENT

#include "esphomelib/component.h"

ESPHOMELIB_NAMESPACE_BEGIN

class CustomComponentConstructor {
 public:
  CustomComponentConstructor(const std::function<std::vector<Component *>()> &init);

  Component *get_component(int i);

 protected:
  std::vector<Component *> components_;
};

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_COMPONENT

#endif //ESPHOMELIB_CUSTOM_COMPONENT_H
