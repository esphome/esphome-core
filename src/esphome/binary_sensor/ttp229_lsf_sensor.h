#ifndef ESPHOME_BINARY_SENSOR_TTP229_LSF_H
#define ESPHOME_BINARY_SENSOR_TTP229_LSF_H

#include "esphome/defines.h"

#ifdef USE_TTP229_LSF

#include "esphome/binary_sensor/binary_sensor.h"
#include "esphome/i2c_component.h"
#include "esphome/component.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

class TTP229Channel : public binary_sensor::BinarySensor {
 public:
  TTP229Channel(const std::string &name, int channel_num);
  void process(uint16_t data);

 protected:
  int channel_;
};

class TTP229LSFComponent : public Component, public I2CDevice {
 public:
  TTP229LSFComponent(I2CComponent *parent, uint8_t address);
  binary_sensor::TTP229Channel *add_channel(binary_sensor::TTP229Channel *channel);
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void loop() override;

 protected:
  std::vector<TTP229Channel *> channels_{};
  enum ErrorCode {
    NONE = 0,
    COMMUNICATION_FAILED,
  } error_code_{NONE};
};

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_TTP229_LSF

#endif  // ESPHOME_BINARY_SENSOR_TTP229_LSF_H
