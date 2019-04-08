#include "esphome/defines.h"

#ifdef USE_COVER

#include "esphome/cover/cover_traits.h"

ESPHOME_NAMESPACE_BEGIN

namespace cover {

bool CoverTraits::get_is_assumed_state() const { return this->is_assumed_state_; }
void CoverTraits::set_is_assumed_state(bool is_assumed_state) { this->is_assumed_state_ = is_assumed_state; }
bool CoverTraits::get_supports_position() const { return this->supports_position_; }
void CoverTraits::set_supports_position(bool supports_position) { this->supports_position_ = supports_position; }
bool CoverTraits::get_supports_tilt() const { return this->supports_tilt_; }
void CoverTraits::set_supports_tilt(bool supports_tilt) { this->supports_tilt_ = supports_tilt; }

}  // namespace cover

ESPHOME_NAMESPACE_END

#endif  // USE_COVER
