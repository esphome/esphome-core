#include "esphome/defines.h"

#ifdef USE_CLIMATEDEVICE

#include "esphome/climatedevice/climatedevice_traits.h"

ESPHOME_NAMESPACE_BEGIN

namespace climatedevice {

ClimateDeviceTraits::ClimateDeviceTraits()
    : current_temperature_(false),
      auto_mode_(false),
      cool_mode_(false),
      heat_mode_(false),
      min_target_temperature_(10),
      max_target_temperature_(30) {}

ClimateDeviceTraits::ClimateDeviceTraits(bool current_temperature, bool auto_mode, bool cool_mode, bool heat_mode,
                                         float min_target_temperature, float max_target_temperature)
    : current_temperature_(current_temperature),
      auto_mode_(auto_mode),
      cool_mode_(cool_mode),
      heat_mode_(heat_mode),
      min_target_temperature_(min_target_temperature),
      max_target_temperature_(max_target_temperature) {}
bool ClimateDeviceTraits::supports_current_temperature() const { return this->current_temperature_; }
bool ClimateDeviceTraits::supports_auto_mode() const { return this->auto_mode_; }
bool ClimateDeviceTraits::supports_cool_mode() const { return this->cool_mode_; }
bool ClimateDeviceTraits::supports_heat_mode() const { return this->heat_mode_; }
float ClimateDeviceTraits::get_min_target_temperature() const { return this->min_target_temperature_; };
float ClimateDeviceTraits::get_max_target_temperature() const { return this->max_target_temperature_; };
void ClimateDeviceTraits::set_current_temperature(bool current_temperature) {
  this->current_temperature_ = current_temperature;
}
void ClimateDeviceTraits::set_auto_mode(bool auto_mode) { this->auto_mode_ = auto_mode; }
void ClimateDeviceTraits::set_cool_mode(bool cool_mode) { this->cool_mode_ = cool_mode; }
void ClimateDeviceTraits::set_heat_mode(bool heat_mode) { this->heat_mode_ = heat_mode; }
void ClimateDeviceTraits::set_min_target_temperature(float min_target_temperature) {
  this->min_target_temperature_ = min_target_temperature;
}
void ClimateDeviceTraits::set_max_target_temperature(float max_target_temperature) {
  this->max_target_temperature_ = max_target_temperature;
}

}  // namespace climatedevice

ESPHOME_NAMESPACE_END

#endif  // USE_CLIMATEDEVICE
