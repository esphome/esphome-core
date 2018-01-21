//
// Created by Otto Winter on 29.12.17.
//

#ifndef ESPHOMELIB_FAN_FAN_TRAITS_H
#define ESPHOMELIB_FAN_FAN_TRAITS_H

namespace esphomelib {

namespace fan {

class FanTraits {
 public:
  FanTraits();
  FanTraits(bool oscillation, bool speed);

  bool supports_oscillation() const;
  void set_oscillation(bool oscillation);
  bool supports_speed() const;
  void set_speed(bool speed);

 protected:
  bool oscillation_;
  bool speed_;
};

} // namespace fan

} // namespace esphomelib

#endif //ESPHOMELIB_FAN_FAN_TRAITS_H
