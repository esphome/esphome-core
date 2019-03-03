#ifndef ESPHOME_FAN_FAN_TRAITS_H
#define ESPHOME_FAN_FAN_TRAITS_H

#include "esphome/defines.h"

#ifdef USE_FAN

ESPHOME_NAMESPACE_BEGIN

namespace fan {

/// This class represents the capabilities/feature set of a fan.
class FanTraits {
 public:
  /// Construct an empty FanTraits object. All features will be marked unsupported.
  FanTraits();
  /// Construct a FanTraits object with the provided oscillation and speed support.
  FanTraits(bool oscillation, bool speed);

  /// Return if this fan supports oscillation.
  bool supports_oscillation() const;
  /// Set whether this fan supports oscillation.
  void set_oscillation(bool oscillation);
  /// Return if this fan supports speed modes.
  bool supports_speed() const;
  /// Set whether this fan supports speed modes.
  void set_speed(bool speed);

 protected:
  bool oscillation_;
  bool speed_;
};

}  // namespace fan

ESPHOME_NAMESPACE_END

#endif  // USE_FAN

#endif  // ESPHOME_FAN_FAN_TRAITS_H
