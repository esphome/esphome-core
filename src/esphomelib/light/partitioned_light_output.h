//
//  partitioned_light_output.h
//  esphomelib
//
//  Created by Patrick Huy on 03.09.18.
//  Copyright © 2018 Patrick Huy. Some rights reserved.
//
// partitioned_light_output.h
//

#ifndef ESPHOMELIB_PARTITIONED_LIGHT_OUTPUT_H
#define ESPHOMELIB_PARTITIONED_LIGHT_OUTPUT_H

#include "esphomelib/power_supply_component.h"
#include "esphomelib/light/light_state.h"
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"

#ifdef USE_NEO_PIXEL_BUS_LIGHT

#include "NeoPixelBus.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light
{

class PartitionableLightOutput
{
    public:
        virtual void write_partition(LightState *state, uint16_t index_start, uint16_t index_end) = 0;
        virtual LightTraits get_traits() = 0;
        
        void register_partition_state(LightState *state) {
            partitions_states_.push_back(state);
        }
    protected:
        std::vector<LightState *> partitions_states_;
};

/** This component allows you to address the subset of a (NeoPixelBus) LED Strip and have it act as one light.
 */
class PartitionedLightOutputComponent : public LightOutput, public Component
{
  public:
    PartitionedLightOutputComponent(PartitionableLightOutput *partitionable, LightState *master_state, uint16_t index_start, uint16_t index_end) : 
        partitionable_(partitionable), master_state_(master_state), index_start_(index_start), index_end_(index_end)
    {
    }
    LightTraits get_traits() override
    {
        return partitionable_->get_traits();
    }

    void write_state(LightState *state) override
    {
        auto current_values = master_state_->get_remote_values_lazy();
        if (state->get_current_values_lazy().get_state() > current_values.get_state()) {
            master_state_->set_immediately_without_write(LightColorValues::from_binary(true));
        }
        partitionable_->write_partition(state, index_start_, index_end_);
    }
    void setup() override
    {
    }
    void loop() override
    {
    }
    float get_setup_priority() const override
    {
        return setup_priority::POST_HARDWARE;
    }

  private:
    PartitionableLightOutput *partitionable_;
    LightState *master_state_;
    uint16_t index_start_;
    uint16_t index_end_;
};
}
ESPHOMELIB_NAMESPACE_END
#endif
#endif