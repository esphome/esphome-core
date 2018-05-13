//
//  fast_led_light_output.h
//  esphomelib
//
//  Created by Otto Winter on 13.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_LIGHT_FAST_LED_LIGHT_OUTPUT_H
#define ESPHOMELIB_LIGHT_FAST_LED_LIGHT_OUTPUT_H

#include "esphomelib/light/light_state.h"
#include "esphomelib/helpers.h"
#include "esphomelib/defines.h"

#ifdef USE_FAST_LED_LIGHT

// Avoid annoying compiler messages
#define FASTLED_INTERNAL

#include "FastLED.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace light {

class FastLEDLightOutput : public LightOutput, public Component {
 public:
  void schedule_show();

  CRGB *get_leds() const;

  CLEDController *get_controller() const;

  void set_max_refresh_rate(uint32_t interval_us);

  int get_num_leds() const;

  void prevent_writing_leds();

  void unprevent_writing_leds();

  CLEDController &add_leds(CLEDController *controller, int num_leds);

  template<ESPIChipsets CHIPSET, uint8_t DATA_PIN, uint8_t CLOCK_PIN, EOrder RGB_ORDER, uint8_t SPI_DATA_RATE>
  CLEDController &add_leds(int num_leds) {
    switch (CHIPSET) {
      case LPD8806: {
        static LPD8806Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER, SPI_DATA_RATE> c;
        return add_leds(&c, num_leds);
      }
      case WS2801: {
        static WS2801Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER, SPI_DATA_RATE> c;
        return add_leds(&c, num_leds);
      }
      case WS2803: {
        static WS2803Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER, SPI_DATA_RATE> c;
        return add_leds(&c, num_leds);
      }
      case SM16716: {
        static SM16716Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER, SPI_DATA_RATE> c;
        return add_leds(&c, num_leds);
      }
      case P9813: {
        static P9813Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER, SPI_DATA_RATE> c;
        return add_leds(&c, num_leds);
      }
      case DOTSTAR:
      case APA102: {
        static APA102Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER, SPI_DATA_RATE> c;
        return add_leds(&c, num_leds);
      }
      case SK9822: {
        static SK9822Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER, SPI_DATA_RATE> c;
        return add_leds(&c, num_leds);
      }
    }
  }

  template<ESPIChipsets CHIPSET, uint8_t DATA_PIN, uint8_t CLOCK_PIN>
  CLEDController &add_leds(int num_leds) {
    switch (CHIPSET) {
      case LPD8806: {
        static LPD8806Controller<DATA_PIN, CLOCK_PIN> c;
        return add_leds(&c, num_leds);
      }
      case WS2801: {
        static WS2801Controller<DATA_PIN, CLOCK_PIN> c;
        return add_leds(&c, num_leds);
      }
      case WS2803: {
        static WS2803Controller<DATA_PIN, CLOCK_PIN> c;
        return add_leds(&c, num_leds);
      }
      case SM16716: {
        static SM16716Controller<DATA_PIN, CLOCK_PIN> c;
        return add_leds(&c, num_leds);
      }
      case P9813: {
        static P9813Controller<DATA_PIN, CLOCK_PIN> c;
        return add_leds(&c, num_leds);
      }
      case DOTSTAR:
      case APA102: {
        static APA102Controller<DATA_PIN, CLOCK_PIN> c;
        return add_leds(&c, num_leds);
      }
      case SK9822: {
        static SK9822Controller<DATA_PIN, CLOCK_PIN> c;
        return add_leds(&c, num_leds);
      }
    }
  }

  template<ESPIChipsets CHIPSET, uint8_t DATA_PIN, uint8_t CLOCK_PIN, EOrder RGB_ORDER>
  CLEDController &add_leds(int num_leds) {
    switch (CHIPSET) {
      case LPD8806: {
        static LPD8806Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER> c;
        return add_leds(&c, num_leds);
      }
      case WS2801: {
        static WS2801Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER> c;
        return add_leds(&c, num_leds);
      }
      case WS2803: {
        static WS2803Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER> c;
        return add_leds(&c, num_leds);
      }
      case SM16716: {
        static SM16716Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER> c;
        return add_leds(&c, num_leds);
      }
      case P9813: {
        static P9813Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER> c;
        return add_leds(&c, num_leds);
      }
      case DOTSTAR:
      case APA102: {
        static APA102Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER> c;
        return add_leds(&c, num_leds);
      }
      case SK9822: {
        static SK9822Controller<DATA_PIN, CLOCK_PIN, RGB_ORDER> c;
        return add_leds(&c, num_leds);
      }
    }
  }

#ifdef SPI_DATA
  template<ESPIChipsets CHIPSET>
  CLEDController &add_leds(int num_leds) {
    return add_leds<CHIPSET, SPI_DATA, SPI_CLOCK, RGB>(num_leds);
  }

  template<ESPIChipsets CHIPSET, EOrder RGB_ORDER>
  CLEDController &add_leds(int num_leds) {
    return add_leds<CHIPSET, SPI_DATA, SPI_CLOCK, RGB_ORDER>(num_leds);
  }

  template<ESPIChipsets CHIPSET, EOrder RGB_ORDER, uint8_t SPI_DATA_RATE>
  CLEDController &add_leds(int num_leds) {
    return add_leds<CHIPSET, SPI_DATA, SPI_CLOCK, RGB_ORDER, SPI_DATA_RATE>(num_leds);
  }
#endif

#ifdef FASTLED_HAS_CLOCKLESS
  template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN, EOrder RGB_ORDER>
  CLEDController &add_leds(int num_leds) {
    static CHIPSET<DATA_PIN, RGB_ORDER> c;
    return add_leds(&c, num_leds);
  }

  template<template<uint8_t DATA_PIN, EOrder RGB_ORDER> class CHIPSET, uint8_t DATA_PIN>
  CLEDController &add_leds(int num_leds) {
    static CHIPSET<DATA_PIN, RGB> c;
    return add_leds(&c, num_leds);
  }

  template<template<uint8_t DATA_PIN> class CHIPSET, uint8_t DATA_PIN>
  CLEDController &add_leds(int num_leds) {
    static CHIPSET<DATA_PIN> c;
    return add_leds(&c, num_leds);
  }

#ifdef FASTSPI_USE_DMX_SIMPLE
  template<EClocklessChipsets CHIPSET, uint8_t DATA_PIN, EOrder RGB_ORDER=RGB>
  CLEDController &add_leds(int num_leds) {
    switch(CHIPSET) {
      case DMX: { static DMXController<DATA_PIN> controller; return add_leds(&controller, num_leds); }
    }
  }
#endif
#endif

  template<template<EOrder RGB_ORDER> class CHIPSET, EOrder RGB_ORDER>
  CLEDController &add_leds(int num_leds) {
    static CHIPSET<RGB_ORDER> c;
    return add_leds(&c, num_leds);
  }

  template<template<EOrder RGB_ORDER> class CHIPSET>
  CLEDController &add_leds(int num_leds) {
    static CHIPSET<RGB> c;
    return add_leds(&c, num_leds);
  }

#ifdef USE_OCTOWS2811
  template<OWS2811 CHIPSET, EOrder RGB_ORDER>
  CLEDController &add_leds(int num_leds) {
    switch(CHIPSET) {
      case OCTOWS2811: { static COctoWS2811Controller<RGB_ORDER,WS2811_800kHz> controller; return add_leds(&controller, num_leds); }
      case OCTOWS2811_400: { static COctoWS2811Controller<RGB_ORDER,WS2811_400kHz> controller; return add_leds(&controller, num_leds); }
#ifdef WS2813_800kHz
        case OCTOWS2813: { static COctoWS2811Controller<RGB_ORDER,WS2813_800kHz> controller; return add_leds(&controller, num_leds); }
#endif
    }
  }

  template<OWS2811 CHIPSET>
  CLEDController &add_leds(int num_leds) {
    return add_leds<CHIPSET, GRB>(num_leds);
  }
#endif

#ifdef USE_WS2812SERIAL
  template<SWS2812 CHIPSET, int DATA_PIN, EOrder RGB_ORDER>
  CLEDController &add_leds(int num_leds) {
    static CWS2812SerialController<DATA_PIN, RGB_ORDER> controller;
    return add_leds(&controller, num_leds);
  }
#endif

#ifdef SmartMatrix_h
  template<ESM CHIPSET>
  CLEDController &add_leds(int num_leds) {
    switch (CHIPSET) {
      case SMART_MATRIX: {
        static CSmartMatrixController controller;
        return add_leds(&controller, num_leds);
      }
    }
  }
#endif

#ifdef FASTLED_HAS_BLOCKLESS
  template<EBlockChipsets CHIPSET, int NUM_LANES, EOrder RGB_ORDER>
  CLEDController &add_leds(int num_leds) {
      switch(CHIPSET) {
#ifdef PORTA_FIRST_PIN
        case WS2811_PORTA:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTA_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER>(), num_leds);
        case WS2811_400_PORTA:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTA_FIRST_PIN, NS(800), NS(800), NS(900), RGB_ORDER>(), num_leds);
        case WS2813_PORTA:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTA_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER, 0, false, 300>(), num_leds);
        case TM1803_PORTA:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTA_FIRST_PIN, NS(700), NS(1100), NS(700), RGB_ORDER>(), num_leds);
        case UCS1903_PORTA:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTA_FIRST_PIN, NS(500), NS(1500), NS(500), RGB_ORDER>(), num_leds);
#endif
#ifdef PORTB_FIRST_PIN
        case WS2811_PORTB:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTB_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER>(), num_leds);
        case WS2811_400_PORTB:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTB_FIRST_PIN, NS(800), NS(800), NS(900), RGB_ORDER>(), num_leds);
        case WS2813_PORTB:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTB_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER, 0, false, 300>(), num_leds);
        case TM1803_PORTB:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTB_FIRST_PIN, NS(700), NS(1100), NS(700), RGB_ORDER>(), num_leds);
        case UCS1903_PORTB:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTB_FIRST_PIN, NS(500), NS(1500), NS(500), RGB_ORDER>(), num_leds);
#endif
#ifdef PORTC_FIRST_PIN
        case WS2811_PORTC:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTC_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER>(), num_leds);
        case WS2811_400_PORTC:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTC_FIRST_PIN, NS(800), NS(800), NS(900), RGB_ORDER>(), num_leds);
        case WS2813_PORTC:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTC_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER, 0, false, 300>(), num_leds);
        case TM1803_PORTC:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTC_FIRST_PIN, NS(700), NS(1100), NS(700), RGB_ORDER>(), num_leds);
        case UCS1903_PORTC:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTC_FIRST_PIN, NS(500), NS(1500), NS(500), RGB_ORDER>(), num_leds);
#endif
#ifdef PORTD_FIRST_PIN
        case WS2811_PORTD:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTD_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER>(), num_leds);
        case WS2811_400_PORTD:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTD_FIRST_PIN, NS(800), NS(800), NS(900), RGB_ORDER>(), num_leds);
        case WS2813_PORTD:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTD_FIRST_PIN, NS(320), NS(320), NS(640), RGB_ORDER, 0, false, 300>(), num_leds);
        case TM1803_PORTD:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTD_FIRST_PIN, NS(700), NS(1100), NS(700), RGB_ORDER>(), num_leds);
        case UCS1903_PORTD:
          return add_leds(new InlineBlockClocklessController<NUM_LANES, PORTD_FIRST_PIN, NS(500), NS(1500), NS(500), RGB_ORDER>(), num_leds);
#endif
#ifdef HAS_PORTDC
        case WS2811_PORTDC:
          return add_leds(new SixteenWayInlineBlockClocklessController<NUM_LANES,NS(320), NS(320), NS(640), RGB_ORDER>(), num_leds);
        case WS2811_400_PORTDC:
          return add_leds(new SixteenWayInlineBlockClocklessController<NUM_LANES,NS(800), NS(800), NS(900), RGB_ORDER>(), num_leds);
        case WS2813_PORTDC:
          return add_leds(new SixteenWayInlineBlockClocklessController<NUM_LANES, NS(320), NS(320), NS(640), RGB_ORDER, 0, false, 300>(), num_leds);
        case TM1803_PORTDC:
          return add_leds(new SixteenWayInlineBlockClocklessController<NUM_LANES, NS(700), NS(1100), NS(700), RGB_ORDER>(), num_leds);
        case UCS1903_PORTDC:
          return add_leds(new SixteenWayInlineBlockClocklessController<NUM_LANES, NS(500), NS(1500), NS(500), RGB_ORDER>(), num_leds);
#endif
      }
  }

  template<EBlockChipsets CHIPSET, int NUM_LANES>
  CLEDController &add_leds(int num_leds) {
      return add_leds<CHIPSET,NUM_LANES,GRB>(num_leds);
  }
#endif

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  LightTraits get_traits() override;
  void write_state(LightState *state) override;
  void setup() override;
  void loop() override;
  float get_setup_priority() const override;

 protected:
  CLEDController *controller_{nullptr};
  CRGB *leds_{nullptr};
  int num_leds_{0};
  uint32_t last_refresh_{0};
  Optional<uint32_t> max_refresh_rate_{};
  bool prevent_writing_leds_{false};
  bool next_show_{true};
};

} // namespace light

ESPHOMELIB_NAMESPACE_END

#endif //USE_FAST_LED_LIGHT

#endif //ESPHOMELIB_LIGHT_FAST_LED_LIGHT_OUTPUT_H
