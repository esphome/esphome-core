#ifndef ESPHOME_MPU_6050_COMPONENT_H
#define ESPHOME_MPU_6050_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_MPU6050

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"
#include "esphome/i2c_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

using MPU6050AccelSensor = EmptyPollingParentSensor<2, ICON_BRIEFCASE_DOWNLOAD, UNIT_M_PER_S_SQUARED>;
using MPU6050GyroSensor = EmptyPollingParentSensor<2, ICON_SCREEN_ROTATION, UNIT_DEGREES_PER_SECOND>;
using MPU6050TemperatureSensor = EmptyPollingParentSensor<1, ICON_EMPTY, UNIT_C>;

class MPU6050Component : public PollingComponent, public I2CDevice {
 public:
  explicit MPU6050Component(I2CComponent *parent, uint8_t address = 0x68, uint32_t update_interval = 60000);

  void setup() override;
  void dump_config() override;

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

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MPU6050

#endif  // ESPHOME_MPU_6050_COMPONENT_H
