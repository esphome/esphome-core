#ifndef ESPHOME_STATUS_LED_H
#define ESPHOME_STATUS_LED_H

#include "esphome/defines.h"

#ifdef USE_STATUS_LED

#include "esphome/component.h"
#include "esphome/esphal.h"

ESPHOME_NAMESPACE_BEGIN

class StatusLEDComponent : public Component {
 public:
  explicit StatusLEDComponent(GPIOPin *pin);

  void setup() override;
  void dump_config() override;
  void loop() override;
  float get_setup_priority() const override;
  float get_loop_priority() const override;

 protected:
  GPIOPin *pin_;
};

extern StatusLEDComponent *global_status_led;

ESPHOME_NAMESPACE_END

#endif  // USE_STATUS_LED

#endif  // ESPHOME_STATUS_LED_H
