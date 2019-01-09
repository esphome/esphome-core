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
  wifiap.set_ssid("fuckgfw");
  wifiap.set_password("refuckgfw");
  wificomponent->add_sta(wifiap);
  OTAComponent *otacomponent = App.init_ota();
  otacomponent->set_auth_password("1234567890");
  otacomponent->start_safe_mode();
  api::APIServer *api_apiserver = App.init_api_server();
  api_apiserver->set_password("1234567890");
  sensor::DallasComponent *sensor_dallascomponent = App.make_dallas_component(19);
  Application::MakeADCSensor application_makeadcsensor = App.make_adc_sensor("tds", 39, 10000);
  application_makeadcsensor.adc->set_attenuation(ADC_11db);
  sensor::ADCSensorComponent *sensor_adcsensorcomponent = application_makeadcsensor.adc;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_3 = application_makeadcsensor.mqtt;
  Application::MakeADCSensor application_makeadcsensor_2 = App.make_adc_sensor("adc_leaking", 36, 10000);
  application_makeadcsensor_2.adc->set_attenuation(ADC_11db);
  sensor::ADCSensorComponent *adc_leaking = application_makeadcsensor_2.adc;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_4 = application_makeadcsensor_2.mqtt;
  Application::MakeADCSensor application_makeadcsensor_3 = App.make_adc_sensor("ap", 33, 10000);
  application_makeadcsensor_3.adc->set_attenuation(ADC_11db);
  sensor::ADCSensorComponent *sensor_adcsensorcomponent_2 = application_makeadcsensor_3.adc;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_5 = application_makeadcsensor_3.mqtt;
  Application::MakeTemplateBinarySensor application_maketemplatebinarysensor = App.make_template_binary_sensor("bin_leaking");
  binary_sensor::TemplateBinarySensor *bin_leaking = application_maketemplatebinarysensor.template_;
  binary_sensor::MQTTBinarySensorComponent *binary_sensor_mqttbinarysensorcomponent_4 = application_maketemplatebinarysensor.mqtt;
  application_maketemplatebinarysensor.template_->set_template([=]() -> optional<bool> {
      if (adc_leaking->state < 0.7)
        return true;
      else
        return false;
  });
  sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor = sensor_dallascomponent->get_sensor_by_address("temp", 0x00);
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent = App.register_sensor(sensor_dallastemperaturesensor);
  Application::MakePulseCounterSensor application_makepulsecountersensor = App.make_pulse_counter_sensor("pc_a", 18, 10000);
  sensor::PulseCounterSensorComponent *sensor_pulsecountersensorcomponent = application_makepulsecountersensor.pcnt;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_6 = application_makepulsecountersensor.mqtt;
  Application::MakePulseCounterSensor application_makepulsecountersensor_2 = App.make_pulse_counter_sensor("pc_a", 17, 10000);
  sensor::PulseCounterSensorComponent *sensor_pulsecountersensorcomponent_2 = application_makepulsecountersensor_2.pcnt;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_7 = application_makepulsecountersensor_2.mqtt;
  Application::MakePulseCounterSensor application_makepulsecountersensor_3 = App.make_pulse_counter_sensor("pc_a", 16, 10000);
  sensor::PulseCounterSensorComponent *sensor_pulsecountersensorcomponent_3 = application_makepulsecountersensor_3.pcnt;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_8 = application_makepulsecountersensor_3.mqtt;
  Application::MakeGPIOBinarySensor application_makegpiobinarysensor = App.make_gpio_binary_sensor("sp", 35);
  binary_sensor::GPIOBinarySensorComponent *sp = application_makegpiobinarysensor.gpio;
  binary_sensor::MQTTBinarySensorComponent *binary_sensor_mqttbinarysensorcomponent = application_makegpiobinarysensor.mqtt;
  Application::MakeGPIOBinarySensor application_makegpiobinarysensor_2 = App.make_gpio_binary_sensor("sp", 34);
  binary_sensor::GPIOBinarySensorComponent *binary_sensor_gpiobinarysensorcomponent = application_makegpiobinarysensor_2.gpio;
  binary_sensor::MQTTBinarySensorComponent *binary_sensor_mqttbinarysensorcomponent_2 = application_makegpiobinarysensor_2.mqtt;
  Application::MakeGPIOBinarySensor application_makegpiobinarysensor_3 = App.make_gpio_binary_sensor("door", GPIOInputPin(23, INPUT_PULLUP));
  binary_sensor::GPIOBinarySensorComponent *binary_sensor_gpiobinarysensorcomponent_2 = application_makegpiobinarysensor_3.gpio;
  binary_sensor::MQTTBinarySensorComponent *binary_sensor_mqttbinarysensorcomponent_3 = application_makegpiobinarysensor_3.mqtt;
  Application::MakeGPIOSwitch application_makegpioswitch = App.make_gpio_switch("r1", 14);
  switch_::GPIOSwitch *r1 = application_makegpioswitch.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent = application_makegpioswitch.mqtt;
  Application::MakeGPIOSwitch application_makegpioswitch_2 = App.make_gpio_switch("r2", 13);
  switch_::GPIOSwitch *r2 = application_makegpioswitch_2.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent_2 = application_makegpioswitch_2.mqtt;
  Application::MakeGPIOSwitch application_makegpioswitch_3 = App.make_gpio_switch("r3", 32);
  switch_::GPIOSwitch *r3 = application_makegpioswitch_3.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent_3 = application_makegpioswitch_3.mqtt;
  Application::MakeGPIOSwitch application_makegpioswitch_4 = App.make_gpio_switch("r4", 4);
  switch_::GPIOSwitch *r4 = application_makegpioswitch_4.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent_4 = application_makegpioswitch_4.mqtt;
  Application::MakeGPIOSwitch application_makegpioswitch_5 = App.make_gpio_switch("r5", 25);
  switch_::GPIOSwitch *r5 = application_makegpioswitch_5.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent_5 = application_makegpioswitch_5.mqtt;
  Application::MakeGPIOSwitch application_makegpioswitch_6 = App.make_gpio_switch("r6", 27);
  switch_::GPIOSwitch *r6 = application_makegpioswitch_6.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent_6 = application_makegpioswitch_6.mqtt;
  SPIComponent *spicomponent = App.init_spi(5);
  StatusLEDComponent *statusledcomponent = App.make_status_led(GPIOOutputPin(12, OUTPUT, true));
  sensor_adcsensorcomponent->set_filters({});
  adc_leaking->set_filters({});
  sensor_adcsensorcomponent_2->set_filters({});
  Application::MakeHX711Sensor application_makehx711sensor = App.make_hx711_sensor("weight", 21, 22, 15000);
  application_makehx711sensor.hx711->set_gain(sensor::HX711_GAIN_128);
  sensor::HX711Sensor *sensor_hx711sensor = application_makehx711sensor.hx711;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_2 = application_makehx711sensor.mqtt;
  spicomponent->set_mosi(2);
  sensor_dallastemperaturesensor->set_filters({});
  sensor_pulsecountersensorcomponent->set_filters({});
  sensor_pulsecountersensorcomponent_2->set_filters({});
  sensor_pulsecountersensorcomponent_3->set_filters({});
  display::MAX7219Component *display_max7219component = App.make_max7219(spicomponent, 15);
  display_max7219component->set_num_chips(1);
  sensor_hx711sensor->set_filters({});
  display_max7219component->set_writer([=](display::MAX7219Component & it) {
      it.print("01234567");    
  });
  // =========== AUTO GENERATED CODE END ============
  // ========= YOU CAN EDIT AFTER THIS LINE =========
  App.setup();
}

void loop() {
  App.loop();
}
