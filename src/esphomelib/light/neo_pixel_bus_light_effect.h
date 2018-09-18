//
//  neo_pixel_bus_light_effect.h
//  esphomelib
//
//  Created by Patrick Huy on 05.09.18.
//  color_to_setright © 2018 Patrick Huy. Some rights reserved.
//

#ifndef ESPHOMELIB_NEO_PIXEL_BUS_LIGHT_EFFECT_H
#define ESPHOMELIB_NEO_PIXEL_BUS_LIGHT_EFFECT_H

#include "esphomelib/defines.h"

#ifdef USE_NEO_PIXEL_BUS_LIGHT

#include "esphomelib/light/light_effect.h"
#include "esphomelib/light/neo_pixel_bus_light_output.h"

#include "NeoPixelAnimator.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light
{

template <typename T_COLOR_FEATURE, typename T_METHOD>
class BaseNeoPixelBusLightEffect : public LightEffect
{
public:
  explicit BaseNeoPixelBusLightEffect(const std::string &name, uint16_t countAnimations,
    uint16_t timeScale = NEO_MILLISECONDS) : LightEffect(name), animations_(NeoPixelAnimator(countAnimations, timeScale)) {

  }
  void start_() override
  {
    this->get_neo_pixel_bus_output_()->prevent_writing_leds();
    this->start();
  }
  void stop() override
  {
    this->get_neo_pixel_bus_output_()->unprevent_writing_leds();
    this->animations_.StopAll();
  }

  virtual void apply(NeoPixelBusLightOutputComponent<T_COLOR_FEATURE, T_METHOD> &output, uint8_t brightness, typename T_COLOR_FEATURE::ColorObject color) {

  }

  void apply() override
  {
    float brightness_f;
    auto state = this->state_;
    const auto values = state->get_remote_values_lazy();
    values.as_brightness(&brightness_f);
    this->color_ = this->get_neo_pixel_bus_output_()->template get_light_color<typename T_COLOR_FEATURE::ColorObject>(state, values);
    this->apply(*this->get_neo_pixel_bus_output_(), brightness_f, this->color_);
    this->animations_.UpdateAnimations();
    this->get_neo_pixel_bus_output_()->schedule_show();
  }

protected:
  NeoPixelAnimator animations_;
  typename T_COLOR_FEATURE::ColorObject color_;
  NeoPixelBusLightOutputComponent<T_COLOR_FEATURE, T_METHOD> *get_neo_pixel_bus_output_() const
  {
    return static_cast<NeoPixelBusLightOutputComponent<T_COLOR_FEATURE, T_METHOD> *>(this->state_->get_output());
  }
  NeoPixelBus<T_COLOR_FEATURE, T_METHOD> *get_controller_() const
  {
    return get_neo_pixel_bus_output_()->getcontroller_();
  }
};

template <typename T_COLOR_FEATURE, typename T_METHOD>
class NeoPixelBusLoop : public BaseNeoPixelBusLightEffect<T_COLOR_FEATURE, T_METHOD>
{
public:
  NeoPixelBusLoop(const std::string &name, const uint16_t duration = 5000, const int looplength = 10) : BaseNeoPixelBusLightEffect<T_COLOR_FEATURE, T_METHOD>(name, 1), duration_(duration), looplength_(looplength) {
  }

  virtual void start() override
  {
    using namespace std::placeholders;

    // setup aniamtion
    this->animations_.StartAnimation(0, this->duration_, std::bind(&NeoPixelBusLoop<T_COLOR_FEATURE,T_METHOD>::update_pixel_loop, this, _1));
  }
private:
  const AnimEaseFunction moveEase = NeoEase::Linear;
  const typename T_COLOR_FEATURE::ColorObject off = typename T_COLOR_FEATURE::ColorObject(0,0,0);
  uint16_t duration_;
  int looplength_;

  void update_pixel_loop(const AnimationParam &param)
  {
    float progress = moveEase(param.progress);

    auto pixel_count = this->get_controller_()->PixelCount();
    uint16_t next_pixel = progress * pixel_count;

    uint16_t stop_pixel = next_pixel + this->looplength_;
    this->get_controller_()->ClearTo(off);
    uint8_t darken_amount_per_step = 255 / this->looplength_;
    for (uint16_t i = next_pixel; i < stop_pixel; i++)
    {
      uint16_t pixel_to_set = i;
      if (pixel_to_set > pixel_count) {
        pixel_to_set -= pixel_count;
      }
      int trail_length = stop_pixel - i;
      auto color_to_set = this->color_;
      color_to_set.Darken(trail_length * darken_amount_per_step);
      this->get_controller_()->SetPixelColor(pixel_to_set, color_to_set);

    }

    if (param.state == AnimationState_Completed)
    {
      // one loop completed, restart
      this->animations_.RestartAnimation(param.index);
    }
  }
};

}

ESPHOMELIB_NAMESPACE_END

#endif //USE_FAST_LED_LIGHT

#endif //ESPHOMELIB_FAST_LED_LIGHT_EFFECT_H
