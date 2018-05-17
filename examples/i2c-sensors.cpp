//
//  i2c-sensors.cpp
//  esphomelib
//
//  Created by Otto Winter on 25.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

// This class shows you how you can use various i2c sensors with esphomelib

#include "esphomelib/application.h"

using namespace esphomelib;

void setup() {
  App.set_name("outside");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  // This is required to set up the i2c bus. 21 is SDA pin and 2 is SCL pin.
  // Alternatively, you can just writ App.init_i2c(); to use the default i2c
  // pins on your board.
  // You only need to call this once.
  App.init_i2c(21, 22);

  // 0x48 is the default address when address pin is pulled low.
  auto *ads1115 = App.make_ads1115_component(0x48);
  App.register_sensor(ads1115->get_sensor("ADS1115 Voltage #1", sensor::ADS1115_MULTIPLEXER_P0_N1, sensor::ADS1115_GAIN_6P144));
  App.register_sensor(ads1115->get_sensor("ADS1115 Voltage #2", sensor::ADS1115_MULTIPLEXER_P0_NG, sensor::ADS1115_GAIN_1P024));
  App.make_bmp085_sensor("BMP085 Temperature", "BMP085 Pressure");
  App.make_htu21d_sensor("HTU21D Temperature", "HTU21D Humidity");
  App.make_hdc1080_sensor("HDC1080 Temperature", "HDC1080 Humidity");

  auto *mpu6050 = App.make_mpu6050_sensor();
  App.register_sensor(mpu6050->make_accel_x_sensor("MPU6050 Accel X"));
  App.register_sensor(mpu6050->make_accel_y_sensor("MPU6050 Accel Y"));
  App.register_sensor(mpu6050->make_accel_z_sensor("MPU6050 Accel Z"));
  App.register_sensor(mpu6050->make_gyro_x_sensor("MPU6050 Gyro X"));
  App.register_sensor(mpu6050->make_gyro_y_sensor("MPU6050 Gyro Y"));
  App.register_sensor(mpu6050->make_gyro_z_sensor("MPU6050 Gyro Z"));
  App.register_sensor(mpu6050->make_temperature_sensor("MPU6050 Temperature"));

  auto tsl2561 = App.make_tsl2561_sensor("TSL2561 Illuminance Sensor");
  // set the time the sensor will take for value accumulation, default: 402 ms
  tsl2561.tsl2561->set_integration_time(sensor::TSL2561_INTEGRATION_14MS);
  // set a higher gain for low light conditions, default: 1x
  tsl2561.tsl2561->set_gain(sensor::TSL2561_GAIN_16X);

  auto bh1750 = App.make_bh1750_sensor("BH1750 Illuminance");
  // default resolution is 0.5 LX
  bh1750.bh1750->set_resolution(sensor::BH1750_RESOLUTION_1P0_LX);

  auto bme280 = App.make_bme280_sensor("BME280 Temperature", "BME280 Pressure", "BME280 Humidity");
  // set infinite impulse response filter, default is OFF.
  bme280.bme280->set_iir_filter(sensor::BME280_IIR_FILTER_4X);
  // set over value sampling, default is 16x
  bme280.bme280->set_temperature_oversampling(sensor::BME280_OVERSAMPLING_16X);
  bme280.bme280->set_humidity_oversampling(sensor::BME280_OVERSAMPLING_4X);
  bme280.bme280->set_pressure_oversampling(sensor::BME280_OVERSAMPLING_16X);

  auto bme680 = App.make_bme680_sensor("BME680 Temperature", "BME680 Pressure", "BME680 Humidity", "BME680 Gas Resistance");
  // default is no iir filter
  bme680.bme680->set_iir_filter(sensor::BME680_IIR_FILTER_15X);
  // set heater to 200°C for 100ms, default is off
  bme680.bme680->set_heater(200, 100);

  auto sht3xd = App.make_sht3xd_sensor("SHT31D Temperature", "SHT31D Humidity");
  // default accuracy is high
  sht3xd.sht3xd->set_accuracy(sensor::SHT3XD_ACCURACY_LOW);

  App.make_dht12_sensor("DHT12 Temperature", "DHT12 Humidity");

  App.setup();
}

void loop() {
  App.loop();
  delay(10); // this keeps the ESP32/ESP8266 from wasting power.
}
