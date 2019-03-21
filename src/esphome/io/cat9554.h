#ifndef ESPHOME_CAT9554_H
#define ESPHOME_CAT9554_H

#include "esphome/defines.h"

#ifdef USE_CAT9554

#include "esphome/component.h"
#include "esphome/esphal.h"
#include "esphome/i2c_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace io {

enum CAT9554GPIOMode {
  CAT9554_INPUT = INPUT,
  CAT9554_OUTPUT = OUTPUT,
};

enum CAT9554Commands {
  INPUT_REG = 0x00,
  OUTPUT_REG = 0x01,
  CONFIG_REG = 0x03,
};

class CAT9554GPIOInputPin;
class CAT9554GPIOOutputPin;

class CAT9554Component : public Component, public I2CDevice {
 public:
  CAT9554Component(I2CComponent *parent, uint8_t address, uint8_t irq);

  CAT9554GPIOInputPin make_input_pin(uint8_t pin, bool inverted = false);

  CAT9554GPIOOutputPin make_output_pin(uint8_t pin, bool inverted = false);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Check i2c availability and setup masks
  void setup() override;
  /// Helper function to read the value of a pin.
  bool digital_read(uint8_t pin);
  /// Helper function to write the value of a pin.
  void digital_write(uint8_t pin, bool value);
  /// Helper function to set the pin mode of a pin.
  void pin_mode(uint8_t pin, uint8_t mode);

  void update_gpio_needed(bool needed) {
    this->update_gpio_ = needed;
  };

  float get_setup_priority() const override;

  void dump_config() override;

 protected:
  uint8_t irq_;
  GPIOInputPin *pin_;
  ISRInternalGPIOPin *isr_;
  bool update_gpio_;
  bool read_gpio_();
  bool write_gpio_();
  bool config_gpio_();
  bool read_config_();

  uint8_t input_mask_{0x00};
  uint8_t output_mask_{0x00};
  uint8_t config_mask_{0x00};
};

class CAT9554GPIOInputPin : public GPIOInputPin {
 public:
  CAT9554GPIOInputPin(CAT9554Component *parent, uint8_t pin, uint8_t mode, bool inverted = false);

  GPIOPin *copy() const override;

  void setup() override;
  void pin_mode(uint8_t mode) override;
  bool digital_read() override;
  void digital_write(bool value) override;

 protected:
  CAT9554Component *parent_;
};

class CAT9554GPIOOutputPin : public GPIOOutputPin {
 public:
  CAT9554GPIOOutputPin(CAT9554Component *parent, uint8_t pin, uint8_t mode, bool inverted = false);

  GPIOPin *copy() const override;

  void setup() override;
  void pin_mode(uint8_t mode) override;
  bool digital_read() override;
  void digital_write(bool value) override;

 protected:
  CAT9554Component *parent_;
};

}  // namespace io

ESPHOME_NAMESPACE_END

#endif  // USE_CAT9554

#endif  // ESPHOME_CAT9554_H
