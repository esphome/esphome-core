#include "esphome/defines.h"
#define USE_MPR121 1
#ifdef USE_MPR121

#include "esphome.h"
#include "esphome/sensor/mpr121_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

    static const char *TAG = "sensor.mpr121";

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
        ESP_LOGCONFIG(TAG, "Setting up MPR121...");
        // soft reset
        if (!this->write_byte(MPR121_SOFTRESET, 0x63)) {
            this->error_code_ = COMMUNICATION_FAILED;
            this->mark_failed();
            return;
        }
        delay(1);
        this->write_byte(MPR121_ECR, 0x0);

        //set touch sensitivity
        for (uint8_t i=0; i<12; i++) {
            this->write_byte(MPR121_TOUCHTH_0 + 2*i, 12);
            this->write_byte(MPR121_RELEASETH_0 + 2*i, 6);
        }
        this->write_byte(MPR121_MHDR, 0x01);
        this->write_byte(MPR121_NHDR, 0x01);
        this->write_byte(MPR121_NCLR, 0x0E);
        this->write_byte(MPR121_FDLR, 0x00);

        this->write_byte(MPR121_MHDF, 0x01);
        this->write_byte(MPR121_NHDF, 0x05);
        this->write_byte(MPR121_NCLF, 0x01);
        this->write_byte(MPR121_FDLF, 0x00);

        this->write_byte(MPR121_NHDT, 0x00);
        this->write_byte(MPR121_NCLT, 0x00);
        this->write_byte(MPR121_FDLT, 0x00);

        this->write_byte(MPR121_DEBOUNCE, 0);
        this->write_byte(MPR121_CONFIG1, 0x10); // default, 16uA charge current
        this->write_byte(MPR121_CONFIG2, 0x20); // 0.5uS encoding, 1ms period

        this->write_byte(MPR121_ECR, 0x8F);     // start with first 5 bits of baseline tracking

    }

    void MPR121_Sensor::dump_config() {
        ESP_LOGCONFIG(TAG, "BMP280:");
        LOG_I2C_DEVICE(this);
        switch (this->error_code_) {
            case COMMUNICATION_FAILED:
                ESP_LOGE(TAG, "Communication with MPR121 failed!");
                break;
            case WRONG_CHIP_STATE:
                ESP_LOGE(TAG, "MPR121 has wrong default value for CONFIG2?");
                break;
            case NONE:
            default:
                break;
        }
        ESP_LOGCONFIG(TAG, "  currvalue: %d", this->read_mpr121_channels());
        //LOG_UPDATE_INTERVAL(this);
    }

    MPR121_Channel *MPR121_Sensor::add_channel(sensor::MPR121_Channel* channel) {
      this->channels.push_back(channel);
      return channel;
    }

    void MPR121_Sensor::process_(uint8_t *ch, uint16_t *data, uint16_t *last_data) {
      for (auto *channel : this->channels) {
        if(channel->channel == *ch)
          channel->process_(data,last_data);
      }
    }

    uint16_t MPR121_Sensor::read_mpr121_channels() {
        uint16_t val = 0;
        this->read_byte_16(MPR121_TOUCHSTATUS_L, &val);
        uint8_t lsb = val >> 8;
        uint8_t msb = val ;
        val = ((uint16_t) msb) << 8;
        val |= lsb;

        //ESP_LOGD(TAG,"msb:%d lsb:%d value: %d",msb,lsb,val);
        return val ;
    }

    void MPR121_Sensor::loop() {
        this->currtouched = this->read_mpr121_channels();
        if(this->currtouched != this->lasttouched) {
          for (uint8_t i = 0; i < this->num_channels; i++) {
            this->process_(&i, &currtouched,&lasttouched);
          }
        }
        // reset touchstate
        this->lasttouched = this->currtouched;
    }


 } // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_BH1750
