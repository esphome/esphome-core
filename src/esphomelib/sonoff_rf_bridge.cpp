#include "esphomelib/defines.h"

#ifdef USE_SONOFF_RF_BRIDGE

#include "esphomelib/sonoff_rf_bridge.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

static const char *TAG = "sonoff_rf_bridge";

void SonoffRFBridge::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Sonoff RF Bridge...");
}
void SonoffRFBridge::loop() {
  uint32_t now = millis();
  if (now - this->last_transmission_ > 500) {
    this->data_at_ = 0;
  }

  while (this->available()) {
    this->last_transmission_ = now;

    uint8_t data;
    if (!this->read_byte(&data) || !this->check_byte_(data, this->data_at_)) {
      this->data_at_ = 0;
      continue;
    }

    // START  RFIN --T_SYNC-- --T_LOW-- --T_HIGH-- ---- DATA ---- -END
    // 0xAA   0xA4 0xXX  0xXX 0xXX 0xXX 0xXX  0xXX 0xXX 0xXX 0xXX 0x55
    // 0      1    2     3    4    5    6     7    8    9    10   11
    if (this->data_at_ == 11) {
      this->ack_();
      this->process_data_();
    }
    this->data_at_ = (this->data_at_ + 1) % 12;
  }
}
float SonoffRFBridge::get_setup_priority() const {
  return setup_priority::HARDWARE;
}
void SonoffRFBridge::send_code(uint16_t sync, uint16_t low, uint16_t high, uint32_t data) {
  uint8_t message[9];
  message[0] = sync >> 8;
  message[1] = sync;
  message[2] = low >> 8;
  message[3] = low;
  message[4] = high >> 8;
  message[5] = high;
  message[6] = data >> 16;
  message[7] = data >> 8;
  message[8] = data;
  this->send_(message);
}
void SonoffRFBridge::send_code(uint16_t sync, uint16_t low, uint16_t high, const char *data) {
  uint32_t data_ = 0;
  for (uint8_t i = 0; i < 24; i++) {
    data_ <<= 1;
    data_ |= data[i] == '1';
  }
}
bool SonoffRFBridge::check_byte_(uint8_t byte, uint8_t i) {
  if (i == 0)
    return byte == 0xAA; // START

  if (i == 1)
    return byte == 0xA4; // RFIN

  if (i == 11)
    return byte == 0x55; // STOP

  return true;
}
void SonoffRFBridge::process_data_() {
  uint16_t t_sync = (uint16_t(this->data_[2]) << 8) | uint16_t(this->data_[3]);
  uint16_t t_low = (uint16_t(this->data_[4]) << 8) | uint16_t(this->data_[5]);
  uint16_t t_high = (uint16_t(this->data_[6]) << 8) | uint16_t(this->data_[7]);
  uint32_t data = (uint32_t(this->data_[8]) << 16) | (uint32_t(this->data_[9]) << 8) | (uint32_t(this->data_[10]));
  char data_s[32];
  for (int8_t i = 0; i < 24; i++) {
    uint8_t b = (data >> (23 - i)) & 1;
    data_s[i] = '0' + b;
  }
  ESP_LOGD(TAG, "Got data sync=%u low=%u high=%u data='%s'",
           t_sync, t_low, t_high, data_s);


}
void SonoffRFBridge::ack_() {
  this->write_str("\r\n");
  this->write_byte(0xAA); // START
  this->write_byte(0xA0); // ACK
  this->write_byte(0x55); // STOP
  this->write_str("\r\n");
}
void SonoffRFBridge::send_(uint8_t *message) {
  this->write_str("\r\n");
  this->write_byte(0xAA); // START
  this->write_byte(0xA5); // RFOUT
  for (uint8_t i = 0; i < 9; i++)
    this->write_byte(message[i]);
  this->write_byte(0x55); // STOP
  this->write_str("\r\n");
}
SonoffRFBinarySensor *SonoffRFBridge::make_binary_sensor(const std::string &name,
                                                         uint16_t sync,
                                                         uint16_t low,
                                                         uint16_t high,
                                                         uint32_t data) {
  auto sens = new SonoffRFBinarySensor(name, sync, low, high, data);
  this->binary_sensors_.push_back(sens);
  return sens;
}
SonoffRFSwitch *SonoffRFBridge::make_switch(const std::string &name,
                                            uint16_t sync,
                                            uint16_t low,
                                            uint16_t high,
                                            uint32_t data) {
  return new SonoffRFSwitch(name, this, sync, low, high, data);
}
SonoffRFBridge::SonoffRFBridge(UARTComponent *parent) : UARTDevice(parent) {}

bool SonoffRFBinarySensor::process(uint16_t sync, uint16_t low, uint16_t high, uint32_t data) {
  if (this->sync_ != sync || this->low_ != low || this->high_ != high || this->data_ != data) {
    return false;
  }

  this->publish_state(true);
  this->publish_state(false);
  return true;
}
SonoffRFBinarySensor::SonoffRFBinarySensor(const std::string &name,
                                           uint16_t sync,
                                           uint16_t low,
                                           uint16_t high,
                                           uint32_t data)
    : BinarySensor(name), sync_(sync), low_(low), high_(high), data_(data) {

}

void SonoffRFSwitch::write_state(bool state) {
  if (!state)
    return;

  // acknowledge
  this->publish_state(true);
  this->parent_->send_code(this->sync_, this->low_, this->high_, this->data_);
}

SonoffRFSwitch::SonoffRFSwitch(const std::string &name,
                               SonoffRFBridge *parent,
                               uint16_t sync,
                               uint16_t low,
                               uint16_t high,
                               uint32_t data)
    : Switch(name), parent_(parent), sync_(sync), low_(low), high_(high), data_(data) {

}

ESPHOMELIB_NAMESPACE_END

#endif //USE_SONOFF_RF_BRIDGE
