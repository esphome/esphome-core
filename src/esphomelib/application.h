//
// Created by Otto Winter on 25.11.17.
//

#ifndef ESPHOMELIB_APPLICATION_H
#define ESPHOMELIB_APPLICATION_H

#include <vector>
#include <WiFiClient.h>
#include "esphomelib/output/ledc_output_component.h"
#include "esphomelib/output/pca9685_output_component.h"
#include "esphomelib/light/mqtt_json_light_component.h"
#include "esphomelib/input/gpio_binary_sensor_component.h"
#include "esphomelib/binary_sensor/mqtt_binary_sensor_component.h"
#include "esphomelib/sensor/mqtt_sensor_component.h"
#include "esphomelib/input/dht_component.h"
#include "esphomelib/light/light_output_component.h"
#include "esphomelib/switch_platform/mqtt_switch_component.h"
#include "esphomelib/switch_platform/switch.h"
#include "esphomelib/output/ir_transmitter_component.h"
#include "esphomelib/input/ultrasonic_sensor.h"
#include "esphomelib/input/dallas_component.h"
#include "esphomelib/switch_platform/simple_switch.h"
#include "esphomelib/fan/mqtt_fan_component.h"
#include "esphomelib/fan/basic_fan_component.h"
#include "esphomelib/output/gpio_binary_output_component.h"
#include "esphomelib/input/pulse_counter.h"
#include "esphomelib/input/adc_sensor_component.h"
#include "esphomelib/component.h"
#include "esphomelib/mqtt/mqtt_client_component.h"
#include "esphomelib/wifi_component.h"
#include "esphomelib/log_component.h"
#include "esphomelib/power_supply_component.h"
#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/sensor/sensor.h"
#include "esphomelib/ota_component.h"
#include "esphomelib/log.h"

namespace esphomelib {

/** This is the class that combines all components.
 *
 * Firstly, this class is used the register component
 */
class Application {
 public:
  /** Set the name of the item that is running this app.
   *
   * Note: This will automatically be converted to lowercase_underscore.
   *
   * @param name The name of your app.
   */
  void set_name(const std::string &name);

  /** Initialize the log system.
   *
   * @param baud_rate The serial baud rate. Set to 0 to disable UART debugging.
   * @param mqtt_topic The MQTT debug topic. Set to "" to for default topic, and disable the optional for disabling this feature.
   * @param tx_buffer_size The size of the printf buffer.
   * @return The created and initialized LogComponent.
   */
  LogComponent *init_log(uint32_t baud_rate = 115200,
                         const Optional<std::string> &mqtt_topic = Optional<std::string>(""),
                         size_t tx_buffer_size = 512);

  /** Initialize the WiFi system.
   *
   * Note: for advanced options, such as manual ip, use the return value.
   *
   * @param ssid The ssid of the network you want to connect to.
   * @param password The password of your network.
   * @return The WiFiComponent.
   */
  WiFiComponent *init_wifi(const std::string &ssid, const std::string &password);

  /** Initialize Over-the-Air updates.
   *
   * @return The OTAComponent. Use this to set advanced settings.
   */
  OTAComponent *init_ota();

  /** Initialize the MQTT client.
   *
   * @param address The address of your server.
   * @param port The port of your server.
   * @param username The username.
   * @param password The password. Empty for no password.
   * @param discovery_prefix The discovery prefix for home assistant. Leave empty for no discovery.
   * @return The MQTTClient. Use this to set advanced settings.
   */
  mqtt::MQTTClientComponent *init_mqtt(const std::string &address, uint16_t port,
                                       const std::string &username, const std::string &password,
                                       const std::string &discovery_prefix = "homeassistant");

  /** Initialize the MQTT client.
   *
   * @param address The address of your server.
   * @param username The username.
   * @param password The password. Empty for no password.
   * @param discovery_prefix The discovery prefix for home assistant. Leave empty for no discovery.
   * @return The MQTTClient. Use this to set advanced settings.
   */
  mqtt::MQTTClientComponent *init_mqtt(const std::string &address,
                                       const std::string &username, const std::string &password,
                                       const std::string &discovery_prefix = "homeassistant");

  /** Create a power supply component that will automatically switch on and off.
   *
   * @param pin The pin the power supply is connected to.
   * @param enable_time The time (in ms) the power supply needs until it can provide high power when powering on.
   * @param keep_on_time The time (in ms) the power supply should stay on when it is not used.
   * @return The PowerSupplyComponent.
   */
  PowerSupplyComponent *make_power_supply(GPIOOutputPin pin, uint32_t enable_time = 20,
                                          uint32_t keep_on_time = 10000);





  /*   ____ ___ _   _    _    ______   __  ____  _____ _   _ ____   ___  ____
   *  | __ |_ _| \ | |  / \  |  _ \ \ / / / ___|| ____| \ | / ___| / _ \|  _ \
   *  |  _ \| ||  \| | / _ \ | |_) \ V /  \___ \|  _| |  \| \___ \| | | | |_) |
   *  | |_) | || |\  |/ ___ \|  _ < | |    ___) | |___| |\  |___) | |_| |  _ <
   *  |____|___|_| \_/_/   \_|_| \_\|_|   |____/|_____|_| \_|____/ \___/|_| \_\
   */
  /// Create a MQTTBinarySensorComponent for a specific BinarySensor. Mostly for internal use.
  binary_sensor::MQTTBinarySensorComponent *make_mqtt_binary_sensor_for(std::string friendly_name,
                                                                        std::string device_class,
                                                                        binary_sensor::BinarySensor *binary_sensor);

  struct SimpleBinarySensor {
    input::GPIOBinarySensorComponent *gpio;
    binary_sensor::MQTTBinarySensorComponent *mqtt;
  };

  /** Create a simple GPIO binary sensor.
   *
   * Note: advanced options such as inverted input are available in the return value.
   *
   * @param pin The GPIO pin.
   * @param friendly_name The friendly name that should be advertised. Leave empty for no automatic discovery.
   * @param device_class The Home Assistant <a href="https://home-assistant.io/components/binary_sensor/">device_class</a>.
   *                     or esphomelib::binary_sensor::device_class
   */
  SimpleBinarySensor make_gpio_binary_sensor(GPIOInputPin pin,
                                             std::string friendly_name,
                                             std::string device_class = "");





  /*   ____  _____ _   _ ____   ___  ____
   *  / ___|| ____| \ | / ___| / _ \|  _ \
   *  \___ \|  _| |  \| \___ \| | | | |_) |
   *   ___) | |___| |\  |___) | |_| |  _ <
   *  |____/|_____|_| \_|____/ \___/|_| \_\
   */
  /// Create a MQTTSensorComponent for the provided Sensor and connect them. Mostly for internal use.
  sensor::MQTTSensorComponent *make_mqtt_sensor_for(sensor::Sensor *sensor, std::string friendly_name);

  struct MakeDHTComponent {
    input::DHTComponent *dht;
    sensor::MQTTSensorComponent *mqtt_temperature;
    sensor::MQTTSensorComponent *mqtt_humidity;
  };

  /** Create a DHT sensor component.
   *
   * Note: This method automatically applies a SlidingWindowMovingAverageFilter.
   *
   * @param pin The pin the DHT sensor is connected to.
   * @param temperature_friendly_name The name the temperature sensor should be advertised as. Leave empty for no
   *                                  automatic discovery.
   * @param humidity_friendly_name The name the humidity sensor should be advertised as. Leave empty for no
   *                                  automatic discovery.
   * @param update_interval The interval (in ms) the sensor should be checked.
   * @return The components. Use this for advanced settings.
   */
  MakeDHTComponent make_dht_sensor(uint8_t pin,
                                   const std::string &temperature_friendly_name,
                                   const std::string &humidity_friendly_name,
                                   uint32_t update_interval = 15000);

  input::DallasComponent *make_dallas_component(OneWire *one_wire);

  input::DallasComponent *make_dallas_component(uint8_t pin);

#ifdef ARDUINO_ARCH_ESP32
  struct MakePulseCounter {
    input::PulseCounterSensorComponent *pcnt;
    sensor::MQTTSensorComponent *mqtt;
  };

  /** Create an ESP32 Pulse Counter component.
   *
   * The pulse counter peripheral will automatically all pulses on pin in the background. Every
   * check_interval ms the amount of pulses will be retrieved and the difference to the last value
   * will be reported via MQTT as a sensor.
   *
   * @param pin The pin the pulse counter should count pulses on.
   * @param friendly_name The name the sensor should be advertised as.
   * @param update_interval The interval in ms the sensor should be checked.
   * @return The components. Use this for advanced settings.
   */
  MakePulseCounter make_pulse_counter_sensor(uint8_t pin,
                                             const std::string &friendly_name,
                                             uint32_t update_interval = 15000);
#endif

  struct MakeADCSensor {
    input::ADCSensorComponent *adc;
    sensor::MQTTSensorComponent *mqtt;
  };

  /** Create an ADC Sensor component.
   *
   * Every check_interval ms, the value from the specified pin (only A0 on ESP8266, 32-39 for ESP32),
   * and converts it into the volt unit. On the ESP32 you can additionally specify a channel attenuation
   * using the return value of this function. pinMode can also be set using the return value.
   *
   * @param pin The pin the ADC should sense on.
   * @param friendly_name The name the sensor should be advertised as.
   * @param update_interval The interval in ms the sensor should be checked.
   * @return The components. Use this for advanced settings.
   */
  MakeADCSensor make_adc_sensor(uint8_t pin,
                                const std::string &friendly_name,
                                uint32_t update_interval = 15000);

  struct MakeUltrasonicSensor {
    input::UltrasonicSensorComponent *ultrasonic;
    sensor::MQTTSensorComponent *mqtt;
  };

  /** Create an Ultrasonic range sensor.
   *
   * This can for example be an HC-SR04 ultrasonic sensor. It sends out a short ultrasonic wave and listens
   * for an echo. The time between the sending and receiving is then (with some maths) converted to a measurement
   * in meters. You need to specify the trigger pin (where to short pulse will be sent to) and the echo pin
   * (where we're waiting for the echo). Note that in order to not block indefinitely if we don't receive an
   * echo, this class has a default timeout of around 2m. You can change that using the return value of this
   * function.
   *
   * @param trigger_pin The pin the short pulse will be sent to, can be integer or GPIOOutputPin.
   * @param echo_pin The pin we wait that we wait on for the echo, can be integer or GPIOInputPin.
   * @param friendly_name The friendly name for this sensor advertised to Home Assistant.
   * @param update_interval The time in ms between updates, defaults to 5 seconds.
   * @return The Ultrasonic sensor + MQTT sensor pair, use this for advanced settings.
   */
  MakeUltrasonicSensor make_ultrasonic_sensor(GPIOOutputPin trigger_pin, GPIOInputPin echo_pin,
                                              const std::string &friendly_name,
                                              uint32_t update_interval = 5000);






  /*    ___  _   _ _____ ____  _   _ _____
   *   / _ \| | | |_   _|  _ \| | | |_   _|
   *  | | | | | | | | | | |_) | | | | | |
   *  | |_| | |_| | | | |  __/| |_| | | |
   *   \___/ \___/  |_| |_|    \___/  |_|
   */
#ifdef ARDUINO_ARCH_ESP32
  /** Create a ESP32 LEDC channel.
   *
   * @param pin The pin.
   * @param frequency The PWM frequency.
   * @param bit_depth The LEDC bit depth.
   * @return The LEDC component. Use this for advanced settings.
   */
  output::LEDCOutputComponent *make_ledc_output(uint8_t pin, float frequency = 1000.0f, uint8_t bit_depth = 12);
#endif

  /** Create a PCA9685 component.
   *
   * @param frequency The PWM frequency.
   * @param i2c_wire The i2c interface.
   * @return The PCA9685 component. Use this for advanced settings.
   */
  output::PCA9685OutputComponent *make_pca9685_component(float frequency, TwoWire &i2c_wire = Wire);

  /** Create a simple binary GPIO output component.
   *
   * Note: This is *only* a binary output component, not a switch that will be exposed
   * in Home Assistant. See make_simple_gpio_switch for a switch.
   *
   * @param pin The GPIO pin.
   * @return The GPIOBinaryOutputComponent. Use this for advanced settings.
   */
  output::GPIOBinaryOutputComponent *make_gpio_output(GPIOOutputPin pin);





  /*   _     ___ ____ _   _ _____
   *  | |   |_ _/ ___| | | |_   _|
   *  | |    | | |  _| |_| | | |
   *  | |___ | | |_| |  _  | | |
   *  |_____|___\____|_| |_| |_|
   */
  /// Create a MQTTJSONLightComponent. Mostly for internal use.
  light::MQTTJSONLightComponent *make_mqtt_light_(const std::string &friendly_name, light::LightState *state);

  struct LightStruct {
    light::LinearLightOutputComponent *output;
    light::LightState *state;
    light::MQTTJSONLightComponent *mqtt;
  };

  /// Create and connect a MQTTJSONLightComponent to the provided light output component. Mostly for internal use.
  LightStruct connect_light_(const std::string &friendly_name, light::LinearLightOutputComponent *out);

  /** Create a binary light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param binary The binary output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  LightStruct make_binary_light(const std::string &friendly_name, output::BinaryOutput *binary);

  /** Create a monochromatic light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param mono The output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  LightStruct make_monochromatic_light(const std::string &friendly_name, output::FloatOutput *mono);

  /** Create a RGB light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param red The red output channel.
   * @param green The green output channel.
   * @param blue The blue output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  LightStruct make_rgb_light(const std::string &friendly_name,
                             output::FloatOutput *red, output::FloatOutput *green, output::FloatOutput *blue);

  /** Create a RGBW light.
   *
   * @param friendly_name The name the light should be advertised as. Leave empty for no automatic discovery.
   * @param red The red output channel.
   * @param green The green output channel.
   * @param blue The blue output channel.
   * @param white The white output channel.
   * @return The components for this light. Use this for advanced settings.
   */
  LightStruct make_rgbw_light(const std::string &friendly_name,
                              output::FloatOutput *red, output::FloatOutput *green, output::FloatOutput *blue,
                              output::FloatOutput *white);





  /*   ______        _____ _____ ____ _   _
   *  / ___\ \      / |_ _|_   _/ ___| | | |
   *  \___ \\ \ /\ / / | |  | || |   | |_| |
   *   ___) |\ V  V /  | |  | || |___|  _  |
   *  |____/  \_/\_/  |___| |_| \____|_| |_|
   */
#ifdef ARDUINO_ARCH_ESP32
  /** Create an IR transmitter.
   *
   * @param pin The pin the IR led is connected to.
   * @param carrier_duty_percent The duty cycle of the IR output. Decrease this if your LED gets hot.
   * @param clock_divider The clock divider for the rmt peripheral.
   * @return The IRTransmitterComponent. Use this for advanced settings.
   */
  output::IRTransmitterComponent *make_ir_transmitter_component(uint8_t pin,
                                                                uint8_t carrier_duty_percent = 50,
                                                                uint8_t clock_divider = output::DEFAULT_CLOCK_DIVIDER);
#endif

  struct GPIOSwitchStruct {
    output::GPIOBinaryOutputComponent *gpio;
    switch_platform::MQTTSwitchComponent *mqtt;
  };

  /** Create a simple GPIO switch that can be toggled on/off and appears in the Home Assistant frontend.
   *
   * @param pin The pin used for this switch. Can be integer or GPIOOutputPin.
   * @param friendly_name The friendly name advertised to Home Assistant for this switch-
   * @return A GPIOSwitchStruct, use this to set advanced settings.
   */
  GPIOSwitchStruct make_gpio_switch(GPIOOutputPin pin, const std::string &friendly_name);

  /// Create a MQTTSwitchComponent for the provided Switch.
  switch_platform::MQTTSwitchComponent *make_mqtt_switch_for(const std::string &friendly_name,
                                                             switch_platform::Switch *switch_);





  /*   _____ _    _   _
   *  |  ___/ \  | \ | |
   *  | |_ / _ \ |  \| |
   *  |  _/ ___ \| |\  |
   *  |_|/_/   \_|_| \_|
   */
  struct FanStruct {
    fan::BasicFanComponent *output;
    fan::FanState *state;
    fan::MQTTFanComponent *mqtt;
  };

  /** Create and connect a Fan with the specified friendly name.
   *
   * @param friendly_name The friendly name of the Fan to advertise.
   * @return A FanStruct, use the output field to set your output channels.
   */
  FanStruct make_fan(const std::string &friendly_name);





  /*   _   _ _____ _     ____  _____ ____  ____
   *  | | | | ____| |   |  _ \| ____|  _ \/ ___|
   *  | |_| |  _| | |   | |_) |  _| | |_) \___ \
   *  |  _  | |___| |___|  __/| |___|  _ < ___) |
   *  |_| |_|_____|_____|_|   |_____|_| \_|____/
   */
  template<class C>
  C *register_mqtt_component(C *c);

  /// Register the component in this Application instance.
  template<class C>
  C *register_component(C *c);

  /// Set up all the registered components. Call this at the end of your setup() function.
  void setup();

  /// Make a loop iteration. Call this in your loop() function.
  void loop();

  WiFiComponent *get_wifi() const;
  mqtt::MQTTClientComponent *get_mqtt_client() const;

  /// Assert that name has been set.
  void assert_name() const;

  /// Get the name of this Application set by set_name().
  const std::string &get_name() const;

 protected:
  std::vector<Component *> components_;
  mqtt::MQTTClientComponent *mqtt_client_;
  WiFiComponent *wifi_;

  std::string name_;
  Component::ComponentState application_state_{Component::CONSTRUCTION};
};

/// Global storage of Application pointer - only one Application can exist.
extern Application App;

template<class C>
C *Application::register_component(C *c) {
  Component *component = c;
  this->components_.push_back(component);
  return c;
}

template<class C>
C *Application::register_mqtt_component(C *c) {
  mqtt::MQTTComponent *component = c;
  if (mqtt::global_mqtt_client->get_use_status_messages())
    component->set_availability(mqtt::Availability{
        .topic = mqtt::global_mqtt_client->get_default_status_message_topic(),
        .payload_available = "online",
        .payload_not_available = "offline"
    });
  return this->register_component(c);
}

} // namespace esphomelib

#endif //ESPHOMELIB_APPLICATION_H
