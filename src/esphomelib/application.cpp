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
#ifdef USE_TIME
using namespace esphomelib::time;
#endif
#ifdef USE_TEXT_SENSOR
using namespace esphomelib::text_sensor;
#endif
#ifdef USE_STEPPER
using namespace esphomelib::stepper;
#endif

static const char *TAG = "application";

void Application::setup() {
  ESP_LOGI(TAG, "Running through setup()...");
  assert(this->application_state_ == COMPONENT_STATE_CONSTRUCTION && "setup() called twice.");
  ESP_LOGV(TAG, "Sorting components by setup priority...");
  std::stable_sort(this->components_.begin(), this->components_.end(), [](const Component *a, const Component *b) {
    return a->get_actual_setup_priority() > b->get_actual_setup_priority();
  });

  for (uint32_t i = 0; i < this->components_.size(); i++) {
    Component *component = this->components_[i];
    if (component->is_failed())
      continue;

    component->setup_();
    if (component->can_proceed())
      continue;

    std::stable_sort(this->components_.begin(), this->components_.begin() + i + 1, [](Component *a, Component *b) {
      return a->get_loop_priority() > b->get_loop_priority();
    });

    do {
      uint32_t new_global_state = STATUS_LED_WARNING;
      for (uint32_t j = 0; j <= i; j++) {
        if (!this->components_[j]->is_failed()) {
          this->components_[j]->loop_();
        }
        new_global_state |= this->components_[j]->get_component_state();
        global_state |= new_global_state;
      }
      global_state = new_global_state;
      yield();
    } while (!component->can_proceed());
  }

  this->application_state_ = COMPONENT_STATE_SETUP;

  ESP_LOGI(TAG, "setup() finished successfully!");

  if (this->compilation_time_.empty()) {
    ESP_LOGI(TAG, "You're running esphomelib v" ESPHOMELIB_VERSION);
  } else {
    ESP_LOGI(TAG, "You're running esphomelib v" ESPHOMELIB_VERSION " compiled on %s", this->compilation_time_.c_str());
  }

  for (uint32_t i = 0; i < this->components_.size(); i++) {
    Component *component = this->components_[i];
    component->dump_config();
  }
}

void HOT Application::loop() {
  assert(this->application_state_ >= COMPONENT_STATE_SETUP && "Did you forget to call setup()?");

  bool first_loop = this->application_state_ == COMPONENT_STATE_SETUP;
  if (first_loop) {
    ESP_LOGI(TAG, "Running through first loop()");
    this->application_state_ = COMPONENT_STATE_LOOP;
  }

  uint32_t new_global_state = 0;
  for (Component *component : this->components_) {
    if (!component->is_failed()) {
      component->loop_();
    }
    new_global_state |= component->get_component_state();
    global_state |= new_global_state;
    feed_wdt();
  }
  global_state = new_global_state;

  const uint32_t now = millis();
  if (HighFrequencyLoopRequester::is_high_frequency()) {
    yield();
  } else {
    uint32_t delay_time = this->loop_interval_;
    if (now - this->last_loop_ < this->loop_interval_)
      delay_time = this->loop_interval_ - (now - this->last_loop_);
    delay(delay_time);
  }
  this->last_loop_  = now;

  if (first_loop) {
    ESP_LOGI(TAG, "First loop finished successfully!");
  }
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
  this->name_ = to_lowercase_underscore(name);
  global_preferences.begin(name);
}

void Application::set_compilation_datetime(const char *str) {
  this->compilation_time_ = str;
}
const std::string &Application::get_compilation_time() const {
  return this->compilation_time_;
}

MQTTClientComponent *Application::init_mqtt(const std::string &address, uint16_t port,
                                            const std::string &username, const std::string &password) {
  MQTTClientComponent *component = new MQTTClientComponent(MQTTCredentials{
      .address = address,
      .port = port,
      .username = username,
      .password = password,
  }, this->get_name());
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
PowerSupplyComponent *Application::make_power_supply(const GPIOOutputPin &pin,
                                                     uint32_t enable_time,
                                                     uint32_t keep_on_time) {
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

#ifdef USE_MY9231_OUTPUT
MY9231OutputComponent *Application::make_my9231_component(const GPIOOutputPin &pin_di,
                                                          const GPIOOutputPin &pin_dcki) {
  auto *my9231 = new MY9231OutputComponent(pin_di.copy(), pin_dcki.copy());
  return this->register_component(my9231);
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
Application::MakeLight Application::make_rgbww_light(const std::string &friendly_name,
                                                     float cold_white_mireds,
                                                     float warm_white_mireds,
                                                     output::FloatOutput *red,
                                                     output::FloatOutput *green,
                                                     output::FloatOutput *blue,
                                                     output::FloatOutput *cold_white,
                                                     output::FloatOutput *warm_white) {
  auto *out = new RGBWWLightOutput(cold_white_mireds, warm_white_mireds,
                                   red, green, blue, cold_white, warm_white);
  return this->make_light_for_light_output(friendly_name, out);
}
#endif

#ifdef USE_LIGHT
Application::MakeLight Application::make_cwww_light(const std::string &friendly_name,
                                                    float cold_white_mireds,
                                                    float warm_white_mireds,
                                                    output::FloatOutput *cold_white,
                                                    output::FloatOutput *warm_white) {
  auto *out = new CWWWLightOutput(cold_white_mireds, warm_white_mireds, cold_white, warm_white);
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
  return this->register_component(new OTAComponent());
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
IRTransmitterComponent *Application::make_ir_transmitter(const GPIOOutputPin &pin_,
                                                         uint8_t carrier_duty_percent) {
  return this->register_component(new IRTransmitterComponent(pin_.copy(), carrier_duty_percent));
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
  auto gpio_switch = this->register_component(new GPIOSwitch(friendly_name, pin.copy()));

  return {
      .switch_ = gpio_switch,
      .mqtt = this->register_switch(gpio_switch),
  };
}
#endif

const std::string &Application::get_name() const {
  return this->name_;
}

#ifdef USE_FAN
Application::MakeFan Application::make_fan(const std::string &friendly_name) {
  MakeFan s{};
  s.state = this->register_component(new FanState(friendly_name));
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
  auto *binary_sensor = this->register_component(new StatusBinarySensor(friendly_name));
  auto *mqtt = this->register_binary_sensor(binary_sensor);
  mqtt->set_custom_state_topic(this->mqtt_client_->get_availability().topic);
  mqtt->disable_availability();
  mqtt->set_is_status(true);
  return MakeStatusBinarySensor{
      .status = binary_sensor,
      .mqtt = mqtt,
  };
}
#endif

#ifdef USE_RESTART_SWITCH
Application::MakeRestartSwitch Application::make_restart_switch(const std::string &friendly_name) {
  auto *switch_ = new RestartSwitch(friendly_name); // not a component
  return MakeRestartSwitch{
      .restart = switch_,
      .mqtt = this->register_switch(switch_),
  };
}
#endif

#ifdef USE_SHUTDOWN_SWITCH
Application::MakeShutdownSwitch Application::make_shutdown_switch(const std::string &friendly_name) {
  auto *switch_ = new ShutdownSwitch(friendly_name);
  return MakeShutdownSwitch{
      .shutdown = switch_,
      .mqtt = this->register_switch(switch_),
  };
}
#endif

#ifdef USE_ESP8266_PWM_OUTPUT
ESP8266PWMOutput *Application::make_esp8266_pwm_output(GPIOOutputPin pin_) {
  return this->register_component(new ESP8266PWMOutput(pin_));
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

#ifdef USE_OUTPUT_SWITCH
Application::MakeOutputSwitch Application::make_output_switch(const std::string &friendly_name, BinaryOutput *output) {
  auto *s = this->register_component(new OutputSwitch(friendly_name, output));
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

#ifdef USE_BMP280
Application::MakeBMP280Sensor Application::make_bmp280_sensor(const std::string &temperature_name,
                                                              const std::string &pressure_name,
                                                              uint8_t address,
                                                              uint32_t update_interval) {
  auto *bmp280 = this->register_component(
      new BMP280Component(this->i2c_,
                          temperature_name, pressure_name,
                          address, update_interval)
  );

  return {
      .bmp280 = bmp280,
      .mqtt_temperature = this->register_sensor(bmp280->get_temperature_sensor()),
      .mqtt_pressure = this->register_sensor(bmp280->get_pressure_sensor()),
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

StartupTrigger *Application::make_startup_trigger() {
  return this->register_component(new StartupTrigger());
}

ShutdownTrigger *Application::make_shutdown_trigger() {
  return new ShutdownTrigger();
}

#ifdef USE_TEMPLATE_SENSOR
Application::MakeTemplateSensor Application::make_template_sensor(const std::string &name,
                                                                  uint32_t update_interval) {
  auto *template_ = this->register_component(new TemplateSensor(name, update_interval));

  return MakeTemplateSensor{
      .template_ = template_,
      .mqtt = this->register_sensor(template_),
  };
}
#endif

#ifdef USE_MAX6675_SENSOR
Application::MakeMAX6675Sensor Application::make_max6675_sensor(const std::string &name,
                                                                SPIComponent *spi_bus,
                                                                const GPIOOutputPin &cs,
                                                                uint32_t update_interval) {
  auto *sensor = this->register_component(
      new MAX6675Sensor(name, spi_bus, cs.copy(), update_interval)
  );

  return MakeMAX6675Sensor{
      .max6675 = sensor,
      .mqtt = this->register_sensor(sensor),
  };
}
#endif

#ifdef USE_TEMPLATE_BINARY_SENSOR
Application::MakeTemplateBinarySensor Application::make_template_binary_sensor(const std::string &name) {
  auto *template_ = this->register_component(new TemplateBinarySensor(name));

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
  return this->register_component(new remote::RemoteTransmitterComponent(output.copy()));
}
#endif

#ifdef USE_REMOTE_RECEIVER
remote::RemoteReceiverComponent *Application::make_remote_receiver_component(const GPIOInputPin &output) {
  return this->register_component(new remote::RemoteReceiverComponent(output.copy()));
}
#endif

#ifdef USE_DUTY_CYCLE_SENSOR
Application::MakeDutyCycleSensor Application::make_duty_cycle_sensor(const std::string &name,
                                                                     const GPIOInputPin &pin,
                                                                     uint32_t update_interval) {
  auto *duty = this->register_component(new DutyCycleSensor(name, pin.copy(), update_interval));

  return MakeDutyCycleSensor{
      .duty = duty,
      .mqtt = this->register_sensor(duty),
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
ESP32BLEBeacon *Application::make_esp32_ble_beacon(const std::array<uint8_t, 16> &uuid) {
  return this->register_component(new ESP32BLEBeacon(uuid));
}
#endif

#ifdef USE_STATUS_LED
StatusLEDComponent *Application::make_status_led(const GPIOOutputPin &pin) {
  return this->register_component(new StatusLEDComponent(pin.copy()));
}
#endif

#ifdef USE_MHZ19
Application::MakeMHZ19Sensor Application::make_mhz19_sensor(UARTComponent *parent,
                                                            const std::string &co2_name,
                                                            uint32_t update_interval) {
  auto *mhz19 = this->register_component(new MHZ19Component(parent, co2_name, update_interval));

  return MakeMHZ19Sensor{
      .mhz19 = mhz19,
      .mqtt = this->register_sensor(mhz19->get_co2_sensor()),
  };
}
#endif

#ifdef USE_UART
UARTComponent *Application::init_uart(int8_t tx_pin, int8_t rx_pin, uint32_t baud_rate) {
  return this->register_component(new UARTComponent(tx_pin, rx_pin, baud_rate));
}
#endif

#ifdef USE_SPI
SPIComponent *Application::init_spi(const GPIOOutputPin &clk, const GPIOInputPin &miso, const GPIOOutputPin &mosi) {
  return this->register_component(new SPIComponent(clk.copy(), miso.copy(), mosi.copy()));
}
SPIComponent *Application::init_spi(const GPIOOutputPin &clk) {
  return this->register_component(new SPIComponent(clk.copy(), nullptr, nullptr));
}
#endif

#ifdef USE_PN532
PN532Component *Application::make_pn532_component(SPIComponent *parent,
                                                  const GPIOOutputPin &cs,
                                                  uint32_t update_interval) {
  return this->register_component(new PN532Component(parent, cs.copy(), update_interval));
}
#endif

#ifdef USE_UART_SWITCH
Application::MakeUARTSwitch Application::make_uart_switch(UARTComponent *parent,
                                                          const std::string &name,
                                                          const std::vector<uint8_t> &data) {
  auto *uart = new UARTSwitch(parent, name, data);

  return MakeUARTSwitch{
      .uart = uart,
      .mqtt = this->register_switch(uart),
  };
}
#endif

#ifdef USE_UPTIME_SENSOR
Application::MakeUptimeSensor Application::make_uptime_sensor(const std::string &name, uint32_t update_interval) {
  auto *uptime = this->register_component(new UptimeSensor(name, update_interval));

  return MakeUptimeSensor{
      .uptime = uptime,
      .mqtt = this->register_sensor(uptime),
  };
}
#endif

#ifdef USE_INA219
sensor::INA219Component *Application::make_ina219(float shunt_resistance_ohm,
                                                  float max_current_a,
                                                  float max_voltage_v,
                                                  uint8_t address,
                                                  uint32_t update_interval) {
  return this->register_component(new INA219Component(this->i2c_,
                                                      shunt_resistance_ohm,
                                                      max_current_a,
                                                      max_voltage_v,
                                                      address,
                                                      update_interval));
}
#endif

#ifdef USE_INA3221
sensor::INA3221Component *Application::make_ina3221(uint8_t address, uint32_t update_interval) {
  return this->register_component(new INA3221Component(this->i2c_, address, update_interval));
}
#endif

#ifdef USE_HMC5883L
sensor::HMC5883LComponent *Application::make_hmc5883l(uint32_t update_interval) {
  return this->register_component(new HMC5883LComponent(this->i2c_, update_interval));
}
#endif

#ifdef USE_HX711
Application::MakeHX711Sensor Application::make_hx711_sensor(const std::string &name,
                                                            const GPIOInputPin &dout,
                                                            const GPIOOutputPin &sck,
                                                            uint32_t update_interval) {
  auto *hx711 = this->register_component(new HX711Sensor(name, dout.copy(), sck.copy(), update_interval));

  return MakeHX711Sensor{
      .hx711 = hx711,
      .mqtt = this->register_sensor(hx711),
  };
}
#endif

#ifdef USE_MS5611
Application::MakeMS5611Sensor Application::make_ms5611_sensor(const std::string &temperature_name,
                                                              const std::string &pressure_name,
                                                              uint32_t update_interval) {
  auto *ms5611 = this->register_component(
      new MS5611Component(this->i2c_, temperature_name, pressure_name, update_interval)
  );

  return MakeMS5611Sensor{
      .ms5611 = ms5611,
      .mqtt_temperature = this->register_sensor(ms5611->get_temperature_sensor()),
      .mqtt_pressure = this->register_sensor(ms5611->get_pressure_sensor()),
  };
}
#endif

#ifdef USE_RDM6300
binary_sensor::RDM6300Component *Application::make_rdm6300_component(UARTComponent *parent) {
  return this->register_component(new RDM6300Component(parent));
}
#endif
bool Application::is_fully_setup() const {
  return this->application_state_ == COMPONENT_STATE_SETUP || this->application_state_ == COMPONENT_STATE_LOOP;
}

#ifdef USE_TCS34725
sensor::TCS34725Component *Application::make_tcs34725(uint32_t update_interval) {
  return this->register_component(new TCS34725Component(this->i2c_, update_interval));
}
#endif

#ifdef USE_MAX7219
display::MAX7219Component *Application::make_max7219(SPIComponent *parent,
                                                     const GPIOOutputPin &cs,
                                                     uint32_t update_interval) {
  return this->register_component(new display::MAX7219Component(parent, cs.copy(), update_interval));
}
#endif

#ifdef USE_LCD_DISPLAY_PCF8574
display::PCF8574LCDDisplay *Application::make_pcf8574_lcd_display(uint8_t columns,
                                                                  uint8_t rows,
                                                                  uint8_t address,
                                                                  uint32_t update_interval) {
  return this->register_component(new display::PCF8574LCDDisplay(this->i2c_, columns, rows, address, update_interval));
}
#endif

#ifdef USE_SSD1306
#ifdef USE_SPI
display::SPISSD1306 *Application::make_spi_ssd1306(SPIComponent *parent,
                                                   const GPIOOutputPin &cs,
                                                   const GPIOOutputPin &dc,
                                                   uint32_t update_interval) {
  return this->register_component(new display::SPISSD1306(parent, cs.copy(), dc.copy(), update_interval));
}
#endif

#ifdef USE_I2C
display::I2CSSD1306 *Application::make_i2c_ssd1306(uint32_t update_interval) {
  return this->register_component(new display::I2CSSD1306(this->i2c_, update_interval));
}
#endif
#endif

#ifdef USE_WAVESHARE_EPAPER
display::WaveshareEPaperTypeA *Application::make_waveshare_epaper_type_a(SPIComponent *parent,
                                                                         const GPIOOutputPin &cs,
                                                                         const GPIOOutputPin &dc_pin,
                                                                         display::WaveshareEPaperTypeAModel model,
                                                                         uint32_t update_interval) {
  return this->register_component(
      new display::WaveshareEPaperTypeA(parent, cs.copy(), dc_pin.copy(), model, update_interval)
  );
}

display::WaveshareEPaper *Application::make_waveshare_epaper_type_b(SPIComponent *parent,
                                                                    const GPIOOutputPin &cs,
                                                                    const GPIOOutputPin &dc_pin,
                                                                    display::WaveshareEPaperTypeBModel model,
                                                                    uint32_t update_interval) {
  display::WaveshareEPaper *ret = nullptr;
  switch (model) {
    case display::WAVESHARE_EPAPER_2_7_IN:
      ret = new display::WaveshareEPaper2P7In(parent, cs.copy(), dc_pin.copy(), update_interval);
      break;
    case display::WAVESHARE_EPAPER_4_2_IN:
      ret = new display::WaveshareEPaper4P2In(parent, cs.copy(), dc_pin.copy(), update_interval);
      break;
    case display::WAVESHARE_EPAPER_7_5_IN:
      ret = new display::WaveshareEPaper7P5In(parent, cs.copy(), dc_pin.copy(), update_interval);
      break;
  }
  return this->register_component(ret);
}
#endif

#ifdef USE_DISPLAY
display::Font *Application::make_font(std::vector<display::Glyph> &&glyphs, int baseline, int bottom) {
  return new display::Font(std::move(glyphs), baseline, bottom);
}
display::Image *Application::make_image(const uint8_t *data_start, int width, int height) {
  return new display::Image(data_start, width, height);
}
#endif

#ifdef USE_LCD_DISPLAY
display::GPIOLCDDisplay *Application::make_gpio_lcd_display(uint8_t columns, uint8_t rows, uint32_t update_interval) {
  return this->register_component(new display::GPIOLCDDisplay(columns, rows, update_interval));
}
#endif

#ifdef USE_SNTP_COMPONENT
SNTPComponent *Application::make_sntp_component() {
  return this->register_component(new SNTPComponent());
}
#endif

#ifdef USE_HLW8012
sensor::HLW8012Component *Application::make_hlw8012(const GPIOOutputPin &sel_pin,
                                           uint8_t cf_pin,
                                           uint8_t cf1_pin,
                                           uint32_t update_interval) {
  return this->register_component(new HLW8012Component(sel_pin.copy(), cf_pin, cf1_pin, update_interval));
}
#endif

#ifdef USE_NEXTION
display::Nextion *Application::make_nextion(UARTComponent *parent, uint32_t update_interval) {
  return this->register_component(new display::Nextion(parent, update_interval));
}
#endif

#ifdef USE_TEXT_SENSOR
text_sensor::MQTTTextSensor *Application::register_text_sensor(text_sensor::TextSensor *sensor) {
  for (auto *controller : this->controllers_)
    controller->register_text_sensor(sensor);
  MQTTTextSensor *ret = nullptr;
  if (this->mqtt_client_ != nullptr)
    ret = this->register_component(new MQTTTextSensor(sensor));
  return ret;
}
#endif

#ifdef USE_MQTT_SUBSCRIBE_TEXT_SENSOR
Application::MakeMQTTSubscribeTextSensor Application::make_mqtt_subscribe_text_sensor(const std::string &name,
                                                                                      std::string topic) {
  auto *sensor = this->register_component(new MQTTSubscribeTextSensor(name, std::move(topic)));
  return MakeMQTTSubscribeTextSensor {
      .sensor = sensor,
      .mqtt = this->register_text_sensor(sensor),
  };
}
#endif

#ifdef USE_VERSION_TEXT_SENSOR
Application::MakeVersionTextSensor Application::make_version_text_sensor(const std::string &name) {
  auto *sensor = this->register_component(new VersionTextSensor(name));
  return MakeVersionTextSensor {
      .sensor = sensor,
      .mqtt = this->register_text_sensor(sensor),
  };
}
#endif

#ifdef USE_MQTT_SUBSCRIBE_SENSOR
Application::MakeMQTTSubscribeSensor Application::make_mqtt_subscribe_sensor(const std::string &name, std::string topic) {
  auto *sensor = this->register_component(new sensor::MQTTSubscribeSensor(name, std::move(topic)));

  return MakeMQTTSubscribeSensor {
      .sensor = sensor,
      .mqtt = this->register_sensor(sensor),
  };
}
#endif

#ifdef USE_TEMPLATE_TEXT_SENSOR
Application::MakeTemplateTextSensor Application::make_template_text_sensor(const std::string &name,
                                                                           uint32_t update_interval) {
  auto *template_ = this->register_component(new TemplateTextSensor(name, update_interval));

  return MakeTemplateTextSensor{
      .template_ = template_,
      .mqtt = this->register_text_sensor(template_),
  };
}
#endif

#ifdef USE_CSE7766
sensor::CSE7766Component *Application::make_cse7766(UARTComponent *parent) {
  return this->register_component(new sensor::CSE7766Component(parent));
}
#endif

#ifdef USE_PMSX003
sensor::PMSX003Component *Application::make_pmsx003(UARTComponent *parent, sensor::PMSX003Type type) {
  return this->register_component(new PMSX003Component(parent, type));
}
#endif

#ifdef USE_A4988
stepper::A4988 *Application::make_a4988(const GPIOOutputPin &step_pin, const GPIOOutputPin &dir_pin) {
  return this->register_component(new A4988(step_pin.copy(), dir_pin.copy()));
}
#endif

#ifdef USE_TOTAL_DAILY_ENERGY_SENSOR
Application::MakeTotalDailyEnergySensor Application::make_total_daily_energy_sensor(const std::string &name,
                                                                                    time::RealTimeClockComponent *time,
                                                                                    sensor::Sensor *parent) {
  auto total = this->register_component(new TotalDailyEnergy(name, time, parent));
  return {
      .total_energy = total,
      .mqtt = this->register_sensor(total)
  };
}
#endif

void Application::set_loop_interval(uint32_t loop_interval) {
  this->loop_interval_ = loop_interval;
}


Application App; // NOLINT

ESPHOMELIB_NAMESPACE_END
