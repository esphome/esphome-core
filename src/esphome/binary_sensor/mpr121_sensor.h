#ifndef ESPHOME_BINARY_SENSOR_MPR121_H
#define ESPHOME_BINARY_SENSOR_MPR121_H

#include "esphome/defines.h"

#ifdef USE_MPR121

#include "esphome/binary_sensor/binary_sensor.h"
#include "esphome/i2c_component.h"
#include "esphome/component.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

enum {
  MPR121_TOUCHSTATUS_L = 0x00,
  MPR121_TOUCHSTATUS_H = 0x01,
  MPR121_FILTDATA_0L = 0x04,
  MPR121_FILTDATA_0H = 0x05,
  MPR121_BASELINE_0 = 0x1E,
  MPR121_MHDR = 0x2B,
  MPR121_NHDR = 0x2C,
  MPR121_NCLR = 0x2D,
  MPR121_FDLR = 0x2E,
  MPR121_MHDF = 0x2F,
  MPR121_NHDF = 0x30,
  MPR121_NCLF = 0x31,
  MPR121_FDLF = 0x32,
  MPR121_NHDT = 0x33,
  MPR121_NCLT = 0x34,
  MPR121_FDLT = 0x35,
  MPR121_TOUCHTH_0 = 0x41,
  MPR121_RELEASETH_0 = 0x42,
  MPR121_DEBOUNCE = 0x5B,
  MPR121_CONFIG1 = 0x5C,
  MPR121_CONFIG2 = 0x5D,
  MPR121_CHARGECURR_0 = 0x5F,
  MPR121_CHARGETIME_1 = 0x6C,
  MPR121_ECR = 0x5E,
  MPR121_AUTOCONFIG0 = 0x7B,
  MPR121_AUTOCONFIG1 = 0x7C,
  MPR121_UPLIMIT = 0x7D,
  MPR121_LOWLIMIT = 0x7E,
  MPR121_TARGETLIMIT = 0x7F,
  MPR121_GPIODIR = 0x76,
  MPR121_GPIOEN = 0x77,
  MPR121_GPIOSET = 0x78,
  MPR121_GPIOCLR = 0x79,
  MPR121_GPIOTOGGLE = 0x7A,
  MPR121_SOFTRESET = 0x80,
};

class MPR121Channel : public binary_sensor::BinarySensor {
 public:
  MPR121Channel(const std::string &name, int channel_num = 0);
  void process(const uint16_t *data, const uint16_t *last_data);

 protected:
  int channel_ = 0;
};

class MPR121Component : public Component, public I2CDevice {
 public:
  MPR121Component(I2CComponent *parent, uint8_t address = 0x5A);
  binary_sensor::MPR121Channel *add_channel(binary_sensor::MPR121Channel *channel);
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void loop() override;

 protected:
  std::vector<MPR121Channel *> channels_{};
  uint16_t lasttouched_ = 0;
  uint16_t currtouched_ = 0;
  enum ErrorCode {
    NONE = 0,
    COMMUNICATION_FAILED,
    WRONG_CHIP_STATE,
  } error_code_{NONE};
  void process_(uint16_t *data, uint16_t *last_data);
  uint16_t read_mpr121_channels_();
};

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MPR121

#endif  // ESPHOME_BINARY_SENSOR_MPR121_H
