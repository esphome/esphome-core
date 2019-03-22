#ifndef ESPHOME_CLIMATEDEVICE_TEMPLATE_CLIMATEDEVICE_H
#define ESPHOME_CLIMATEDEVICE_TEMPLATE_CLIMATEDEVICE_H

#include "esphome/defines.h"

#ifdef USE_TEMPLATE_CLIMATEDEVICE

#include "esphome/climatedevice/climatedevice.h"
#include "esphome/automation.h"

ESPHOME_NAMESPACE_BEGIN

namespace climatedevice {

class TemplateClimateDevice : public ClimateDevice {
 public:
  explicit TemplateClimateDevice(const std::string &name, uint32_t update_interval = 60000);

  void set_mode_lambda(std::function<optional<ClimateDeviceMode>()> &&f);
  void set_target_temperature_lambda(std::function<optional<float>()> &&f);
  Trigger<ClimateDeviceState> *get_control_trigger() const;
  Trigger<float> *get_error_value_trigger() const;
  void set_optimistic(bool optimistic);

  void setup() override;
  void update() override;
  void dump_config() override;

  float get_setup_priority() const override;

 protected:
  optional<std::function<optional<ClimateDeviceMode>()>> mode_f_;
  optional<std::function<optional<float>()>> target_temperature_f_;
  bool optimistic_{false};
  Trigger<ClimateDeviceState> *control_trigger_{nullptr};
  Trigger<float> *errro_value_trigger_{nullptr};
};

}  // namespace climatedevice

ESPHOME_NAMESPACE_END

#endif  // USE_TEMPLATE_CLIMATEDEVICE

#endif  // ESPHOME_CLIMATEDEVICE_TEMPLATE_CLIMATEDEVICE_H
