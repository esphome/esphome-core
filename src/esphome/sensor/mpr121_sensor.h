#ifndef ESPHOME_SENSOR_MPR121_H
#define ESPHOME_SENSOR_MPR121_H

#include "esphome/defines.h"

#ifdef USE_MPR121

#include "esphome/sensor/sensor.h"
#include "esphome/binary_sensor/binary_sensor.h"
#include "esphome/i2c_component.h"
#include "esphome/component.h"

#define MPR121_I2CADDR_DEFAULT 0x5A

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

    enum {
        MPR121_TOUCHSTATUS_L = 0x00,
        MPR121_TOUCHSTATUS_H = 0x01,
        MPR121_FILTDATA_0L  = 0x04,
        MPR121_FILTDATA_0H  = 0x05,
        MPR121_BASELINE_0   = 0x1E,
        MPR121_MHDR         = 0x2B,
        MPR121_NHDR         = 0x2C,
        MPR121_NCLR         = 0x2D,
        MPR121_FDLR         = 0x2E,
        MPR121_MHDF         = 0x2F,
        MPR121_NHDF         = 0x30,
        MPR121_NCLF         = 0x31,
        MPR121_FDLF         = 0x32,
        MPR121_NHDT         = 0x33,
        MPR121_NCLT         = 0x34,
        MPR121_FDLT         = 0x35,

        MPR121_TOUCHTH_0    = 0x41,
        MPR121_RELEASETH_0  = 0x42,
        MPR121_DEBOUNCE     = 0x5B,
        MPR121_CONFIG1      = 0x5C,
        MPR121_CONFIG2      = 0x5D,
        MPR121_CHARGECURR_0 = 0x5F,
        MPR121_CHARGETIME_1 = 0x6C,
        MPR121_ECR          = 0x5E,
        MPR121_AUTOCONFIG0  = 0x7B,
        MPR121_AUTOCONFIG1  = 0x7C,
        MPR121_UPLIMIT      = 0x7D,
        MPR121_LOWLIMIT     = 0x7E,
        MPR121_TARGETLIMIT  = 0x7F,

        MPR121_GPIODIR      = 0x76,
        MPR121_GPIOEN       = 0x77,
        MPR121_GPIOSET      = 0x78,
        MPR121_GPIOCLR      = 0x79,
        MPR121_GPIOTOGGLE   = 0x7A,

        MPR121_SOFTRESET    = 0x80,
    };

    class MPR121_Channel : public binary_sensor::BinarySensor {
        public:
            int channel = 0;

            MPR121_Channel(const std::string &name,int channel_num = 0);
            void process_(uint16_t *data, uint16_t *last_data);
    };

    class MPR121_Sensor : public Component , public I2CDevice {
        public:
            MPR121_Sensor(I2CComponent *parent, uint8_t address = MPR121_I2CADDR_DEFAULT);
            sensor::MPR121_Channel *add_channel(sensor::MPR121_Channel *channel);
            void process_(uint16_t *data,uint16_t *last_data);
            void setup() override;
            void dump_config() override;
            //float get_setup_priority() const override;
            void loop() override;
            void set_sensitivity(uint8_t sens_touch = 12, uint8_t sens_release = 6, uint8 channel=-1);
        protected:
            std::vector<MPR121_Channel *> channels{};
            uint16_t lasttouched = 0;
            uint16_t currtouched = 0;
            enum ErrorCode {
                NONE = 0,
                COMMUNICATION_FAILED,
                WRONG_CHIP_STATE,
            } error_code_{NONE};
            uint16_t read_mpr121_channels();
    };



} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_MPR121

#endif //ESPHOME_SENSOR_MPR121_H
