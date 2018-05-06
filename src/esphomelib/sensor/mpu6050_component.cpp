//
//  mpu6050_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 05.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/sensor/mpu6050_component.h"
#include "esphomelib/log.h"

#ifdef USE_MPU6050

#include <Wire.h>

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor.mpu6050";

const uint8_t MPU6050_REGISTER_WHO_AM_I = 0x75;
const uint8_t MPU6050_REGISTER_POWER_MANAGEMENT_1 = 0x6B;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG = 0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG = 0x1C;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H = 0x3B;
const uint8_t MPU6050_CLOCK_SOURCE_X_GYRO = 0b001;
const uint8_t MPU6050_SCALE_2000_DPS = 0b11;
const float MPU6050_SCALE_DPS_PER_DIGIT_2000 = 0.060975f;
const uint8_t MPU6050_RANGE_2G = 0b00;
const float MPU6050_RANGE_PER_DIGIT_2G = 0.000061f;
const uint8_t MPU6050_BIT_SLEEP_ENABLED = 6;
const uint8_t MPU6050_BIT_TEMPERATURE_DISABLED = 3;
const float GRAVITY_EARTH = 9.80665f;

MPU6050Component::MPU6050Component(uint8_t address, uint32_t update_interval)
    : PollingComponent(update_interval), address_(address) {

}

void MPU6050Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MPU6050 on address 0x%X...", this->address_);
  if (this->read_register_8_(MPU6050_REGISTER_WHO_AM_I) != 0x68) {
    ESP_LOGE(TAG, "Can't communicate with MPU6050.");
    this->mark_failed();
    return;
  }

  ESP_LOGV(TAG, "    Setting up Power Management...");
  // Setup power management
  uint8_t power_management = this->read_register_8_(MPU6050_REGISTER_POWER_MANAGEMENT_1);
  ESP_LOGV(TAG, "    Input power_management: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(power_management));
  // Set clock source - X-Gyro
  power_management &= 0b11111000;
  power_management |= MPU6050_CLOCK_SOURCE_X_GYRO;
  // Disable sleep
  power_management &= ~(1 << MPU6050_BIT_SLEEP_ENABLED);
  // Enable temperature
  power_management &= ~(1 << MPU6050_BIT_TEMPERATURE_DISABLED);
  ESP_LOGV(TAG, "    Output power_management: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(power_management));
  this->write_register_8_(MPU6050_REGISTER_POWER_MANAGEMENT_1, power_management);

  ESP_LOGV(TAG, "    Setting up Gyro Config...");
  // Set scale - 2000DPS
  uint8_t gyro_config = this->read_register_8_(MPU6050_REGISTER_GYRO_CONFIG);
  ESP_LOGV(TAG, "    Input gyro_config: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(gyro_config));
  gyro_config &= 0b11100111;
  gyro_config |= MPU6050_SCALE_2000_DPS << 3;
  ESP_LOGV(TAG, "    Output gyro_config: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(gyro_config));
  this->write_register_8_(MPU6050_REGISTER_GYRO_CONFIG, gyro_config);

  ESP_LOGV(TAG, "    Setting up Accel Config...");
  // Set range - 2G
  uint8_t accel_config = this->read_register_8_(MPU6050_REGISTER_ACCEL_CONFIG);
  ESP_LOGV(TAG, "    Input accel_config: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(accel_config));
  accel_config &= 0b11100111;
  accel_config |= (MPU6050_RANGE_2G << 3);
  ESP_LOGV(TAG, "    Output accel_config: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(accel_config));
  this->write_register_8_(MPU6050_REGISTER_ACCEL_CONFIG, accel_config);
}

void MPU6050Component::update() {
  ESP_LOGV(TAG, "    Updating MPU6050...");
  Wire.beginTransmission(this->address_);
  Wire.write(MPU6050_REGISTER_ACCEL_XOUT_H);
  Wire.endTransmission();

  Wire.beginTransmission(this->address_);
  Wire.requestFrom(this->address_, 14u);

  uint32_t start = millis();
  while (Wire.available() < 14) {
    if (millis() - start > 100) {
      ESP_LOGW(TAG, "Reading data timed out...");
      return;
    }
  }

  int16_t raw_accel_x = (Wire.read() & 0xFF) << 8;
  raw_accel_x |= Wire.read() & 0xFF;
  int16_t raw_accel_y = (Wire.read() & 0xFF) << 8;
  raw_accel_y |= Wire.read() & 0xFF;
  int16_t raw_accel_z = (Wire.read() & 0xFF) << 8;
  raw_accel_z |= Wire.read() & 0xFF;

  int16_t raw_temp = (Wire.read() & 0xFF) << 8;
  raw_temp |= Wire.read() & 0xFF;

  int16_t raw_gyro_x = (Wire.read() & 0xFF) << 8;
  raw_gyro_x |= Wire.read() & 0xFF;
  int16_t raw_gyro_y = (Wire.read() & 0xFF) << 8;
  raw_gyro_y |= Wire.read() & 0xFF;
  int16_t raw_gyro_z = (Wire.read() & 0xFF) << 8;
  raw_gyro_z |= Wire.read() & 0xFF;

  Wire.endTransmission();

  ESP_LOGV(TAG, "Raw accel: {x=0b" BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN
      ", y=0b" BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN
      ", z=0b" BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN,
             BYTE_TO_BINARY(raw_accel_x >> 8), BYTE_TO_BINARY(raw_accel_x),
             BYTE_TO_BINARY(raw_accel_y >> 8), BYTE_TO_BINARY(raw_accel_y),
             BYTE_TO_BINARY(raw_accel_z >> 8), BYTE_TO_BINARY(raw_accel_z)
  );

  ESP_LOGV(TAG, "Raw gyro: {x=0b" BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN
      ", y=0b" BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN
      ", z=0b" BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN,
           BYTE_TO_BINARY(raw_gyro_x >> 8), BYTE_TO_BINARY(raw_gyro_x),
           BYTE_TO_BINARY(raw_gyro_y >> 8), BYTE_TO_BINARY(raw_gyro_y),
           BYTE_TO_BINARY(raw_gyro_z >> 8), BYTE_TO_BINARY(raw_gyro_z)
  );

  ESP_LOGV(TAG, "Raw temperature: 0b" BYTE_TO_BINARY_PATTERN BYTE_TO_BINARY_PATTERN,
           BYTE_TO_BINARY(raw_temp >> 8), BYTE_TO_BINARY(raw_temp));

  float accel_x = raw_accel_x * MPU6050_RANGE_PER_DIGIT_2G * GRAVITY_EARTH;
  float accel_y = raw_accel_y * MPU6050_RANGE_PER_DIGIT_2G * GRAVITY_EARTH;
  float accel_z = raw_accel_z * MPU6050_RANGE_PER_DIGIT_2G * GRAVITY_EARTH;

  float temperature = raw_temp / 340.0f + 36.53f;

  float gyro_x = raw_gyro_x * MPU6050_SCALE_DPS_PER_DIGIT_2000;
  float gyro_y = raw_gyro_y * MPU6050_SCALE_DPS_PER_DIGIT_2000;
  float gyro_z = raw_gyro_z * MPU6050_SCALE_DPS_PER_DIGIT_2000;

  ESP_LOGD(TAG, "Got accel={x=%.3f m/s², y=%.3f m/s², z=%.3f m/s²}, "
                "gyro={x=%.3f °/s, y=%.3f °/s, z=%.3f °/s}, temp=%.3f°C",
           accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z, temperature);

  if (this->accel_x_sensor_ != nullptr)
    this->accel_x_sensor_->push_new_value(accel_x);
  if (this->accel_y_sensor_ != nullptr)
    this->accel_y_sensor_->push_new_value(accel_y);
  if (this->accel_z_sensor_ != nullptr)
    this->accel_z_sensor_->push_new_value(accel_z);

  if (this->temperature_sensor_ != nullptr)
    this->temperature_sensor_->push_new_value(temperature);

  if (this->gyro_x_sensor_ != nullptr)
    this->gyro_x_sensor_->push_new_value(gyro_x);
  if (this->gyro_y_sensor_ != nullptr)
    this->gyro_y_sensor_->push_new_value(gyro_y);
  if (this->gyro_z_sensor_ != nullptr)
    this->gyro_z_sensor_->push_new_value(gyro_z);
}

uint8_t MPU6050Component::read_register_8_(uint8_t reg) {
  Wire.beginTransmission(this->address_);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.beginTransmission(this->address_);
  Wire.requestFrom(this->address_, 1u);
  uint32_t start = millis();
  while (Wire.available() < 1) {
    if (millis() - start > 100) {
      ESP_LOGW(TAG, "Reading register timed out...");
      return 0;
    }
  }
  uint8_t value = Wire.read();
  Wire.endTransmission();
  delay(5);

  return value;
}
void MPU6050Component::write_register_8_(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(this->address_);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();

  delay(5);
}
MPU6050AccelSensor *MPU6050Component::make_accel_x_sensor(const std::string &name) {
  return this->accel_x_sensor_ = new MPU6050AccelSensor(name, this);
}
MPU6050AccelSensor *MPU6050Component::make_accel_y_sensor(const std::string &name) {
  return this->accel_y_sensor_ = new MPU6050AccelSensor(name, this);
}
MPU6050AccelSensor *MPU6050Component::make_accel_z_sensor(const std::string &name) {
  return this->accel_z_sensor_ = new MPU6050AccelSensor(name, this);
}
MPU6050GyroSensor *MPU6050Component::make_gyro_x_sensor(const std::string &name) {
  return this->gyro_x_sensor_ = new MPU6050GyroSensor(name, this);
}
MPU6050GyroSensor *MPU6050Component::make_gyro_y_sensor(const std::string &name) {
  return this->gyro_y_sensor_ = new MPU6050GyroSensor(name, this);
}
MPU6050GyroSensor *MPU6050Component::make_gyro_z_sensor(const std::string &name) {
  return this->gyro_z_sensor_ = new MPU6050GyroSensor(name, this);
}
MPU6050TemperatureSensor *MPU6050Component::make_temperature_sensor(const std::string &name) {
  return this->temperature_sensor_ = new MPU6050TemperatureSensor(name, this);
}
float MPU6050Component::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

} // namespace sensor

} // namespace esphomelib

#endif //USE_MPUT6050
