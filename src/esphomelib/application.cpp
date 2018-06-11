//
// Created by Otto Winter on 25.11.17.
//

#include "esphomelib/application.h"

#include <utility>
#include <algorithm>

#include "esphomelib/log.h"
#include "esphomelib/espmath.h"
#include "esphomelib/esppreferences.h"

ESPHOMELIB_NAMESPACE_BEGIN

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
#ifdef USE_IO
using namespace esphomelib::io;
#endif
#ifdef USE_COVER
using namespace esphomelib::cover;
#endif
#ifdef USE_REMOTE
using namespace esphomelib::remote;
#endif

static const char *TAG = "application";

void Application::setup() {
  ESP_LOGI(TAG, "Running through setup()...");
  assert(this->application_state_ == Component::CONSTRUCTION && "setup() called twice.");
  ESP_LOGV(TAG, "Sorting components by setup priority...");
  std::stable_sort(this->components_.begin(), this->components_.end(), [](const Component *a, const Component *b) {
    return a->get_setup_priority() >= b->get_setup_priority();
  });

  for (uint32_t i = 0; i < this->components_.size(); i++) {
    Component *component = this->components_[i];
    if (component->is_failed())
      continue;

    component->setup_();
    if (component->can_proceed())
      continue;

    std::stable_sort(this->components_.begin(), this->components_.begin() + i + 1, [](Component *a, Component *b) {
      return a->get_loop_priority() >= b->get_loop_priority();
    });

    do {
      for (uint32_t j = 0; j <= i; j++) {
        if (!this->components_[j]->is_failed())
          this->components_[j]->loop_();
      }
      yield();
    } while (!component->can_proceed());
  }

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
    if (!component->is_failed())
      component->loop_();
  }
  yield();

  if (first_loop)
    ESP_LOGI(TAG, "First loop finished successfully!");
}

WiFiComponent *Application::init_wifi(const std::string &ssid, const std::string &password) {
  WiFiComponent *wifi = this->init_wifi();
  wifi->set_sta(WiFiAp{
      .ssid = ssid,
      .password = password,
      .channel = -1,
      .manual_ip = {},
  });
  return wifi;
}

void Application::set_name(const std::string &name) {
  assert(this->name_.empty() && "Name was already set!");
  this->name_ = to_lowercase_underscore(name);
  global_preferences.begin(name);
}

MQTTClientComponent *Application::init_mqtt(const std::string &address, uint16_t port,
                                            const std::string &username, const std::string &password) {
  assert(this->mqtt_client_ == nullptr);
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
                                    size_t tx_buffer_size) {
  assert(global_log_component == nullptr && "Log already set up!");
  auto *log = new LogComponent(baud_rate, tx_buffer_size);
  log->pre_setup();
  return this->register_component(log);
}

#ifdef USE_OUTPUT
PowerSupplyComponent *Application::make_power_supply(const GPIOOutputPin &pin, uint32_t enable_time, uint32_t keep_on_time) {
  auto *atx = new PowerSupplyComponent(pin.copy(), enable_time, keep_on_time);
  return this->register_component(atx);
}
#endif

#ifdef USE_BINARY_SENSOR
MQTTBinarySensorComponent *Application::register_binary_sensor(binary_sensor::BinarySensor *binary_sensor) {
  for (auto *controller : this->controllers_)
    controller->register_binary_sensor(binary_sensor);
  MQTTBinarySensorComponent *ret = nullptr;
  if (this->mqtt_client_ != nullptr)
    ret = this->register_component(new MQTTBinarySensorComponent(binary_sensor));
  return ret;
}
#endif

#ifdef USE_GPIO_BINARY_SENSOR
Application::MakeGPIOBinarySensor Application::make_gpio_binary_sensor(const std::string &friendly_name,
                                                                       const GPIOInputPin &pin,
                                                                       const std::string &device_class) {
  MakeGPIOBinarySensor s{};
  s.gpio = this->register_component(new GPIOBinarySensorComponent(friendly_name, pin.copy()));
  s.gpio->set_device_class(device_class);
  s.mqtt = this->register_binary_sensor(s.gpio);
  return s;
}
#endif

#ifdef USE_DHT_SENSOR
Application::MakeDHTSensor Application::make_dht_sensor(const std::string &temperature_friendly_name,
                                                        const std::string &humidity_friendly_name,
                                                        const GPIOOutputPin &pin,
                                                        uint32_t check_interval) {
  auto *dht = new DHTComponent(temperature_friendly_name, humidity_friendly_name, pin.copy(), check_interval);
  this->register_component(dht);

  return MakeDHTSensor{
      .dht = dht,
      .mqtt_temperature = this->register_sensor(dht->get_temperature_sensor()),
      .mqtt_humidity = this->register_sensor(dht->get_humidity_sensor()),
  };
}
#endif

#ifdef USE_SENSOR
sensor::MQTTSensorComponent *Application::register_sensor(sensor::Sensor *sensor) {
  for (auto *controller : this->controllers_)
    controller->register_sensor(sensor);
  MQTTSensorComponent *ret = nullptr;
  if (this->mqtt_client_ != nullptr)
    ret = this->register_component(new MQTTSensorComponent(sensor));
  return ret;
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
  auto *pca9685 = new PCA9685OutputComponent(this->i2c_, frequency);
  return this->register_component(pca9685);
}
#endif

#ifdef USE_LIGHT
Application::MakeLight Application::make_rgb_light(const std::string &friendly_name,
                                                   FloatOutput *red, FloatOutput *green, FloatOutput *blue) {
  auto *out = new RGBLightOutput(red, green, blue);
  return this->make_light_for_light_output(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
Application::MakeLight Application::make_rgbw_light(const std::string &friendly_name,
                                                    output::FloatOutput *red,
                                                    output::FloatOutput *green,
                                                    output::FloatOutput *blue,
                                                    output::FloatOutput *white) {
  auto *out = new RGBWLightOutput(red, green, blue, white);
  return this->make_light_for_light_output(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
MQTTJSONLightComponent *Application::register_light(LightState *state) {
  for (auto *controller : this->controllers_)
    controller->register_light(state);
  MQTTJSONLightComponent *ret = nullptr;
  if (this->mqtt_client_ != nullptr)
    ret = this->register_component(new MQTTJSONLightComponent(state));
  return ret;
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
Application::MakeLight Application::make_monochromatic_light(const std::string &friendly_name,
                                                             output::FloatOutput *mono) {
  auto *out = new MonochromaticLightOutput(mono);
  return this->make_light_for_light_output(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
Application::MakeLight Application::make_binary_light(const std::string &friendly_name,
                                                      output::BinaryOutput *binary) {
  auto *out = new BinaryLightOutput(binary);
  return this->make_light_for_light_output(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
Application::MakeLight Application::make_light_for_light_output(const std::string &name, light::LightOutput *output) {
  LightState *state = this->register_component(new LightState(name, output));

  return MakeLight{
      .output = output,
      .state = state,
      .mqtt = this->register_light(state),
  };
}
#endif

MQTTClientComponent *Application::get_mqtt_client() const {
  return this->mqtt_client_;
}

#ifdef USE_IR_TRANSMITTER
IRTransmitterComponent *Application::make_ir_transmitter(const GPIOOutputPin &pin,
                                                         uint8_t carrier_duty_percent) {
  return this->register_component(new IRTransmitterComponent(pin.copy(), carrier_duty_percent));
}
#endif

#ifdef USE_SWITCH
MQTTSwitchComponent *Application::register_switch(switch_::Switch *switch_) {
  for (auto *controller : this->controllers_)
    controller->register_switch(switch_);
  MQTTSwitchComponent *ret = nullptr;
  if (this->mqtt_client_ != nullptr)
    ret = this->register_component(new MQTTSwitchComponent(switch_));
  return ret;
}
#endif

#ifdef USE_DALLAS_SENSOR
DallasComponent *Application::make_dallas_component(ESPOneWire *one_wire, uint32_t update_interval) {
  return this->register_component(new DallasComponent(one_wire, update_interval));
}
DallasComponent *Application::make_dallas_component(const GPIOOutputPin &pin, uint32_t update_interval) {
  return this->make_dallas_component(new ESPOneWire(pin.copy()), update_interval);
}
#endif

#ifdef USE_GPIO_SWITCH
Application::MakeGPIOSwitch Application::make_gpio_switch(const std::string &friendly_name,
                                                          const GPIOOutputPin &pin) {
  auto *binary_output = this->make_gpio_output(pin);
  auto simple_switch = this->make_simple_switch(friendly_name, binary_output);

  return {
      .gpio = binary_output,
      .switch_ = simple_switch.switch_,
      .mqtt = simple_switch.mqtt,
  };
}
#endif

const std::string &Application::get_name() const {
  return this->name_;
}

#ifdef USE_FAN
Application::MakeFan Application::make_fan(const std::string &friendly_name) {
  MakeFan s{};
  s.state = new FanState(friendly_name);
  s.mqtt = this->register_fan(s.state);
  s.output = this->register_component(new BasicFanComponent());
  s.output->set_state(s.state);
  return s;
}
#endif

#ifdef USE_GPIO_OUTPUT
output::GPIOBinaryOutputComponent *Application::make_gpio_output(const GPIOOutputPin &pin) {
  return this->register_component(new GPIOBinaryOutputComponent(pin.copy()));
}
#endif

#ifdef USE_PULSE_COUNTER_SENSOR
Application::MakePulseCounterSensor Application::make_pulse_counter_sensor(const std::string &friendly_name,
                                                                           const GPIOInputPin &pin,
                                                                           uint32_t update_interval) {
  auto *pcnt = this->register_component(new PulseCounterSensorComponent(friendly_name, pin.copy(), update_interval));
  auto *mqtt = this->register_sensor(pcnt);
  return MakePulseCounterSensor{
      .pcnt = pcnt,
      .mqtt = mqtt
  };
}
#endif

#ifdef USE_ADC_SENSOR
Application::MakeADCSensor Application::make_adc_sensor(const std::string &friendly_name,
                                                        uint8_t pin,
                                                        uint32_t update_interval) {
  auto *adc = this->register_component(new ADCSensorComponent(friendly_name, pin, update_interval));
  auto *mqtt = this->register_sensor(adc);
  return MakeADCSensor{
      .adc = adc,
      .mqtt = mqtt
  };
}
#endif

#ifdef USE_ULTRASONIC_SENSOR
Application::MakeUltrasonicSensor Application::make_ultrasonic_sensor(const std::string &friendly_name,
                                                                      const GPIOOutputPin &trigger_pin,
                                                                      const GPIOInputPin &echo_pin,
                                                                      uint32_t update_interval) {
  auto *ultrasonic = this->register_component(
      new UltrasonicSensorComponent(friendly_name, trigger_pin.copy(), echo_pin.copy(), update_interval)
  );

  return MakeUltrasonicSensor{
      .ultrasonic = ultrasonic,
      .mqtt = this->register_sensor(ultrasonic),
  };
}
#endif

#ifdef USE_WIFI_SIGNAL_SENSOR
Application::MakeWiFiSignalSensor Application::make_wifi_signal_sensor(const std::string &name,
                                                                       uint32_t update_interval) {
  auto *wifi = this->register_component(new WiFiSignalSensor(name, update_interval));

  return MakeWiFiSignalSensor{
      .wifi = wifi,
      .mqtt = this->register_sensor(wifi),
  };
}
#endif

#ifdef USE_ADS1115_SENSOR
ADS1115Component *Application::make_ads1115_component(uint8_t address) {
  return this->register_component(new ADS1115Component(this->i2c_, address));
}
#endif

#ifdef USE_BMP085_SENSOR
Application::MakeBMP085Sensor Application::make_bmp085_sensor(const std::string &temperature_friendly_name,
                                                              const std::string &pressure_friendly_name,
                                                              uint32_t update_interval) {
  auto *bmp = this->register_component(
      new BMP085Component(this->i2c_,
                          temperature_friendly_name, pressure_friendly_name,
                          update_interval)
  );

  return MakeBMP085Sensor{
      .bmp = bmp,
      .mqtt_temperature = this->register_sensor(bmp->get_temperature_sensor()),
      .mqtt_pressure = this->register_sensor(bmp->get_pressure_sensor()),
  };
}
#endif

#ifdef USE_HTU21D_SENSOR
Application::MakeHTU21DSensor Application::make_htu21d_sensor(const std::string &temperature_friendly_name,
                                                              const std::string &humidity_friendly_name,
                                                              uint32_t update_interval) {
  auto *htu21d = this->register_component(
      new HTU21DComponent(this->i2c_, temperature_friendly_name, humidity_friendly_name,
                          update_interval)
  );

  return MakeHTU21DSensor{
      .htu21d = htu21d,
      .mqtt_temperature = this->register_sensor(htu21d->get_temperature_sensor()),
      .mqtt_humidity = this->register_sensor(htu21d->get_humidity_sensor()),
  };
}
#endif

#ifdef USE_I2C
I2CComponent *Application::init_i2c(uint8_t sda_pin, uint8_t scl_pin, bool scan) {
  auto *i2c = this->register_component(new I2CComponent(sda_pin, scl_pin, scan));
  if (this->i2c_ == nullptr)
    this->i2c_ = i2c;
  return i2c;
}
#endif

#ifdef USE_STATUS_BINARY_SENSOR
Application::MakeStatusBinarySensor Application::make_status_binary_sensor(const std::string &friendly_name) {
  assert(this->mqtt_client_ != nullptr);
  auto *binary_sensor = new StatusBinarySensor(friendly_name); // not a component
  auto *mqtt = this->register_binary_sensor(binary_sensor);
  mqtt->set_custom_state_topic(this->mqtt_client_->get_availability().topic);
  mqtt->set_payload_on(this->mqtt_client_->get_availability().payload_available);
  mqtt->set_payload_off(this->mqtt_client_->get_availability().payload_not_available);
  mqtt->disable_availability();
  return MakeStatusBinarySensor{
      .status = binary_sensor,
      .mqtt = mqtt,
  };
}
#endif

#ifdef USE_RESTART_SWITCH
Application::MakeRestartSwitch Application::make_restart_switch(const std::string &friendly_name) {
  auto *switch_ = this->register_component(new RestartSwitch(friendly_name)); // not a component
  return MakeRestartSwitch{
      .restart = switch_,
      .mqtt = this->register_switch(switch_),
  };
}
#endif

#ifdef USE_SHUTDOWN_SWITCH
Application::MakeShutdownSwitch Application::make_shutdown_switch(const std::string &friendly_name) {
  auto *switch_ = this->register_component(new ShutdownSwitch(friendly_name));
  return MakeShutdownSwitch{
      .shutdown = switch_,
      .mqtt = this->register_switch(switch_),
  };
}
#endif

#ifdef USE_ESP8266_PWM_OUTPUT
ESP8266PWMOutput *Application::make_esp8266_pwm_output(GPIOOutputPin pin) {
  return this->register_component(new ESP8266PWMOutput(pin));
}
#endif

#ifdef USE_HDC1080_SENSOR
Application::MakeHDC1080Sensor Application::make_hdc1080_sensor(const std::string &temperature_friendly_name,
                                                                const std::string &humidity_friendly_name,
                                                                uint32_t update_interval) {
  auto *hdc1080 = this->register_component(
      new HDC1080Component(this->i2c_,
                           temperature_friendly_name, humidity_friendly_name,
                           update_interval)
  );

  return MakeHDC1080Sensor{
      .hdc1080 = hdc1080,
      .mqtt_temperature = this->register_sensor(hdc1080->get_temperature_sensor()),
      .mqtt_humidity = this->register_sensor(hdc1080->get_humidity_sensor()),
  };
}
#endif

#ifdef USE_DEBUG_COMPONENT
DebugComponent *Application::make_debug_component() {
  return this->register_component(new DebugComponent());
}
#endif

#ifdef USE_FAN
fan::MQTTFanComponent *Application::register_fan(fan::FanState *state) {
  for (auto *controller : this->controllers_)
    controller->register_fan(state);
  MQTTFanComponent *ret = nullptr;
  if (this->mqtt_client_ != nullptr)
    ret = this->register_component(new MQTTFanComponent(state));
  return ret;
}
#endif

#ifdef USE_WEB_SERVER
WebServer *Application::init_web_server(uint16_t port) {
  auto *web_server = new WebServer(port);
  this->register_component(web_server);
  return this->register_controller(web_server);
}
#endif

#ifdef USE_DEEP_SLEEP
DeepSleepComponent *Application::make_deep_sleep_component() {
  return this->register_component(new DeepSleepComponent());
}
#endif

WiFiComponent *Application::init_wifi() {
  assert(this->wifi_ == nullptr && "WiFi already setup!");
  auto *wifi = new WiFiComponent();
  wifi->set_hostname(sanitize_hostname(this->name_));
  this->wifi_ = wifi;
  return this->register_component(wifi);
}

#ifdef USE_PCF8574
PCF8574Component *Application::make_pcf8574_component(uint8_t address, bool pcf8575) {
  return this->register_component(new PCF8574Component(this->i2c_, address, pcf8575));
}
#endif

#ifdef USE_MPU6050
sensor::MPU6050Component *Application::make_mpu6050_sensor(uint8_t address, uint32_t update_interval) {
  return this->register_component(new MPU6050Component(this->i2c_, address, update_interval));
}
#endif

#ifdef USE_SIMPLE_SWITCH
Application::MakeSimpleSwitch Application::make_simple_switch(const std::string &friendly_name, BinaryOutput *output) {
  auto *s = this->register_component(new SimpleSwitch(friendly_name, output));
  return {
      .switch_ = s,
      .mqtt = this->register_switch(s)
  };
}
#endif

#ifdef USE_TSL2561
Application::MakeTSL2561Sensor Application::make_tsl2561_sensor(const std::string &name,
                                                                uint8_t address,
                                                                uint32_t update_interval) {
  auto *tsl = this->register_component(new TSL2561Sensor(this->i2c_, name, address, update_interval));
  return {
      .tsl2561 = tsl,
      .mqtt = this->register_sensor(tsl)
  };
}
#endif

#ifdef USE_BH1750
Application::MakeBH1750Sensor Application::make_bh1750_sensor(const std::string &name,
                                                              uint8_t address,
                                                              uint32_t update_interval) {
  auto *bh1750 = this->register_component(new BH1750Sensor(this->i2c_, name, address, update_interval));
  return {
      .bh1750 = bh1750,
      .mqtt = this->register_sensor(bh1750)
  };
}
#endif

#ifdef USE_BME280
Application::MakeBME280Sensor Application::make_bme280_sensor(const std::string &temperature_name,
                                                              const std::string &pressure_name,
                                                              const std::string &humidity_name,
                                                              uint8_t address,
                                                              uint32_t update_interval) {
  auto *bme280 = this->register_component(
      new BME280Component(this->i2c_,
                          temperature_name, pressure_name, humidity_name,
                          address, update_interval)
  );

  return {
      .bme280 = bme280,
      .mqtt_temperature = this->register_sensor(bme280->get_temperature_sensor()),
      .mqtt_pressure = this->register_sensor(bme280->get_pressure_sensor()),
      .mqtt_humidity = this->register_sensor(bme280->get_humidity_sensor()),
  };
}
#endif

#ifdef USE_BME680
Application::MakeBME680Sensor Application::make_bme680_sensor(const std::string &temperature_name,
                                                              const std::string &pressure_name,
                                                              const std::string &humidity_name,
                                                              const std::string &gas_resistance_name,
                                                              uint8_t address,
                                                              uint32_t update_interval) {
  auto *bme680 = this->register_component(
      new BME680Component(this->i2c_,
                          temperature_name, pressure_name, humidity_name, gas_resistance_name,
                          address, update_interval)
  );

  return {
      .bme680 = bme680,
      .mqtt_temperature = this->register_sensor(bme680->get_temperature_sensor()),
      .mqtt_pressure = this->register_sensor(bme680->get_pressure_sensor()),
      .mqtt_humidity = this->register_sensor(bme680->get_humidity_sensor()),
      .mqtt_gas_resistance = this->register_sensor(bme680->get_gas_resistance_sensor())
  };
}
#endif

#ifdef USE_SHT3XD
Application::MakeSHT3XDSensor Application::make_sht3xd_sensor(const std::string &temperature_name,
                                                              const std::string &humidity_name,
                                                              uint8_t address,
                                                              uint32_t update_interval) {
  auto *sht3xd = this->register_component(
      new SHT3XDComponent(this->i2c_,
                          temperature_name, humidity_name,
                          address, update_interval)
  );

  return {
      .sht3xd = sht3xd,
      .mqtt_temperature = this->register_sensor(sht3xd->get_temperature_sensor()),
      .mqtt_humidity = this->register_sensor(sht3xd->get_humidity_sensor()),
  };
}
#endif

#ifdef USE_ESP32_TOUCH_BINARY_SENSOR
ESP32TouchComponent *Application::make_esp32_touch_component() {
  return this->register_component(new ESP32TouchComponent());
}
#endif

#ifdef USE_ESP32_BLE_TRACKER
ESP32BLETracker *Application::make_esp32_ble_tracker() {
  return this->register_component(new ESP32BLETracker());
}
#endif

#ifdef USE_FAST_LED_LIGHT
Application::MakeFastLEDLight Application::make_fast_led_light(const std::string &name) {
  auto *fast_led = this->register_component(new FastLEDLightOutputComponent());
  auto make = this->make_light_for_light_output(name, fast_led);

  return MakeFastLEDLight{
      .fast_led = fast_led,
      .state = make.state,
      .mqtt = make.mqtt,
  };
}
#endif

#ifdef USE_DHT12_SENSOR
Application::MakeDHT12Sensor Application::make_dht12_sensor(const std::string &temperature_name,
                                                            const std::string &humidity_name,
                                                            uint32_t update_interval) {
  auto *dht12 = this->register_component(
      new DHT12Component(this->i2c_, temperature_name, humidity_name, update_interval)
  );

  return {
      .dht12 = dht12,
      .mqtt_temperature = this->register_sensor(dht12->get_temperature_sensor()),
      .mqtt_humidity = this->register_sensor(dht12->get_humidity_sensor()),
  };
}
#endif

#ifdef USE_ROTARY_ENCODER_SENSOR
Application::MakeRotaryEncoderSensor Application::make_rotary_encoder_sensor(const std::string &name,
                                                                             const GPIOInputPin &pin_a,
                                                                             const GPIOInputPin &pin_b) {
  auto *encoder = this->register_component(new RotaryEncoderSensor(name, pin_a.copy(), pin_b.copy()));

  return MakeRotaryEncoderSensor{
      .rotary_encoder = encoder,
      .mqtt = this->register_sensor(encoder),
  };
}
#endif


mqtt::MQTTMessageTrigger *Application::make_mqtt_message_trigger(const std::string &topic, uint8_t qos) {
  return global_mqtt_client->make_message_trigger(topic, qos);
}

StartupTrigger *Application::make_startup_trigger() {
  return this->register_component(new StartupTrigger());
}

ShutdownTrigger *Application::make_shutdown_trigger() {
  return new ShutdownTrigger();
}

#ifdef USE_TEMPLATE_SENSOR
Application::MakeTemplateSensor Application::make_template_sensor(const std::string &name,
                                                                  std::function<optional<float>()> &&f,
                                                                  uint32_t update_interval) {
  auto *template_ = this->register_component(new TemplateSensor(name, std::move(f), update_interval));

  return MakeTemplateSensor{
      .template_ = template_,
      .mqtt = this->register_sensor(template_),
  };
}
#endif

#ifdef USE_MAX6675_SENSOR
Application::MakeMAX6675Sensor Application::make_max6675_sensor(const std::string &name,
                                                                const GPIOOutputPin &cs,
                                                                const GPIOOutputPin &clock,
                                                                const GPIOInputPin &miso,
                                                                uint32_t update_interval) {
  auto *sensor = this->register_component(
      new MAX6675Sensor(name, cs.copy(), clock.copy(), miso.copy(), update_interval)
  );

  return MakeMAX6675Sensor {
      .max6675 = sensor,
      .mqtt = this->register_sensor(sensor),
  };
}
#endif

#ifdef USE_TEMPLATE_BINARY_SENSOR
Application::MakeTemplateBinarySensor Application::make_template_binary_sensor(const std::string &name,
                                                                               std::function<optional<bool>()> &&f) {
  auto *template_ = this->register_component(new TemplateBinarySensor(name, std::move(f)));

  return MakeTemplateBinarySensor{
      .template_ = template_,
      .mqtt = this->register_binary_sensor(template_),
  };
}
#endif

#ifdef USE_TEMPLATE_SWITCH
Application::MakeTemplateSwitch Application::make_template_switch(const std::string &name) {
  auto *template_ = this->register_component(new TemplateSwitch(name));

  return MakeTemplateSwitch{
      .template_ = template_,
      .mqtt = this->register_switch(template_),
  };
}
#endif

#ifdef USE_COVER
cover::MQTTCoverComponent *Application::register_cover(cover::Cover *cover) {
  for (auto *controller : this->controllers_)
    controller->register_cover(cover);
  MQTTCoverComponent *ret = nullptr;
  if (this->mqtt_client_ != nullptr)
    ret = this->register_component(new MQTTCoverComponent(cover));
  return ret;
}
#endif

#ifdef USE_TEMPLATE_COVER
Application::MakeTemplateCover Application::make_template_cover(const std::string &name) {
  auto *template_ = this->register_component(new TemplateCover(name));

  return MakeTemplateCover{
      .template_ = template_,
      .mqtt = this->register_cover(template_),
  };
}
#endif

#ifdef USE_REMOTE_TRANSMITTER
remote::RemoteTransmitterComponent *Application::make_remote_transmitter_component(const GPIOOutputPin &output) {
  return App.register_component(new remote::RemoteTransmitterComponent(output.copy()));
}
#endif

#ifdef USE_REMOTE_RECEIVER
remote::RemoteReceiverComponent *Application::make_remote_receiver_component(const GPIOInputPin &output) {
  return App.register_component(new remote::RemoteReceiverComponent(output.copy()));
}
#endif

#ifdef USE_DUTY_CYCLE_SENSOR
Application::MakeDutyCycleSensor Application::make_duty_cycle_sensor(const std::string &name,
                                                                     const GPIOInputPin &pin,
                                                                     uint32_t update_interval) {
  auto *duty = App.register_component(new DutyCycleSensor(name, pin.copy(), update_interval));

  return MakeDutyCycleSensor{
      .duty = duty,
      .mqtt = App.register_sensor(duty),
  };
}
#endif

#ifdef USE_ESP32_HALL_SENSOR
Application::MakeESP32HallSensor Application::make_esp32_hall_sensor(const std::string &name,
                                                                     uint32_t update_interval) {
  auto *hall = this->register_component(new ESP32HallSensor(name, update_interval));

  return MakeESP32HallSensor{
      .hall = hall,
      .mqtt = this->register_sensor(hall),
  };
}
#endif

#ifdef USE_ESP32_BLE_BEACON
ESP32BLEBeacon *Application::make_esp32_ble_beacon() {
  return App.register_component(new ESP32BLEBeacon());
}
#endif

Application App; // NOLINT

ESPHOMELIB_NAMESPACE_END
