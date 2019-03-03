#include "esphome/defines.h"

#ifdef USE_FAN

#include "esphome/fan/fan_traits.h"

ESPHOME_NAMESPACE_BEGIN

namespace fan {

FanTraits::FanTraits() : oscillation_(false), speed_(false) {}

FanTraits::FanTraits(bool oscillation, bool speed) : oscillation_(oscillation), speed_(speed) {}
bool FanTraits::supports_oscillation() const { return this->oscillation_; }
bool FanTraits::supports_speed() const { return this->speed_; }
void FanTraits::set_oscillation(bool oscillation) { this->oscillation_ = oscillation; }
void FanTraits::set_speed(bool speed) { this->speed_ = speed; }

}  // namespace fan

ESPHOME_NAMESPACE_END

#endif  // USE_FAN
