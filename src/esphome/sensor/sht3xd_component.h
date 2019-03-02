#ifndef ESPHOME_SENSOR_SHT3XD_COMPONENT_H
#define ESPHOME_SENSOR_SHT3XD_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_SHT3XD

#include "esphome/sensor/sensor.h"
#include "esphome/i2c_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

class SHT3XDTemperatureSensor;
class SHT3XDHumiditySensor;

/// This class implements support for the SHT3x-DIS family of temperature+humidity i2c sensors.
class SHT3XDComponent : public PollingComponent, public I2CDevice {
 public:
  SHT3XDComponent(I2CComponent *parent, const std::string &temperature_name, const std::string &humidity_name,
                  uint8_t address = 0x44, uint32_t update_interval = 60000);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  SHT3XDTemperatureSensor *get_temperature_sensor() const;
  SHT3XDHumiditySensor *get_humidity_sensor() const;

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;

 protected:
  bool write_command_(uint16_t command);
  bool read_data_(uint16_t *data, uint8_t len);

  SHT3XDTemperatureSensor *temperature_sensor_;
  SHT3XDHumiditySensor *humidity_sensor_;
};

/// Helper class exposing an SHT3xD temperature sensor with a unique id.
class SHT3XDTemperatureSensor : public EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C, SHT3XDComponent> {
 public:
  SHT3XDTemperatureSensor(const std::string &name, SHT3XDComponent *parent);

  std::string unique_id() override;

 protected:
  friend SHT3XDComponent;

  std::string unique_id_;
};

/// Helper class exposing an SHT3xD humidity sensor with a unique id.
class SHT3XDHumiditySensor : public EmptyPollingParentSensor<1, ICON_WATER_PERCENT, UNIT_PERCENT, SHT3XDComponent> {
 public:
  SHT3XDHumiditySensor(const std::string &name, SHT3XDComponent *parent);

  std::string unique_id() override;

 protected:
  friend SHT3XDComponent;

  std::string unique_id_;
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_SHT3XD

#endif  // ESPHOME_SENSOR_SHT3XD_COMPONENT_H
