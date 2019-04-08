#ifndef ESPHOME_CORE_COVER_TRAITS_H
#define ESPHOME_CORE_COVER_TRAITS_H

#include "esphome/defines.h"

#ifdef USE_COVER

#include <string>

ESPHOME_NAMESPACE_BEGIN

namespace cover {

class CoverTraits {
 public:
  CoverTraits() {}
  bool get_is_assumed_state() const;
  void set_is_assumed_state(bool is_assumed_state);
  bool get_supports_position() const;
  void set_supports_position(bool supports_position);
  bool get_supports_tilt() const;
  void set_supports_tilt(bool supports_tilt);

 protected:
  bool is_assumed_state_{false};
  bool supports_position_{false};
  bool supports_tilt_{false};
};

}  // namespace cover

ESPHOME_NAMESPACE_END

#endif  // USE_COVER

#endif  // ESPHOME_CORE_COVER_TRAITS_H
