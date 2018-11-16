#ifndef ESPHOMELIB_DEFINES_H
#define ESPHOMELIB_DEFINES_H

#define ESPHOMELIB_VERSION "1.10.0-dev"

#define HOT __attribute__ ((hot))
#define ESPDEPRECATED(msg) __attribute__((deprecated(msg)))

#ifndef DOXYGEN
  #define ESPHOMELIB_NAMESPACE_BEGIN namespace esphomelib {
  #define ESPHOMELIB_NAMESPACE_END }
  #define ESPHOMELIB_NAMESPACE esphomelib
#else
  #define ESPHOMELIB_NAMESPACE_BEGIN /##/
  #define ESPHOMELIB_NAMESPACE_END /##/
  #define ESPHOMELIB_NAMESPACE
#endif

#ifndef ESPHOMEYAML_USE
  #define USE_OTA
  #define USE_I2C
  #define USE_BINARY_SENSOR
  #define USE_GPIO_BINARY_SENSOR
  #define USE_STATUS_BINARY_SENSOR
  #define USE_SENSOR
  #define USE_DHT_SENSOR
  #define USE_DHT12_SENSOR
  #define USE_DALLAS_SENSOR
  #define USE_PULSE_COUNTER_SENSOR
  #define USE_ADC_SENSOR
  #define USE_ADS1115_SENSOR
  #define USE_BMP085_SENSOR
  #define USE_HTU21D_SENSOR
  #define USE_HDC1080_SENSOR
  #define USE_ULTRASONIC_SENSOR
  #define USE_WIFI_SIGNAL_SENSOR
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
  #define USE_OUTPUT_SWITCH
  #define USE_REMOTE
  #define USE_REMOTE_RECEIVER
  #define USE_REMOTE_TRANSMITTER
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
  #define USE_TSL2561
  #define USE_BH1750
  #define USE_BME280
  #define USE_BMP280
  #define USE_BME680
  #define USE_SHT3XD
  #define USE_TEMPLATE_SENSOR
  #ifdef ARDUINO_ARCH_ESP32
    #define USE_ESP32_TOUCH_BINARY_SENSOR
  #endif
  #ifdef ARDUINO_ARCH_ESP32
    #define USE_ESP32_BLE_TRACKER
    #define USE_ESP32_BLE_BEACON
  #endif
  #define USE_FAST_LED_LIGHT
  #define USE_ROTARY_ENCODER_SENSOR
  #define USE_MAX6675_SENSOR
  #define USE_TEMPLATE_BINARY_SENSOR
  #define USE_TEMPLATE_SWITCH
  #define USE_COVER
  #define USE_TEMPLATE_COVER
  #ifdef ARDUINO_ARCH_ESP32
    #define USE_ESP32_HALL_SENSOR
  #endif
  #define USE_DUTY_CYCLE_SENSOR
  #define USE_STATUS_LED
  #define USE_SPI
  #define USE_PN532
  #define USE_UART
  #define USE_MHZ19
  #define USE_UART_SWITCH
  #define USE_UPTIME_SENSOR
  #define USE_INA219
  #define USE_INA3221
  #define USE_HMC5883L
  #define USE_RDM6300
  #define USE_MS5611
  #define USE_HX711
  #define USE_TCS34725
  #define USE_MAX7219
  #define USE_LCD_DISPLAY
  #define USE_LCD_DISPLAY_PCF8574
  #define USE_SSD1306
  #define USE_WAVESHARE_EPAPER
  #define USE_DISPLAY
  #define USE_TIME
  #define USE_SNTP_COMPONENT
  #define USE_NEXTION
  #define USE_HLW8012
  #define USE_TEXT_SENSOR
  #define USE_MQTT_SUBSCRIBE_TEXT_SENSOR
  #define USE_VERSION_TEXT_SENSOR
  #define USE_TEMPLATE_TEXT_SENSOR
  #define USE_MQTT_SUBSCRIBE_SENSOR
  #define USE_CSE7766
  #define USE_PMSX003
  #define USE_STEPPER
  #define USE_A4988
  #define USE_TOTAL_DAILY_ENERGY_SENSOR
  #define USE_MY9231_OUTPUT
  #define USE_CUSTOM_SENSOR
#endif

#ifdef USE_REMOTE_RECEIVER
  #ifndef USE_REMOTE
    #define USE_REMOTE
  #endif
  #ifndef USE_BINARY_SENSOR
    #define USE_BINARY_SENSOR
  #endif
#endif
#ifdef USE_REMOTE_TRANSMITTER
  #ifndef USE_REMOTE
    #define USE_REMOTE
  #endif
  #ifndef USE_SWITCH
    #define USE_SWITCH
  #endif
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
#ifdef USE_ESP32_TOUCH_BINARY_SENSOR
  #ifndef USE_BINARY_SENSOR
    #define USE_BINARY_SENSOR
  #endif
#endif
#ifdef USE_ESP32_BLE_TRACKER
  #ifndef USE_BINARY_SENSOR
    #define USE_BINARY_SENSOR
  #endif
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_TEMPLATE_BINARY_SENSOR
  #ifndef USE_BINARY_SENSOR
    #define USE_BINARY_SENSOR
  #endif
#endif
#ifdef USE_PN532
  #ifndef USE_BINARY_SENSOR
    #define USE_BINARY_SENSOR
  #endif
  #ifndef USE_SPI
    #define USE_SPI
  #endif
#endif
#ifdef USE_MAX7219
  #ifndef USE_SPI
    #define USE_SPI
  #endif
  #ifndef USE_DISPLAY
    #define USE_DISPLAY
  #endif
#endif
#ifdef USE_RDM6300
  #ifndef USE_BINARY_SENSOR
    #define USE_BINARY_SENSOR
  #endif
  #ifndef USE_UART
    #define USE_UART
  #endif
#endif
#ifdef USE_MHZ19
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_UART
    #define USE_UART
  #endif
#endif
#ifdef USE_TCS34725
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_UART_SWITCH
  #ifndef USE_SWITCH
    #define USE_SWITCH
  #endif
  #ifndef USE_UART
    #define USE_UART
  #endif
#endif
#ifdef USE_UPTIME_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_HX711
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_DHT_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_DHT12_SENSOR
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
#ifdef USE_DUTY_CYCLE_SENSOR
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
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_INA219
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_INA3221
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_HMC5883L
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_MS5611
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_ESP32_HALL_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_MAX6675_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_TSL2561
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_BH1750
  #ifndef USE_SENSOR
  #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_BME280
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_BME680
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_SHT3XD
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_TEMPLATE_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_ULTRASONIC_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_WIFI_SIGNAL_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_ROTARY_ENCODER_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_GPIO_SWITCH
  #ifndef USE_SWITCH
    #define USE_SWITCH
  #endif
#endif
#ifdef USE_TEMPLATE_SWITCH
  #ifndef USE_SWITCH
    #define USE_SWITCH
  #endif
#endif
#ifdef USE_RESTART_SWITCH
  #ifndef USE_SWITCH
    #define USE_SWITCH
  #endif
#endif
#ifdef USE_FAST_LED_LIGHT
  #ifndef USE_LIGHT
    #define USE_LIGHT
  #endif
#endif
#ifdef USE_LIGHT
  #ifndef USE_OUTPUT
    #define USE_OUTPUT
  #endif
#endif
#ifdef USE_OUTPUT_SWITCH
  #ifndef USE_SWITCH
    #define USE_SWITCH
  #endif
  #ifndef USE_OUTPUT
    #define USE_OUTPUT
  #endif
#endif
#ifdef USE_SWITCH
  #ifndef USE_BINARY_SENSOR
    #define USE_BINARY_SENSOR
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
#ifdef USE_TEMPLATE_COVER
  #ifndef USE_COVER
    #define USE_COVER
  #endif
#endif
#ifdef USE_LCD_DISPLAY_PCF8574
  #ifndef USE_LCD_DISPLAY
    #define USE_LCD_DISPLAY
  #endif
  #ifndef USE_I2C
    #define USE_I2C
  #endif
#endif
#ifdef USE_LCD_DISPLAY
  #ifndef USE_DISPLAY
    #define USE_DISPLAY
  #endif
#endif
#ifdef USE_SSD1306
  #ifndef USE_DISPLAY
    #define USE_DISPLAY
  #endif
#endif
#ifdef USE_WAVESHARE_EPAPER
  #ifndef USE_DISPLAY
    #define USE_DISPLAY
  #endif
#endif
#ifdef USE_SNTP_COMPONENT
  #ifndef USE_TIME
    #define USE_TIME
  #endif
#endif
#ifdef USE_HLW8012
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_PULSE_COUNTER_SENSOR
    #define USE_PULSE_COUNTER_SENSOR
  #endif
#endif
#ifdef USE_NEXTION
  #ifndef USE_DISPLAY
    #define USE_DISPLAY
  #endif
  #ifndef USE_UART
    #define USE_UART
  #endif
#endif
#ifdef USE_MQTT_SUBSCRIBE_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif
#ifdef USE_CSE7766
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_UART
    #define USE_UART
  #endif
#endif
#ifdef USE_PMSX003
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_UART
    #define USE_UART
  #endif
#endif

#ifdef USE_MQTT_SUBSCRIBE_TEXT_SENSOR
  #ifndef USE_TEXT_SENSOR
    #define USE_TEXT_SENSOR
  #endif
#endif
#ifdef USE_VERSION_TEXT_SENSOR
  #ifndef USE_TEXT_SENSOR
    #define USE_TEXT_SENSOR
  #endif
#endif
#ifdef USE_TEMPLATE_TEXT_SENSOR
  #ifndef USE_TEXT_SENSOR
    #define USE_TEXT_SENSOR
  #endif
#endif

#ifdef USE_A4988
  #ifndef USE_STEPPER
    #define USE_STEPPER
  #endif
#endif

#ifdef USE_TOTAL_DAILY_ENERGY_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
  #ifndef USE_TIME
    #define USE_TIME
  #endif
#endif

#ifdef USE_MY9231_OUTPUT
  #ifndef USE_OUTPUT
    #define USE_OUTPUT
  #endif
#endif

#ifndef USE_CUSTOM_SENSOR
  #ifndef USE_SENSOR
    #define USE_SENSOR
  #endif
#endif

#endif //ESPHOMELIB_DEFINES_H
