//
// Created by Otto Winter on 25.11.17.
//

#include "application.h"
#include "wifi_component.h"

#include <utility>
#include <algorithm>
#include <esp_log.h>
#include <esphomelib/output/gpio_binary_output_component.h>

namespace esphomelib {

using namespace esphomelib::mqtt;
using namespace esphomelib::input;
using namespace esphomelib::binary_sensor;
using namespace esphomelib::sensor;
using namespace esphomelib::output;
using namespace esphomelib::light;
using namespace esphomelib::switch_platform;

static const char *TAG = "app";

void Application::setup() {
  ESP_LOGV(TAG, "Sorting components by setup priority...");
  std::sort(this->components_.begin(), this->components_.end(), [](const Component *a, const Component *b) {
    return a->get_setup_priority() > b->get_setup_priority();
  });
  ESP_LOGD(TAG, "Calling setup");
  for (Component *component : this->components_)
    component->setup_();

  ESP_LOGV(TAG, "Sorting components by loop priority...");
  std::sort(this->components_.begin(), this->components_.end(), [](const Component *a, const Component *b) {
    return a->get_loop_priority() > b->get_loop_priority();
  });
}

void Application::loop() {
  for (Component *component : this->components_)
    component->loop_();
  yield();
}

WiFiComponent *Application::init_wifi(const std::string &ssid, const std::string &password) {
  assert_nullptr(this->wifi_);
  WiFiComponent *wifi = new WiFiComponent(ssid, password, generate_hostname(this->name_));
  this->wifi_ = wifi;
  return this->register_component(wifi);
}

void Application::set_name(const std::string &name) {
  this->name_ = to_lowercase_underscore(name);
}

MQTTClientComponent *Application::init_mqtt(const std::string &address, uint16_t port,
                                            const std::string &username, const std::string &password,
                                            const std::string &discovery_prefix) {
  MQTTClientComponent *component = new MQTTClientComponent(MQTTCredentials{
      .address = address,
      .port = port,
      .username = username,
      .password = password,
      .client_id = generate_hostname(this->name_)
  }, this->name_);
  component->set_discovery_info(discovery_prefix, true);
  this->mqtt_client_ = component;

  return this->register_component(component);
}

MQTTClientComponent *Application::init_mqtt(const std::string &address,
                                            const std::string &username, const std::string &password,
                                            const std::string &discovery_prefix) {
  return this->init_mqtt(address, 1883, username, password, discovery_prefix);
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

ATXComponent *Application::make_atx(uint8_t pin, uint32_t enable_time, uint32_t keep_on_time) {
  auto *atx = new ATXComponent(pin, enable_time, keep_on_time);
  return this->register_component(atx);
}

GPIOBinarySensorComponent *Application::make_gpio_binary_sensor(uint8_t pin, uint8_t mode, binary_callback_t callback) {
  auto *io = new GPIOBinarySensorComponent(pin, mode);
  io->set_on_new_state_callback(std::move(callback));
  return this->register_component(io);
}

MQTTBinarySensorComponent *Application::make_mqtt_binary_sensor(std::string friendly_name,
                                                                std::string device_class) {
  auto *mqtt = new MQTTBinarySensorComponent(std::move(friendly_name), std::move(device_class));
  return this->register_mqtt_component(mqtt);
}

Application::SimpleBinarySensor Application::make_simple_gpio_binary_sensor(std::string friendly_name,
                                                                            std::string device_class,
                                                                            uint8_t pin) {
  SimpleBinarySensor s{};
  s.mqtt = this->make_mqtt_binary_sensor(std::move(friendly_name), std::move(device_class));
  s.gpio = this->make_gpio_binary_sensor(pin);
  this->connect_binary_sensor_pair(s.gpio, s.mqtt);
  return s;
}

MQTTSensorComponent *Application::make_mqtt_sensor(std::string friendly_name,
                                                   std::string unit_of_measurement, Optional<uint32_t> expire_after) {
  auto *mqtt = new MQTTSensorComponent(std::move(friendly_name), std::move(unit_of_measurement),
                                       expire_after);
  return this->register_mqtt_component(mqtt);
}

Application::MakeDHTComponent Application::make_dht_component(uint8_t pin,
                                                              const std::string &temperature_friendly_name,
                                                              const std::string &humidity_friendly_name,
                                                              uint32_t check_interval) {
  auto *dht = new DHTComponent(pin, check_interval);
  auto *mqtt_temperature =
      this->make_mqtt_sensor_for(dht->get_temperature_sensor(), temperature_friendly_name);
  auto *mqtt_humidity = this->make_mqtt_sensor_for(dht->get_humidity_sensor(), humidity_friendly_name);
  mqtt_temperature->set_filter(new SlidingWindowMovingAverageFilter(15, 15));
  mqtt_temperature->set_expire_after(check_interval * 30 / 1000);
  mqtt_humidity->set_filter(new SlidingWindowMovingAverageFilter(15, 15));
  mqtt_humidity->set_expire_after(check_interval * 30 / 1000);
  this->register_component(dht);
  return MakeDHTComponent{
      .dht = dht,
      .mqtt_temperature = mqtt_temperature,
      .mqtt_humidity = mqtt_humidity
  };
}

sensor::MQTTSensorComponent *Application::make_mqtt_sensor_for(sensor::Sensor *sensor,
                                                               std::string friendly_name,
                                                               Optional<uint32_t> expire_after) {
  auto *mqtt = this->make_mqtt_sensor(std::move(friendly_name), sensor->unit_of_measurement(), expire_after);
  sensor->set_new_value_callback(mqtt->create_new_data_callback());
  return mqtt;
}

LEDCOutputComponent *Application::make_ledc_component(uint8_t pin, float frequency, uint8_t bit_depth) {
  auto *ledc = new LEDCOutputComponent(pin, frequency, bit_depth);
  return this->register_component(ledc);
}

PCA9685OutputComponent *Application::make_pca9685_component(float frequency, TwoWire &i2c_wire) {
  auto *pca9685 = new PCA9685OutputComponent(frequency, i2c_wire);
  return this->register_component(pca9685);
}

Application::LightStruct Application::make_rgb_light(const std::string &friendly_name,
                                                     FloatOutput *red, FloatOutput *green, FloatOutput *blue) {
  auto *out = new LinearLightOutputComponent();
  out->setup_rgb(red, green, blue);
  this->register_component(out);

  return this->connect_light(friendly_name, out);
}

Application::LightStruct Application::make_rgbw_light(const std::string &friendly_name,
                                                      output::FloatOutput *red,
                                                      output::FloatOutput *green,
                                                      output::FloatOutput *blue,
                                                      output::FloatOutput *white) {
  auto *out = new LinearLightOutputComponent();
  out->setup_rgbw(red, green, blue, white);
  this->register_component(out);

  return this->connect_light(friendly_name, out);
}

MQTTJSONLightComponent *Application::make_mqtt_light(const std::string &friendly_name, LightState *state) {
  auto *mqtt = new MQTTJSONLightComponent(friendly_name);
  mqtt->set_state(state);
  return this->register_mqtt_component(mqtt);
}
WiFiComponent *Application::get_wifi() const {
  return this->wifi_;
}
OTAComponent *Application::init_ota() {
  auto *ota = new OTAComponent();
  ota->set_hostname(this->wifi_->get_hostname());
  this->register_component(ota);
}
void Application::connect_binary_sensor_pair(BinarySensor *binary_sensor, MQTTBinarySensorComponent *mqtt) {
  binary_sensor->set_on_new_state_callback(mqtt->create_on_new_state_callback());
}
Application::LightStruct Application::make_monochromatic_light(const std::string &friendly_name,
                                                               output::FloatOutput *mono) {
  auto *out = new LinearLightOutputComponent();
  out->setup_monochromatic(mono);
  this->register_component(out);

  return this->connect_light(friendly_name, out);
}
Application::LightStruct Application::make_binary_light(const std::string &friendly_name,
                                                        output::BinaryOutput *binary) {
  auto *out = new LinearLightOutputComponent();
  out->setup_binary(binary);
  this->register_component(out);

  return this->connect_light(friendly_name, out);
}
Application::LightStruct Application::connect_light(const std::string &friendly_name,
                                                    light::LinearLightOutputComponent *out) {
  LightStruct s{};
  s.state = new LightState(out->get_traits());
  out->set_state(s.state);
  s.output = out;
  s.mqtt = this->make_mqtt_light(friendly_name, s.state);

  return s;
}
MQTTClientComponent *Application::get_mqtt_client() const {
  return this->mqtt_client_;
}
MQTTSwitchComponent *Application::make_mqtt_switch(const std::string &friendly_name) {
  return this->register_mqtt_component(new MQTTSwitchComponent(friendly_name));
}
void Application::connect_switch(switch_platform::Switch *switch_, switch_platform::MQTTSwitchComponent *mqtt) {
  switch_->set_on_new_state_callback(mqtt->create_on_new_state_callback());
  mqtt->set_write_value_callback(switch_->create_write_state_callback());
}
IRTransmitterComponent *Application::make_ir_transmitter(uint8_t pin,
                                                         uint8_t carrier_duty_percent,
                                                         uint8_t clock_divider) {
  return this->register_component(new IRTransmitterComponent(pin, carrier_duty_percent, clock_divider));
}
MQTTSwitchComponent *Application::make_mqtt_switch_for(const std::string &friendly_name,
                                                       switch_platform::Switch *switch_) {
  auto *mqtt = this->make_mqtt_switch(friendly_name);
  this->connect_switch(switch_, mqtt);
  return mqtt;
}
void Application::assert_name() const {
  assert(!this->name_.empty());
}
input::DallasComponent *Application::make_dallas_component(OneWire *one_wire) {
  return this->register_component(new DallasComponent(one_wire));
}
input::DallasComponent *Application::make_dallas_component(uint8_t pin) {
  return this->make_dallas_component(new OneWire(pin));
}

Application::SimpleGPIOSwitchStruct Application::make_simple_gpio_switch(uint8_t pin,
                                                                         const std::string &friendly_name) {
  auto *binary_output = this->register_component(new GPIOBinaryOutputComponent(pin));
  auto *simple_switch = new SimpleSwitch(binary_output);
  auto *mqtt = this->make_mqtt_switch_for(friendly_name, simple_switch);

  return {
      .simple_switch = simple_switch,
      .mqtt_switch = mqtt
  };
}

const std::string &Application::get_name() const {
  return this->name_;
}

Application::Application() {
  global_application = this;
}

Application *global_application;

} // namespace esphomelib
