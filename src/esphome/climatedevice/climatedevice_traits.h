#ifndef ESPHOME_CLIMATEDEVICE_CLIMATEDEVICE_TRAITS_H
#define ESPHOME_CLIMATEDEVICE_CLIMATEDEVICE_TRAITS_H

#include "esphome/defines.h"

#ifdef USE_CLIMATEDEVICE

ESPHOME_NAMESPACE_BEGIN

namespace climatedevice {

/// This class represents the capabilities/feature set of a climatedevice.
class ClimateDeviceTraits {
 public:
  /// Construct an empty ClimateDeviceTraits object. All features will be marked unsupported, min_target_temperature=10
  /// and max_target_temperature=30.
  ClimateDeviceTraits();
  /// Construct a ClimateDeviceTraits object with the provided mode support and min/max target temperature.
  ClimateDeviceTraits(bool current_temperature, bool auto_mode, bool cool_mode, bool heat_mode,
                      float min_target_temperature, float max_target_temperature);

  /// Return if this climatedevice component supports current temperature.
  bool supports_current_temperature() const;
  /// Set whether this climatedevice component supports current temperature.
  void set_current_temperature(bool current_temperature);
  /// Return if this climatedevice component supports auto mode.
  bool supports_auto_mode() const;
  /// Set whether this climatedevice component supports auto mode.
  void set_auto_mode(bool auto_mode);
  /// Return if this climatedevice component supports cool mode.
  bool supports_cool_mode() const;
  /// Set whether this climatedevice component supports cool mode.
  void set_cool_mode(bool cool_mode);
  /// Return if this climatedevice component supports heat mode.
  bool supports_heat_mode() const;
  /// Set whether this climatedevice component supports heat mode.
  void set_heat_mode(bool heat_mode);
  /// Get minimal allowed target temperature.
  float get_min_target_temperature() const;
  /// Set minimal allowed target temperature.
  void set_min_target_temperature(float min_target_temperature);
  /// Get maximal allowed target temperature.
  float get_max_target_temperature() const;
  /// Set maximal allowed target temperature.
  void set_max_target_temperature(float max_target_temperature);

 protected:
  bool current_temperature_;
  bool auto_mode_;
  bool cool_mode_;
  bool heat_mode_;
  float min_target_temperature_;
  float max_target_temperature_;
};

}  // namespace climatedevice

ESPHOME_NAMESPACE_END

#endif  // USE_CLIMATEDEVICE

#endif  // ESPHOME_CLIMATEDEVICE_CLIMATEDEVICE_TRAITS_H
