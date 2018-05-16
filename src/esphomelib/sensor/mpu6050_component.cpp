//
//  mpu6050_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 05.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//
// Based on:
//   - https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
//   - https://github.com/jarzebski/Arduino-MPU6050
//   - https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050

#include "esphomelib/sensor/mpu6050_component.h"
#include "esphomelib/log.h"

#ifdef USE_MPU6050

ESPHOMELIB_NAMESPACE_BEGIN

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

MPU6050Component::MPU6050Component(I2CComponent *parent, uint8_t address, uint32_t update_interval)
    : PollingComponent(update_interval), I2CDevice(parent, address) {

}

void MPU6050Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MPU6050 on address 0x%02X...", this->address_);
  uint8_t who_am_i;
  if (!this->read_byte(MPU6050_REGISTER_WHO_AM_I, &who_am_i) || who_am_i != 0x68) {
    ESP_LOGE(TAG, "Can't communicate with MPU6050.");
    this->mark_failed();
    return;
  }

  ESP_LOGV(TAG, "    Setting up Power Management...");
  // Setup power management
  uint8_t power_management;
  this->read_byte(MPU6050_REGISTER_POWER_MANAGEMENT_1, &power_management);
  ESP_LOGV(TAG, "    Input power_management: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(power_management));
  // Set clock source - X-Gyro
  power_management &= 0b11111000;
  power_management |= MPU6050_CLOCK_SOURCE_X_GYRO;
  // Disable sleep
  power_management &= ~(1 << MPU6050_BIT_SLEEP_ENABLED);
  // Enable temperature
  power_management &= ~(1 << MPU6050_BIT_TEMPERATURE_DISABLED);
  ESP_LOGV(TAG, "    Output power_management: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(power_management));
  this->write_byte(MPU6050_REGISTER_POWER_MANAGEMENT_1, power_management);

  ESP_LOGV(TAG, "    Setting up Gyro Config...");
  // Set scale - 2000DPS
  uint8_t gyro_config;
  this->read_byte(MPU6050_REGISTER_GYRO_CONFIG, &gyro_config);
  ESP_LOGV(TAG, "    Input gyro_config: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(gyro_config));
  gyro_config &= 0b11100111;
  gyro_config |= MPU6050_SCALE_2000_DPS << 3;
  ESP_LOGV(TAG, "    Output gyro_config: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(gyro_config));
  this->write_byte(MPU6050_REGISTER_GYRO_CONFIG, gyro_config);

  ESP_LOGV(TAG, "    Setting up Accel Config...");
  // Set range - 2G
  uint8_t accel_config;
  this->read_byte(MPU6050_REGISTER_ACCEL_CONFIG, &accel_config);
  ESP_LOGV(TAG, "    Input accel_config: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(accel_config));
  accel_config &= 0b11100111;
  accel_config |= (MPU6050_RANGE_2G << 3);
  ESP_LOGV(TAG, "    Output accel_config: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(accel_config));
  this->write_byte(MPU6050_REGISTER_GYRO_CONFIG, gyro_config);
}

void MPU6050Component::update() {
  ESP_LOGV(TAG, "    Updating MPU6050...");
  uint16_t data[7];
  if (!this->read_bytes_16(MPU6050_REGISTER_ACCEL_XOUT_H, data, 7))
    return;

  float accel_x = data[0] * MPU6050_RANGE_PER_DIGIT_2G * GRAVITY_EARTH;
  float accel_y = data[1] * MPU6050_RANGE_PER_DIGIT_2G * GRAVITY_EARTH;
  float accel_z = data[2] * MPU6050_RANGE_PER_DIGIT_2G * GRAVITY_EARTH;

  float temperature = data[3] / 340.0f + 36.53f;

  float gyro_x = data[4] * MPU6050_SCALE_DPS_PER_DIGIT_2000;
  float gyro_y = data[5] * MPU6050_SCALE_DPS_PER_DIGIT_2000;
  float gyro_z = data[6] * MPU6050_SCALE_DPS_PER_DIGIT_2000;

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

ESPHOMELIB_NAMESPACE_END

#endif //USE_MPUT6050
