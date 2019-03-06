#ifndef ESPHOME_SDS011_COMPONENT_H
#define ESPHOME_SDS011_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_SDS011

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"
#include "esphome/uart_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

using SDS011Sensor = sensor::EmptySensor<1, ICON_CHEMICAL_WEAPON, UNIT_MICROGRAMS_PER_CUBIC_METER>;

class SDS011Component : public Component, public UARTDevice {
 public:
  /** Construct the component.
   *
   * @param parent UARTComponent
   * @param update_interval_min The update interval in minutes.
   * @param rx_mode_only RX-only mode to avoid sending data to the sensor.
   */
  SDS011Component(UARTComponent *parent, uint8_t update_interval_min = 0, bool rx_mode_only = false);

  /// Manually set the rx-only mode. Defaults to false.
  void set_rx_mode_only(bool rx_mode_only);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void setup() override;
  void dump_config() override;
  void loop() override;

  float get_setup_priority() const override;
  SDS011Sensor *make_pm_2_5_sensor(const std::string &name);
  SDS011Sensor *make_pm_10_0_sensor(const std::string &name);
  bool get_rx_mode_only() const;

  void set_update_interval_min(uint8_t update_interval_min);

 protected:
  void sds011_write_command_(const uint8_t *command);
  uint8_t sds011_checksum_(const uint8_t *command_data, uint8_t length) const;
  optional<bool> check_byte_() const;
  void parse_data_();
  uint16_t get_16_bit_uint_(uint8_t start_index) const;

  SDS011Sensor *pm_2_5_sensor_{nullptr};
  SDS011Sensor *pm_10_0_sensor_{nullptr};

  uint8_t data_[10];
  uint8_t data_index_{0};
  uint32_t last_transmission_{0};
  uint8_t update_interval_min_;

  bool rx_mode_only_;
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_SDS011

#endif  // ESPHOME_SDS011_COMPONENT_H
