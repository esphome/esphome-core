// Auto generated code by esphomeyaml
// ========== AUTO GENERATED INCLUDE BLOCK BEGIN ===========
#include "esphomelib/application.h"
using namespace esphomelib;
// ========== AUTO GENERATED INCLUDE BLOCK END ==========="

void setup() {
  // ===== DO NOT EDIT ANYTHING BELOW THIS LINE =====
  // ========== AUTO GENERATED CODE BEGIN ===========
  App.set_name("water");
  App.set_compilation_datetime(__DATE__ ", " __TIME__);
  LogComponent *logcomponent = App.init_log(115200);
  WiFiComponent *wificomponent = App.init_wifi();
  WiFiAP wifiap = WiFiAP();
  wifiap.set_ssid("FuckGFW");
  wifiap.set_password("refuckgfw");
  wificomponent->add_sta(wifiap);
  OTAComponent *otacomponent = App.init_ota();
  otacomponent->set_auth_password("1234567890");
  otacomponent->start_safe_mode();
  api::APIServer *api_apiserver = App.init_api_server();
  api_apiserver->set_password("1234567890");
  mqtt::MQTTClientComponent *mqtt_mqttclientcomponent = App.init_mqtt("voicevon.vicp.io", 1883, "von", "von1970");
  // sensor::DallasComponent *sensor_dallascomponent = App.make_dallas_component(19);
  Application::MakeADCSensor application_makeadcsensor = App.make_adc_sensor("tds", 39, 10000);
  application_makeadcsensor.adc->set_attenuation(ADC_11db);
  sensor::ADCSensorComponent *sensor_adcsensorcomponent = application_makeadcsensor.adc;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_2 = application_makeadcsensor.mqtt;
  Application::MakeADCSensor application_makeadcsensor_2 = App.make_adc_sensor("leaking", 36, 10000);
  application_makeadcsensor_2.adc->set_attenuation(ADC_11db);
  sensor::ADCSensorComponent *sensor_adcsensorcomponent_2 = application_makeadcsensor_2.adc;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_3 = application_makeadcsensor_2.mqtt;
  Application::MakeADCSensor application_makeadcsensor_3 = App.make_adc_sensor("ap", 33, 10000);
  application_makeadcsensor_3.adc->set_attenuation(ADC_11db);
  sensor::ADCSensorComponent *sensor_adcsensorcomponent_3 = application_makeadcsensor_3.adc;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_4 = application_makeadcsensor_3.mqtt;
  // sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor = sensor_dallascomponent->get_sensor_by_address("temp", 0x00);
  // sensor::MQTTSensorComponent *sensor_mqttsensorcomponent = App.register_sensor(sensor_dallastemperaturesensor);
  Application::MakePulseCounterSensor application_makepulsecountersensor = App.make_pulse_counter_sensor("pc_a", 18, 10000);
  sensor::PulseCounterSensorComponent *sensor_pulsecountersensorcomponent = application_makepulsecountersensor.pcnt;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_5 = application_makepulsecountersensor.mqtt;
  Application::MakePulseCounterSensor application_makepulsecountersensor_2 = App.make_pulse_counter_sensor("pc_a", 17, 10000);
  sensor::PulseCounterSensorComponent *sensor_pulsecountersensorcomponent_2 = application_makepulsecountersensor_2.pcnt;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_6 = application_makepulsecountersensor_2.mqtt;
  Application::MakePulseCounterSensor application_makepulsecountersensor_3 = App.make_pulse_counter_sensor("pc_a", 16, 10000);
  sensor::PulseCounterSensorComponent *sensor_pulsecountersensorcomponent_3 = application_makepulsecountersensor_3.pcnt;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_7 = application_makepulsecountersensor_3.mqtt;
  Application::MakeGPIOBinarySensor application_makegpiobinarysensor = App.make_gpio_binary_sensor("sp", 35);
  binary_sensor::GPIOBinarySensorComponent *binary_sensor_gpiobinarysensorcomponent = application_makegpiobinarysensor.gpio;
  binary_sensor::MQTTBinarySensorComponent *binary_sensor_mqttbinarysensorcomponent = application_makegpiobinarysensor.mqtt;
  Application::MakeGPIOBinarySensor application_makegpiobinarysensor_2 = App.make_gpio_binary_sensor("sp", 34);
  binary_sensor::GPIOBinarySensorComponent *binary_sensor_gpiobinarysensorcomponent_2 = application_makegpiobinarysensor_2.gpio;
  binary_sensor::MQTTBinarySensorComponent *binary_sensor_mqttbinarysensorcomponent_2 = application_makegpiobinarysensor_2.mqtt;
  Application::MakeGPIOBinarySensor application_makegpiobinarysensor_3 = App.make_gpio_binary_sensor("door", 23);
  binary_sensor::GPIOBinarySensorComponent *binary_sensor_gpiobinarysensorcomponent_3 = application_makegpiobinarysensor_3.gpio;
  binary_sensor::MQTTBinarySensorComponent *binary_sensor_mqttbinarysensorcomponent_3 = application_makegpiobinarysensor_3.mqtt;
  Application::MakeGPIOSwitch application_makegpioswitch = App.make_gpio_switch("r1", 14);
  switch_::GPIOSwitch *switch__gpioswitch = application_makegpioswitch.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent = application_makegpioswitch.mqtt;
  Application::MakeGPIOSwitch application_makegpioswitch_2 = App.make_gpio_switch("r2", 13);
  switch_::GPIOSwitch *switch__gpioswitch_2 = application_makegpioswitch_2.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent_2 = application_makegpioswitch_2.mqtt;
  Application::MakeGPIOSwitch application_makegpioswitch_3 = App.make_gpio_switch("r3", 32);
  switch_::GPIOSwitch *switch__gpioswitch_3 = application_makegpioswitch_3.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent_3 = application_makegpioswitch_3.mqtt;

  Application::MakeGPIOSwitch application_makegpioswitch_4 = App.make_gpio_switch("r4", 4);
  switch_::GPIOSwitch *switch__gpioswitch_4 = application_makegpioswitch_4.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent_4 = application_makegpioswitch_4.mqtt;

  
  Application::MakeGPIOSwitch application_makegpioswitch_5 = App.make_gpio_switch("r5", 25);
  switch_::GPIOSwitch *switch__gpioswitch_5 = application_makegpioswitch_5.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent_5 = application_makegpioswitch_5.mqtt;
  Application::MakeGPIOSwitch application_makegpioswitch_6 = App.make_gpio_switch("r6", 27);
  switch_::GPIOSwitch *switch__gpioswitch_6 = application_makegpioswitch_6.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent_6 = application_makegpioswitch_6.mqtt;
  StatusLEDComponent *statusledcomponent = App.make_status_led(12);
  sensor_adcsensorcomponent->set_filters({});
  sensor_adcsensorcomponent_2->set_filters({});
  sensor_adcsensorcomponent_3->set_filters({});
  sensor_pulsecountersensorcomponent->set_filters({});
  sensor_pulsecountersensorcomponent_2->set_filters({});
  sensor_pulsecountersensorcomponent_3->set_filters({});
  // =========== AUTO GENERATED CODE END ============
  // ========= YOU CAN EDIT AFTER THIS LINE =========
  App.setup();
}

void loop() {
  App.loop();
}