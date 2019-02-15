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

    class MPR121 {
        public:
            // Hardware I2C
            //MPR121(void);

            boolean begin(uint8_t i2caddr = MPR121_I2CADDR_DEFAULT);

            uint16_t filteredData(uint8_t t);
            uint16_t  baselineData(uint8_t t);

            uint8_t readRegister8(uint8_t reg);
            uint16_t readRegister16(uint8_t reg);
            void writeRegister(uint8_t reg, uint8_t value);
            uint16_t touched(void);
            void setThresholds(uint8_t touch, uint8_t release);

        protected:
            int8_t _i2caddr;
    };

    class MPR121_Channel : public binary_sensor::BinarySensor {
        public:
            int channel = 0;

            MPR121_Channel(const std::string &name,int channel_num = 0);
            void process_(uint16_t *data, uint16_t *last_data);
    };

    class MPR121_Sensor : public Component , public I2CDevice {
        public:
            MPR121_Sensor(I2CComponent *parent, const std::string &name, uint8_t address = MPR121_I2CADDR_DEFAULT, uint8_t num_channels = 12);
            sensor::MPR121_Channel *add_channel(sensor::MPR121_Channel *channel);
            void process_(uint8_t *ch, uint16_t *data,uint16_t *last_data);
            void setup() override;
            void dump_config() override;
            //float get_setup_priority() const override;
            void loop() override;
        protected:
            MPR121 cap = MPR121();
            std::vector<MPR121_Channel *> channels{};
            uint8_t num_channels;
            uint16_t lasttouched = 0;
            uint16_t currtouched = 0;
    };



} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_MPR121

#endif //ESPHOME_SENSOR_MPR121_H
