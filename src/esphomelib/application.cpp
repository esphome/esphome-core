//
// Created by Otto Winter on 25.11.17.
//

#include "esphomelib/application.h"

#include <utility>
#include <algorithm>

#include "esphomelib/log.h"
#include "esphomelib/output/gpio_binary_output_component.h"
#include "esphomelib/esppreferences.h"
#include "esphomelib/wifi_component.h"

namespace esphomelib {

using namespace esphomelib::mqtt;
#ifdef USE_BINARY_SENSOR
using namespace esphomelib::binary_sensor;
#endif
#ifdef USE_SENSOR
using namespace esphomelib::sensor;
#endif
#ifdef USE_OUTPUT
using namespace esphomelib::output;
#endif
#ifdef USE_LIGHT
using namespace esphomelib::light;
#endif
#ifdef USE_FAN
using namespace esphomelib::fan;
#endif
#ifdef USE_SWITCH
using namespace esphomelib::switch_;
#endif

static const char *TAG = "application";

void Application::setup() {
  ESP_LOGI(TAG, "Application::setup()");
  assert(this->application_state_ == Component::CONSTRUCTION && "setup() called twice.");
  ESP_LOGV(TAG, "Sorting components by setup priority...");
  std::stable_sort(this->components_.begin(), this->components_.end(), [](const Component *a, const Component *b) {
    return a->get_setup_priority() > b->get_setup_priority();
  });
  ESP_LOGV(TAG, "Calling setup");
  for (Component *component : this->components_) {
    if (component->get_component_state() != Component::FAILED)
      component->setup_();
  }

  ESP_LOGV(TAG, "Sorting components by loop priority...");
  std::stable_sort(this->components_.begin(), this->components_.end(), [](const Component *a, const Component *b) {
    return a->get_loop_priority() > b->get_loop_priority();
  });
  this->application_state_ = Component::SETUP;
}

void Application::loop() {
  assert(this->application_state_ >= Component::SETUP && "Did you forget to call setup()?");

  bool first_loop = this->application_state_ == Component::SETUP;
  if (first_loop) {
    ESP_LOGI(TAG, "Running through first loop()");
    this->application_state_ = Component::LOOP;
  }

  for (Component *component : this->components_) {
    if (component->get_component_state() != Component::FAILED)
      component->loop_();
  }
  yield();

  if (first_loop)
    ESP_LOGI(TAG, "First loop finished successfully!");
}

WiFiComponent *Application::init_wifi(const std::string &ssid, const std::string &password) {
  assert(this->wifi_ == nullptr && "WiFi already setup!");
  WiFiComponent *wifi = new WiFiComponent(ssid, password, sanitize_hostname(this->name_));
  this->wifi_ = wifi;
  return this->register_component(wifi);
}

void Application::set_name(const std::string &name) {
  assert(this->name_.empty() && "Name was already set!");
  this->name_ = to_lowercase_underscore(name);
  global_preferences.begin(name);
}

MQTTClientComponent *Application::init_mqtt(const std::string &address, uint16_t port,
                                            const std::string &username, const std::string &password) {
  assert(this->mqtt_client_ == nullptr && "Did you initialize MQTT already?");
  MQTTClientComponent *component = new MQTTClientComponent(MQTTCredentials{
      .address = address,
      .port = port,
      .username = username,
      .password = password,
  });
  this->mqtt_client_ = component;

  return this->register_component(component);
}

MQTTClientComponent *Application::init_mqtt(const std::string &address,
                                            const std::string &username, const std::string &password) {
  return this->init_mqtt(address, 1883, username, password);
}

LogComponent *Application::init_log(uint32_t baud_rate,
                                    const Optional<std::string> &mqtt_topic,
                                    size_t tx_buffer_size) {
  auto *log = new LogComponent(baud_rate, tx_buffer_size);
  if (mqtt_topic) {
    if (!mqtt_topic->empty())
      log->set_custom_logging_topic(mqtt_topic.value);
  } else {
    log->set_mqtt_logging_enabled(false);
  }
  log->pre_setup();
  return this->register_component(log);
}

#ifdef USE_OUTPUT
PowerSupplyComponent *Application::make_power_supply(GPIOOutputPin pin, uint32_t enable_time, uint32_t keep_on_time) {
  auto *atx = new PowerSupplyComponent(pin, enable_time, keep_on_time);
  return this->register_component(atx);
}
#endif

#ifdef USE_BINARY_SENSOR
MQTTBinarySensorComponent *Application::make_mqtt_binary_sensor_for(binary_sensor::BinarySensor *binary_sensor,
                                                                    const std::string &friendly_name,
                                                                    Optional<std::string> device_class) {
  auto *mqtt = new MQTTBinarySensorComponent(friendly_name, binary_sensor);
  if (device_class.defined)
    mqtt->set_device_class(device_class.value);
  return this->register_mqtt_component(mqtt);
}
#endif

#ifdef USE_GPIO_BINARY_SENSOR
Application::MakeGPIOBinarySensor Application::make_gpio_binary_sensor(GPIOInputPin pin,
                                                                       const std::string &friendly_name,
                                                                       const std::string &device_class) {
  MakeGPIOBinarySensor s{};
  s.gpio = this->register_component(new GPIOBinarySensorComponent(pin));
  s.mqtt = this->make_mqtt_binary_sensor_for(s.gpio, friendly_name, std::move(device_class));
  return s;
}
#endif

#ifdef USE_DHT_SENSOR
Application::MakeDHTComponent Application::make_dht_sensor(uint8_t pin,
                                                           const std::string &temperature_friendly_name,
                                                           const std::string &humidity_friendly_name,
                                                           uint32_t check_interval) {
  auto *dht = new DHTComponent(pin, check_interval);
  this->register_component(dht);

  return MakeDHTComponent{
      .dht = dht,
      .mqtt_temperature = this->make_mqtt_sensor_for(dht->get_temperature_sensor(), std::move(temperature_friendly_name)),
      .mqtt_humidity = this->make_mqtt_sensor_for(dht->get_humidity_sensor(), std::move(humidity_friendly_name)),
  };
}
#endif

#ifdef USE_SENSOR
sensor::MQTTSensorComponent *Application::make_mqtt_sensor_for(sensor::Sensor *sensor,
                                                               const std::string &friendly_name) {
  return this->register_mqtt_component(new MQTTSensorComponent(friendly_name, sensor));
}
#endif

#ifdef USE_LEDC_OUTPUT
LEDCOutputComponent *Application::make_ledc_output(uint8_t pin, float frequency, uint8_t bit_depth) {
  auto *ledc = new LEDCOutputComponent(pin, frequency, bit_depth);
  return this->register_component(ledc);
}
#endif

#ifdef USE_PCA9685_OUTPUT
PCA9685OutputComponent *Application::make_pca9685_component(float frequency) {
  this->assert_i2c_initialized();
  auto *pca9685 = new PCA9685OutputComponent(frequency);
  return this->register_component(pca9685);
}
#endif

#ifdef USE_LIGHT
Application::LightStruct Application::make_rgb_light(const std::string &friendly_name,
                                                     FloatOutput *red, FloatOutput *green, FloatOutput *blue) {
  auto *out = new LinearLightOutputComponent();
  out->setup_rgb(red, green, blue);
  this->register_component(out);

  return this->connect_light_(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
Application::LightStruct Application::make_rgbw_light(const std::string &friendly_name,
                                                      output::FloatOutput *red,
                                                      output::FloatOutput *green,
                                                      output::FloatOutput *blue,
                                                      output::FloatOutput *white) {
  auto *out = new LinearLightOutputComponent();
  out->setup_rgbw(red, green, blue, white);
  this->register_component(out);

  return this->connect_light_(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
MQTTJSONLightComponent *Application::make_mqtt_light_(LightState *state, const std::string &friendly_name) {
  auto *mqtt = new MQTTJSONLightComponent(friendly_name);
  mqtt->set_state(state);
  return this->register_mqtt_component(mqtt);
}
#endif

WiFiComponent *Application::get_wifi() const {
  return this->wifi_;
}

#ifdef USE_OTA
OTAComponent *Application::init_ota() {
  auto *ota = new OTAComponent();
  ota->set_hostname(this->wifi_->get_hostname());
  return this->register_component(ota);
}
#endif

#ifdef USE_LIGHT
Application::LightStruct Application::make_monochromatic_light(const std::string &friendly_name,
                                                               output::FloatOutput *mono) {
  auto *out = new LinearLightOutputComponent();
  out->setup_monochromatic(mono);
  this->register_component(out);

  return this->connect_light_(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
Application::LightStruct Application::make_binary_light(const std::string &friendly_name,
                                                        output::BinaryOutput *binary) {
  auto *out = new LinearLightOutputComponent();
  out->setup_binary(binary);
  this->register_component(out);

  return this->connect_light_(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
Application::LightStruct Application::connect_light_(const std::string &friendly_name,
                                                     light::LinearLightOutputComponent *out) {
  LightStruct s{};
  s.state = new LightState(out->get_traits());
  out->set_state(s.state);
  s.output = out;
  s.mqtt = this->make_mqtt_light_(s.state, friendly_name);

  return s;
}
#endif

MQTTClientComponent *Application::get_mqtt_client() const {
  return this->mqtt_client_;
}

#ifdef USE_IR_TRANSMITTER
IRTransmitterComponent *Application::make_ir_transmitter(GPIOOutputPin pin,
                                                         uint8_t carrier_duty_percent,
                                                         uint8_t clock_divider) {
  return this->register_component(new IRTransmitterComponent(pin, carrier_duty_percent, clock_divider));
}
#endif

#ifdef USE_SWITCH
MQTTSwitchComponent *Application::make_mqtt_switch_for(switch_::Switch *switch_,
                                                       const std::string &friendly_name) {
  return this->register_mqtt_component(new MQTTSwitchComponent(friendly_name, switch_));
}
#endif

#ifdef USE_DALLAS_SENSOR
DallasComponent *Application::make_dallas_component(ESPOneWire *one_wire, uint32_t update_interval) {
  return this->register_component(new DallasComponent(one_wire, update_interval));
}
DallasComponent *Application::make_dallas_component(uint8_t pin, uint32_t update_interval) {
  return this->make_dallas_component(new ESPOneWire(pin), update_interval);
}
#endif

#ifdef USE_GPIO_SWITCH
Application::GPIOSwitchStruct Application::make_gpio_switch(GPIOOutputPin pin,
                                                            const std::string &friendly_name) {
  auto *binary_output = this->make_gpio_output(pin);
  auto *simple_switch = new SimpleSwitch(binary_output);
  auto *mqtt = this->make_mqtt_switch_for(simple_switch, friendly_name);

  return {
      .gpio = binary_output,
      .mqtt = mqtt
  };
}
#endif

const std::string &Application::get_name() const {
  return this->name_;
}

#ifdef USE_FAN
Application::FanStruct Application::make_fan(const std::string &friendly_name) {
  FanStruct s{};
  s.state = new FanState();
  s.mqtt = this->register_mqtt_component(new MQTTFanComponent(friendly_name));
  s.output = this->register_component(new BasicFanComponent());
  s.mqtt->set_state(s.state);
  s.output->set_state(s.state);
  return s;
}
#endif

#ifdef USE_GPIO_OUTPUT
output::GPIOBinaryOutputComponent *Application::make_gpio_output(GPIOOutputPin pin) {
  return this->register_component(new GPIOBinaryOutputComponent(pin));
}
#endif

#ifdef USE_PULSE_COUNTER_SENSOR
Application::MakePulseCounter Application::make_pulse_counter_sensor(uint8_t pin,
                                                                     const std::string &friendly_name,
                                                                     uint32_t update_interval) {
  auto *pcnt = this->register_component(new PulseCounterSensorComponent(pin, update_interval));
  auto *mqtt = this->make_mqtt_sensor_for(pcnt, friendly_name);
  return MakePulseCounter {
      .pcnt = pcnt,
      .mqtt = mqtt
  };
}
#endif

#ifdef USE_ADC_SENSOR
Application::MakeADCSensor Application::make_adc_sensor(uint8_t pin,
                                                        const std::string &friendly_name,
                                                        uint32_t update_interval) {
  auto *adc = this->register_component(new ADCSensorComponent(pin, update_interval));
  auto *mqtt = this->make_mqtt_sensor_for(adc, friendly_name);
  return MakeADCSensor {
      .adc = adc,
      .mqtt = mqtt
  };
}
#endif

#ifdef USE_ULTRASONIC_SENSOR
Application::MakeUltrasonicSensor Application::make_ultrasonic_sensor(GPIOOutputPin trigger_pin,
                                                                      GPIOInputPin echo_pin,
                                                                      const std::string &friendly_name,
                                                                      uint32_t update_interval) {
  auto *ultrasonic = this->register_component(
      new UltrasonicSensorComponent(trigger_pin, echo_pin, update_interval)
  );

  return MakeUltrasonicSensor {
      .ultrasonic = ultrasonic,
      .mqtt = this->make_mqtt_sensor_for(ultrasonic, friendly_name),
  };
}
#endif

#ifdef USE_ADS1115_SENSOR
ADS1115Component *Application::make_ads1115_component(uint8_t address) {
  this->assert_i2c_initialized();
  return this->register_component(new ADS1115Component(address));
}
#endif

#ifdef USE_BMP085_SENSOR
Application::MakeBMP085Component Application::make_bmp085_sensor(const std::string &temperature_friendly_name,
                                                                 const std::string &pressure_friendly_name,
                                                                 uint32_t update_interval) {
  this->assert_i2c_initialized();
  auto *bmp = this->register_component(new BMP085Component(update_interval));

  return MakeBMP085Component{
      .bmp = bmp,
      .mqtt_temperature = this->make_mqtt_sensor_for(bmp->get_temperature_sensor(), temperature_friendly_name),
      .mqtt_pressure = this->make_mqtt_sensor_for(bmp->get_pressure_sensor(), pressure_friendly_name),
  };
}
#endif

#ifdef USE_HTU21D_SENSOR
Application::MakeHTU21DComponent Application::make_htu21d_sensor(const std::string &temperature_friendly_name,
                                                                 const std::string &humidity_friendly_name,
                                                                 uint32_t update_interval) {
  this->assert_i2c_initialized();
  auto *htu21d = this->register_component(new HTU21DComponent(update_interval));

  return MakeHTU21DComponent{
      .htu21d = htu21d,
      .mqtt_temperature = this->make_mqtt_sensor_for(htu21d->get_temperature_sensor(), temperature_friendly_name),
      .mqtt_humidity = this->make_mqtt_sensor_for(htu21d->get_humidity_sensor(), humidity_friendly_name),
  };
}
#endif

#ifdef USE_I2C
#ifdef ARDUINO_ARCH_ESP32
void Application::init_i2c(uint8_t sda_pin, uint8_t scl_pin, uint32_t frequency) {
#else
void Application::init_i2c(uint8_t sda_pin, uint8_t scl_pin) {
#endif
  if (this->i2c_initialized_) {
    ESP_LOGE(TAG, "You have already initialized i2c, you should only initialize it once.");
    delay(1000);
    ESP.restart();
  }
#ifdef ARDUINO_ARCH_ESP32
  Wire.begin(sda_pin, scl_pin, frequency);
#else
  Wire.begin(sda_pin, scl_pin);
#endif
  this->i2c_initialized_ = true;
}
#endif //USE_I2C

#ifdef USE_I2C
void Application::assert_i2c_initialized() const {
  if (this->i2c_initialized_)
    return;
  ESP_LOGE(TAG, "You need to call App.init_i2c() because a component requires i2c to work.");
  delay(1000);
  ESP.restart();
}
#endif

#ifdef USE_STATUS_BINARY_SENSOR
MQTTBinarySensorComponent *Application::make_status_binary_sensor(const std::string &friendly_name) {
  auto *binary_sensor = new StatusBinarySensor(); // not a component
  auto *mqtt = this->make_mqtt_binary_sensor_for(binary_sensor, friendly_name);
  mqtt->set_custom_state_topic(this->mqtt_client_->get_availability().topic);
  mqtt->set_payload_on(this->mqtt_client_->get_availability().payload_available);
  mqtt->set_payload_off(this->mqtt_client_->get_availability().payload_not_available);
  mqtt->disable_availability();
  return mqtt;
}
#endif

#ifdef USE_RESTART_SWITCH
switch_::MQTTSwitchComponent *Application::make_restart_switch(const std::string &friendly_name) {
  auto *switch_ = new RestartSwitch(); // not a component
  return this->make_mqtt_switch_for(switch_, friendly_name);
}
#endif

#ifdef USE_ESP8266_PWM_OUTPUT
ESP8266PWMOutput *Application::make_esp8266_pwm_output(GPIOOutputPin pin) {
  return this->register_component(new ESP8266PWMOutput(pin));
}
#endif

#ifdef USE_HDC1080_SENSOR
Application::MakeHDC1080Component Application::make_hdc1080_sensor(const std::string &temperature_friendly_name,
                                                                   const std::string &humidity_friendly_name,
                                                                   uint32_t update_interval) {
  this->assert_i2c_initialized();
  auto *hdc1080 = this->register_component(new HDC1080Component(update_interval));

  return MakeHDC1080Component{
      .hdc1080 = hdc1080,
      .mqtt_temperature = this->make_mqtt_sensor_for(hdc1080->get_temperature_sensor(), temperature_friendly_name),
      .mqtt_humidity = this->make_mqtt_sensor_for(hdc1080->get_humidity_sensor(), humidity_friendly_name),
  };
}
#endif

Application App; // NOLINT

} // namespace esphomelib
