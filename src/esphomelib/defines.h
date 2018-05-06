//
//  defines.h
//  esphomelib
//
//  Created by Otto Winter on 12.04.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_DEFINES_H
#define ESPHOMELIB_DEFINES_H

#define ESPHOMELIB_VERSION "1.4.0"

#ifndef ESPHOMEYAML_USE
  #define USE_OTA
  #define USE_I2C
  #define USE_BINARY_SENSOR
  #define USE_GPIO_BINARY_SENSOR
  #define USE_STATUS_BINARY_SENSOR
  #define USE_SENSOR
  #define USE_DHT_SENSOR
  #define USE_DALLAS_SENSOR
  #ifdef ARDUINO_ARCH_ESP32
    #define USE_PULSE_COUNTER_SENSOR
  #endif
  #define USE_ADC_SENSOR
  #define USE_ADS1115_SENSOR
  #define USE_BMP085_SENSOR
  #define USE_HTU21D_SENSOR
  #define USE_HDC1080_SENSOR
  #define USE_ULTRASONIC_SENSOR
  #define USE_OUTPUT
  #ifdef ARDUINO_ARCH_ESP32
    #define USE_LEDC_OUTPUT
  #endif
  #define USE_PCA9685_OUTPUT
  #define USE_GPIO_OUTPUT
  #ifdef ARDUINO_ARCH_ESP8266
    #define USE_ESP8266_PWM_OUTPUT
  #endif
  #define USE_LIGHT
  #define USE_SWITCH
  #define USE_SIMPLE_SWITCH
  #ifdef ARDUINO_ARCH_ESP32
    #define USE_IR_TRANSMITTER
  #endif
  #define USE_GPIO_SWITCH
  #define USE_RESTART_SWITCH
  #define USE_SHUTDOWN_SWITCH
  #define USE_FAN
  #define USE_DEBUG_COMPONENT
  #define USE_WEB_SERVER
  #define USE_DEEP_SLEEP
  #define USE_PCF8574
  #define USE_IO
  #define USE_MPU6050
#endif

#ifdef USE_GPIO_BINARY_SENSOR
  #ifndef USE_BINARY_SENSOR
    #define USE_BINARY_SENSOR
  #endif
#endif
#ifdef USE_STATUS_BINARY_SENSOR
  #ifndef USE_BINARY_SENSOR
    #define USE_BINARY_SENSOR
  #endif
#endif
#ifdef USE_DHT_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_DALLAS_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_ONE_WIRE
    #define USE_ONE_WIRE
  #endif
#endif
#ifdef USE_PULSE_COUNTER_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_ADC_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_ADS1115_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_BMP085_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_BMP085_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_HTU21D_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_HDC1080_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_MPU6050
  #ifndef USE_SENSOR
    #define SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_ULTRASONIC_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_IR_TRANSMITTER
  #ifndef USE_SWITCH
    #define USE_SWITCH
  #endif
#endif
#ifdef USE_GPIO_SWITCH
  #ifndef USE_SIMPLE_SWITCH
    #define USE_SIMPLE_SWITCH
  #endif
  #ifndef USE_GPIO_OUTPUT
    #define USE_GPIO_OUTPUT
  #endif
#endif
#ifdef USE_RESTART_SWITCH
  #ifndef USE_SWITCH
    #define USE_SWITCH
  #endif
#endif
#ifdef USE_LIGHT
  #ifndef USE_OUTPUT
    #define USE_OUTPUT
  #endif
#endif
#ifdef USE_SIMPLE_SWITCH
  #ifndef USE_SWITCH
    #define USE_SWITCH
  #endif
  #ifndef USE_OUTPUT
    #define USE_OUTPUT
  #endif
#endif
#ifdef USE_LEDC_OUTPUT
  #ifndef USE_OUTPUT
    #define USE_OUTPUT
  #endif
#endif
#ifdef USE_PCA9685_OUTPUT
  #ifndef USE_OUTPUT
    #define USE_OUTPUT
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_GPIO_OUTPUT
  #ifndef USE_OUTPUT
    #define USE_OUTPUT
  #endif
#endif
#ifdef USE_ESP8266_PWM_OUTPUT
  #ifndef USE_OUTPUT
    #define USE_OUTPUT
  #endif
#endif
#ifdef USE_PCF8574
  #ifndef USE_I2C
    #define USE_I2C
  #endif
  #ifndef USE_IO
    #define USE_IO
  #endif
#endif


#endif //ESPHOMELIB_DEFINES_H
