#include "esphomelib/defines.h"

#ifdef USE_NEO_PIXEL_BUS_LIGHT

#include "esphomelib/light/partitioned_light_output.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

void PartitionableLightOutput::register_partition_state(LightState *state) {
  this->partitions_states_.push_back(state);
}

LightTraits PartitionedLightOutputComponent::get_traits() {
  return this->partitionable_->get_traits();
}

void PartitionedLightOutputComponent::write_state(LightState *state) {
  auto remote_values = this->master_state_->get_remote_values();
  if (state->get_current_values_lazy().get_state() > remote_values.get_state()) {
    this->master_state_->set_immediately_without_write(LightColorValues::from_binary(true));
  }
  this->partitionable_->write_partition(state, index_start_, index_end_);
}

void PartitionedLightOutputComponent::setup() {}

void PartitionedLightOutputComponent::loop() {}

float PartitionedLightOutputComponent::get_setup_priority() const {
  return setup_priority::POST_HARDWARE - 1.0f;
}

}  // namespace light
ESPHOMELIB_NAMESPACE_END
#endif  // USE_NEO_PIXEL_BUS_LIGHT
