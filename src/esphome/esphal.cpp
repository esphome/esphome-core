#include "esphome/esphal.h"
#include "esphome/helpers.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

static const char *TAG = "esphal";

GPIOPin::GPIOPin(uint8_t pin, uint8_t mode, bool inverted)
  : pin_(pin), mode_(mode), inverted_(inverted),
#ifdef ARDUINO_ARCH_ESP8266
    gpio_read_(pin < 16 ? &GPI : &GP16I),
    gpio_mask_(pin < 16 ? (1UL << pin) : 1)
#endif
#ifdef ARDUINO_ARCH_ESP32
    gpio_set_(pin < 32 ? &GPIO.out_w1ts : &GPIO.out1_w1ts.val),
    gpio_clear_(pin < 32 ? &GPIO.out_w1tc : &GPIO.out1_w1tc.val),
    gpio_read_(pin < 32 ? &GPIO.in : &GPIO.in1.val),
    gpio_mask_(pin < 32 ? (1UL << pin) : (1UL << (pin - 32)))
#endif
  {

}

const char *GPIOPin::get_pin_mode_name() const {
  const char *mode_s;
  switch (this->mode_) {
    case INPUT: mode_s = "INPUT"; break;
    case OUTPUT: mode_s = "OUTPUT"; break;
    case INPUT_PULLUP: mode_s = "INPUT_PULLUP"; break;
    case OUTPUT_OPEN_DRAIN: mode_s = "OUTPUT_OPEN_DRAIN"; break;
    case SPECIAL: mode_s = "SPECIAL"; break;
    case FUNCTION_1: mode_s = "FUNCTION_1"; break;
    case FUNCTION_2: mode_s = "FUNCTION_2"; break;
    case FUNCTION_3: mode_s = "FUNCTION_3"; break;
    case FUNCTION_4: mode_s = "FUNCTION_4"; break;

#ifdef ARDUINO_ARCH_ESP32
    case PULLUP: mode_s = "PULLUP"; break;
    case PULLDOWN: mode_s = "PULLDOWN"; break;
    case INPUT_PULLDOWN: mode_s = "INPUT_PULLDOWN"; break;
    case OPEN_DRAIN: mode_s = "OPEN_DRAIN"; break;
    case FUNCTION_5: mode_s = "FUNCTION_5"; break;
    case FUNCTION_6: mode_s = "FUNCTION_6"; break;
    case ANALOG: mode_s = "ANALOG"; break;
#endif
#ifdef ARDUINO_ARCH_ESP8266
    case FUNCTION_0: mode_s = "FUNCTION_0"; break;
    case WAKEUP_PULLUP: mode_s = "WAKEUP_PULLUP"; break;
    case WAKEUP_PULLDOWN: mode_s = "WAKEUP_PULLDOWN"; break;
    case INPUT_PULLDOWN_16: mode_s = "INPUT_PULLDOWN_16"; break;
#endif

    default: mode_s = "UNKNOWN"; break;
  }

  return mode_s;
}

unsigned char GPIOPin::get_pin() const {
  return this->pin_;
}
unsigned char GPIOPin::get_mode() const {
  return this->mode_;
}

bool GPIOPin::is_inverted() const {
  return this->inverted_;
}
void GPIOPin::setup() {
  this->pin_mode(this->mode_);
}
bool ICACHE_RAM_ATTR HOT GPIOPin::digital_read() {
  return bool((*this->gpio_read_) & this->gpio_mask_) != this->inverted_;
}
void ICACHE_RAM_ATTR HOT GPIOPin::digital_write(bool value) {
#ifdef ARDUINO_ARCH_ESP8266
  if (this->pin_ != 16) {
    if (value != this->inverted_) {
      GPOS = this->gpio_mask_;
    } else {
      GPOC = this->gpio_mask_;
    }
  } else {
    if (value != this->inverted_) {
      GP16O |= 1;
    } else {
      GP16O &= ~1;
    }
  }
#endif
#ifdef ARDUINO_ARCH_ESP32
  if (value != this->inverted_) {
    (*this->gpio_set_) = this->gpio_mask_;
  } else {
    (*this->gpio_clear_) = this->gpio_mask_;
  }
#endif
}
GPIOPin *GPIOPin::copy() const { return new GPIOPin(*this); }

void ICACHE_RAM_ATTR HOT GPIOPin::pin_mode(uint8_t mode) {
  pinMode(this->pin_, mode);
}

GPIOOutputPin::GPIOOutputPin(uint8_t pin, uint8_t mode, bool inverted)
    : GPIOPin(pin, mode, inverted) {}

GPIOInputPin::GPIOInputPin(uint8_t pin, uint8_t mode, bool inverted)
    : GPIOPin(pin, mode, inverted) {}

ESPHOME_NAMESPACE_END
