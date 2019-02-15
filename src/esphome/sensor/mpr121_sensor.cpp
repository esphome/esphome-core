#include "esphome/defines.h"
#define USE_MPR121 1
#ifdef USE_MPR121

#include "esphome.h"
#include "esphome/sensor/mpr121_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

    static const char *TAG = "sensor.mpr121";

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

        MPR121_SOFTRESET = 0x80,
    };

    MPR121_Channel::MPR121_Channel(const std::string &name,int channel_num) : BinarySensor(name) {
      ESP_LOGD(TAG, "channel %d " , channel_num);
      channel = channel_num;
    }

    void MPR121_Channel::process_(uint16_t *data, uint16_t *last_data) {
        if ((*data & _BV(channel)) && !(*last_data & _BV(channel))) {
            this->publish_state(true);
        }
        if (!(*data & _BV(channel)) && (*last_data & _BV(channel)) ) {
            this->publish_state(false);
        }
    }

    MPR121_Sensor::MPR121_Sensor(I2CComponent *parent, const std::string &name, uint8_t address, uint8_t num_channels)
    :  I2CDevice(parent, address) {
        this->num_channels = num_channels;
    }

    void MPR121_Sensor::setup() {
      bool connected = cap.begin(0x5A);
        if (!connected) {
          ;//ESP_LOGD("mpr121_setup", "NOT connected");
        }
    }

    void MPR121_Sensor::dump_config() {
        ESP_LOGCONFIG(TAG, "MPR121:");
        LOG_I2C_DEVICE(this);
    }

    MPR121_Channel *MPR121_Sensor::add_channel(sensor::MPR121_Channel* channel) {
      this->channels.push_back(channel);
      return channel;
    }

    void MPR121_Sensor::process_(uint8_t *ch, uint16_t *data,uint16_t *last_data) {
      for (auto *channel : this->channels) {
        if(channel->channel == *ch)
          channel->process_(data,last_data);
      }
    }

    void MPR121_Sensor::loop() {
        // Get the currently touched pads
        this->currtouched = this->cap.touched();
        if(this->currtouched != this->lasttouched) {
          for (uint8_t i=0; i<this->num_channels; i++) {
            this->process_(&i,&currtouched,&lasttouched);
          }
        }

        // reset our state
        this->lasttouched = this->currtouched;
    }


    /*******************************************************************************************
    *  @brief      Default constructor
    ****************************************************************************************/
  //  MPR121::MPR121() {
  //  }

    /**
     *****************************************************************************************
    *  @brief      Begin an MPR121 object on a given I2C bus. This function resets the
    *              device and writes the default settings.
    *
    *  @param      i2caddr the i2c address the device can be found on. Defaults to 0x5A.
    *  @returns    true on success, false otherwise
    ****************************************************************************************/
    boolean MPR121::begin(uint8_t i2caddr) {
        Wire.begin();

        _i2caddr = i2caddr;

        // soft reset
        writeRegister(MPR121_SOFTRESET, 0x63);
        delay(1);
        for (uint8_t i=0; i<0x7F; i++) {
        //  Serial.print("$"); Serial.print(i, HEX);
        //  Serial.print(": 0x"); Serial.println(readRegister8(i));
        }


        writeRegister(MPR121_ECR, 0x0);

        uint8_t c = readRegister8(MPR121_CONFIG2);

        if (c != 0x24) return false;


        setThresholds(12, 6);
        writeRegister(MPR121_MHDR, 0x01);
        writeRegister(MPR121_NHDR, 0x01);
        writeRegister(MPR121_NCLR, 0x0E);
        writeRegister(MPR121_FDLR, 0x00);

        writeRegister(MPR121_MHDF, 0x01);
        writeRegister(MPR121_NHDF, 0x05);
        writeRegister(MPR121_NCLF, 0x01);
        writeRegister(MPR121_FDLF, 0x00);

        writeRegister(MPR121_NHDT, 0x00);
        writeRegister(MPR121_NCLT, 0x00);
        writeRegister(MPR121_FDLT, 0x00);

        writeRegister(MPR121_DEBOUNCE, 0);
        writeRegister(MPR121_CONFIG1, 0x10); // default, 16uA charge current
        writeRegister(MPR121_CONFIG2, 0x20); // 0.5uS encoding, 1ms period

        //  writeRegister(MPR121_AUTOCONFIG0, 0x8F);

        //  writeRegister(MPR121_UPLIMIT, 150);
        //  writeRegister(MPR121_TARGETLIMIT, 100); // should be ~400 (100 shifted)
        //  writeRegister(MPR121_LOWLIMIT, 50);
        // enable all electrodes
        writeRegister(MPR121_ECR, 0x8F);  // start with first 5 bits of baseline tracking

        return true;
    }

    /**
     *****************************************************************************************
    *  @brief      Set the touch and release thresholds for all 13 channels on the device to the
    *              passed values. The threshold is defined as a deviation value from the baseline value,
    *              so it remains constant even baseline value changes. Typically the touch
    *              threshold is a little bigger than the release threshold to touch debounce and hysteresis.
    *
    *              For typical touch application, the value can be in range 0x05~0x30 for example. The setting
    *              of the threshold is depended on the actual application. For the operation details and how to set the threshold refer to
    *              application note AN3892 and MPR121 design guidelines.
    *
    *  @param      touch the touch threshold value from 0 to 255.
    *  @param      release the release threshold from 0 to 255.
    ****************************************************************************************/
    void MPR121::setThresholds(uint8_t touch, uint8_t release) {
        for (uint8_t i=0; i<12; i++) {
            writeRegister(MPR121_TOUCHTH_0 + 2*i, touch);
            writeRegister(MPR121_RELEASETH_0 + 2*i, release);
        }
    }

    /**
     *****************************************************************************************
    *  @brief      Read the filtered data from channel t. The ADC raw data outputs run through 3
    *              levels of digital filtering to filter out the high frequency and low frequency noise
    *              encountered. For detailed information on this filtering see page 6 of the device datasheet.
    *
    *  @param      t the channel to read
    *  @returns    the filtered reading as a 10 bit unsigned value
    ****************************************************************************************/
    uint16_t  MPR121::filteredData(uint8_t t) {
        if (t > 12) return 0;
        return readRegister16(MPR121_FILTDATA_0L + t*2);
    }

    /**
     *****************************************************************************************
    *  @brief      Read the baseline value for the channel. The 3rd level filtered result is internally 10bit
    *              but only high 8 bits are readable from registers 0x1E~0x2A as the baseline value output for each channel.
    *
    *  @param      t the channel to read.
    *  @returns    the baseline data that was read
    ****************************************************************************************/
    uint16_t  MPR121::baselineData(uint8_t t) {
        if (t > 12) return 0;
        uint16_t bl = readRegister8(MPR121_BASELINE_0 + t);
        return (bl << 2);
    }

    /**
     *****************************************************************************************
    *  @brief      Read the touch status of all 13 channels as bit values in a 12 bit integer.
    *
    *  @returns    a 12 bit integer with each bit corresponding to the touch status of a sensor.
    *              For example, if bit 0 is set then channel 0 of the device is currently deemed to be touched.
    ****************************************************************************************/
    uint16_t  MPR121::touched(void) {
        uint16_t t = readRegister16(MPR121_TOUCHSTATUS_L);
        return t & 0x0FFF;
    }

    /*********************************************************************/

    /**
     *****************************************************************************************
    *  @brief      Read the contents of an 8 bit device register.
    *
    *  @param      reg the register address to read from
    *  @returns    the 8 bit value that was read.
    ****************************************************************************************/
    uint8_t MPR121::readRegister8(uint8_t reg) {
        Wire.beginTransmission(_i2caddr);
        Wire.write(reg);
        Wire.endTransmission(false);
        Wire.requestFrom(_i2caddr, 1);
        if (Wire.available() < 1)
        return 0;
        return (Wire.read());
    }

    /**
     *****************************************************************************************
    *  @brief      Read the contents of a 16 bit device register.
    *
    *  @param      reg the register address to read from
    *  @returns    the 16 bit value that was read.
    ****************************************************************************************/
    uint16_t MPR121::readRegister16(uint8_t reg) {
        Wire.beginTransmission(_i2caddr);
        Wire.write(reg);
        Wire.endTransmission(false);
        Wire.requestFrom(_i2caddr, 2);
        if (Wire.available() < 2)
        return 0;
        uint16_t v = Wire.read();
        v |=  ((uint16_t) Wire.read()) << 8;
        return v;
    }

    /**************************************************************************/
    /*!
        @brief  Writes 8-bits to the specified destination register
        @param  reg the register address to write to
        @param  value the value to write
    */
    /**************************************************************************/
    void MPR121::writeRegister(uint8_t reg, uint8_t value) {
        Wire.beginTransmission(_i2caddr);
        Wire.write((uint8_t)reg);
        Wire.write((uint8_t)(value));
        Wire.endTransmission();
    }
} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_BH1750
