#ifndef ESPHOMELIB_STATUS_LED_H
#define ESPHOMELIB_STATUS_LED_H

#include "esphomelib/component.h"
#include "esphomelib/esphal.h"
#include "esphomelib/defines.h"

#ifdef USE_STATUS_LED

ESPHOMELIB_NAMESPACE_BEGIN

class StatusLEDComponent : public Component {
 public:
  explicit StatusLEDComponent(GPIOPin *pin);

  void setup() override;
  void loop() override;
  float get_setup_priority() const override;
  float get_loop_priority() const override;

 protected:
  GPIOPin *pin_;
};

extern StatusLEDComponent *global_status_led;

ESPHOMELIB_NAMESPACE_END

#endif //USE_STATUS_LED

#endif //ESPHOMELIB_STATUS_LED_H
