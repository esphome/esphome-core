#include "esphomelib/defines.h"

#ifdef USE_CUSTOM_COMPONENT

#include "esphomelib/custom_component.h"
#include "esphomelib/application.h"

ESPHOMELIB_NAMESPACE_BEGIN

CustomComponentConstructor::CustomComponentConstructor(const std::function<std::vector<Component *>()> &init) {
  this->components_ = init();

  for (auto *comp : this->components_) {
    App.register_component(comp);
  }
}

Component *CustomComponentConstructor::get_component(int i) {
  return this->components_[i];
}

ESPHOMELIB_NAMESPACE_END

#endif //USE_CUSTOM_COMPONENT

