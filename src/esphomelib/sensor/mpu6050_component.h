//
//  mpu6050_component.h
//  esphomelib
//
//  Created by Otto Winter on 05.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_MPU_6050_COMPONENT_H
#define ESPHOMELIB_MPU_6050_COMPONENT_H

#include "esphomelib/component.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/defines.h"
#include "esphomelib/i2c_component.h"

#ifdef USE_MPU6050

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

using MPU6050AccelSensor = EmptyPollingParentSensor<2, ICON_BRIEFCASE_DOWNLOAD, UNIT_M_PER_S_SQUARED>;
using MPU6050GyroSensor = EmptyPollingParentSensor<2, ICON_SCREEN_ROTATION, UNIT_DEGREES_PER_SECOND>;
using MPU6050TemperatureSensor = EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C>;

class MPU6050Component : public PollingComponent, public I2CDevice {
 public:
  explicit MPU6050Component(I2CComponent *parent, uint8_t address = 0x68, uint32_t update_interval = 15000);

  void setup() override;

  void update() override;

  float get_setup_priority() const override;

  MPU6050AccelSensor *make_accel_x_sensor(const std::string &name);
  MPU6050AccelSensor *make_accel_y_sensor(const std::string &name);
  MPU6050AccelSensor *make_accel_z_sensor(const std::string &name);
  MPU6050GyroSensor *make_gyro_x_sensor(const std::string &name);
  MPU6050GyroSensor *make_gyro_y_sensor(const std::string &name);
  MPU6050GyroSensor *make_gyro_z_sensor(const std::string &name);
  MPU6050TemperatureSensor *make_temperature_sensor(const std::string &name);

 protected:
  MPU6050AccelSensor *accel_x_sensor_{nullptr};
  MPU6050AccelSensor *accel_y_sensor_{nullptr};
  MPU6050AccelSensor *accel_z_sensor_{nullptr};
  MPU6050TemperatureSensor *temperature_sensor_{nullptr};
  MPU6050GyroSensor *gyro_x_sensor_{nullptr};
  MPU6050GyroSensor *gyro_y_sensor_{nullptr};
  MPU6050GyroSensor *gyro_z_sensor_{nullptr};
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_MPU6050

#endif //ESPHOMELIB_MPU_6050_COMPONENT_H
