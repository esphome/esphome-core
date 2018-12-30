// Auto generated code by esphomeyaml
// ========== AUTO GENERATED INCLUDE BLOCK BEGIN ===========
#include "esphomelib/application.h"
using namespace esphomelib;
// ========== AUTO GENERATED INCLUDE BLOCK END ==========="

void setup() {
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

  //sensors:  Dallas
  // sensor::DallasComponent *sensor_dallascomponent = App.make_dallas_component(19);
  // sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor = sensor_dallascomponent->get_sensor_by_address("temp", 0x00);
  // sensor::MQTTSensorComponent *sensor_mqttsensorcomponent = App.register_sensor(sensor_dallastemperaturesensor);

  //sensors: Adc
  Application::MakeADCSensor application_makeadcsensor = App.make_adc_sensor("tds", 39, 10000);
  application_makeadcsensor.adc->set_attenuation(ADC_11db);
  sensor::ADCSensorComponent *sensor_adcsensorcomponent = application_makeadcsensor.adc;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_2 = application_makeadcsensor.mqtt;

  Application::MakeADCSensor application_makeadcsensor_2 = App.make_adc_sensor("leaking", 36, 10000);
  sensor::ADCSensorComponent *leaking = application_makeadcsensor_2.adc;
  application_makeadcsensor_2.adc->set_attenuation(ADC_11db);
  sensor::ADCSensorComponent *sensor_adcsensorcomponent_2 = application_makeadcsensor_2.adc;

  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_3 = application_makeadcsensor_2.mqtt;
  Application::MakeADCSensor application_makeadcsensor_3 = App.make_adc_sensor("ap", 33, 10000);
  application_makeadcsensor_3.adc->set_attenuation(ADC_11db);

  sensor::ADCSensorComponent *sensor_adcsensorcomponent_3 = application_makeadcsensor_3.adc;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_4 = application_makeadcsensor_3.mqtt;

  sensor_adcsensorcomponent->set_filters({});
  sensor_adcsensorcomponent_2->set_filters({});
  sensor_adcsensorcomponent_3->set_filters({});

  //sensors: Pulse_counter
  Application::MakePulseCounterSensor application_makepulsecountersensor = App.make_pulse_counter_sensor("pc_a", 18, 10000);
  sensor::PulseCounterSensorComponent *sensor_pulsecountersensorcomponent = application_makepulsecountersensor.pcnt;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_5 = application_makepulsecountersensor.mqtt;
  Application::MakePulseCounterSensor application_makepulsecountersensor_2 = App.make_pulse_counter_sensor("pc_a", 17, 10000);
  sensor::PulseCounterSensorComponent *sensor_pulsecountersensorcomponent_2 = application_makepulsecountersensor_2.pcnt;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_6 = application_makepulsecountersensor_2.mqtt;
  Application::MakePulseCounterSensor application_makepulsecountersensor_3 = App.make_pulse_counter_sensor("pc_a", 16, 10000);
  sensor::PulseCounterSensorComponent *sensor_pulsecountersensorcomponent_3 = application_makepulsecountersensor_3.pcnt;
  sensor::MQTTSensorComponent *sensor_mqttsensorcomponent_7 = application_makepulsecountersensor_3.mqtt;

  sensor_pulsecountersensorcomponent->set_filters({});
  sensor_pulsecountersensorcomponent_2->set_filters({});
  sensor_pulsecountersensorcomponent_3->set_filters({});

  //sensors: gpio_binary
  Application::MakeGPIOBinarySensor application_makegpiobinarysensor = App.make_gpio_binary_sensor("sp", 35);
  binary_sensor::GPIOBinarySensorComponent *binary_sensor_gpiobinarysensorcomponent = application_makegpiobinarysensor.gpio;
  binary_sensor::MQTTBinarySensorComponent *binary_sensor_mqttbinarysensorcomponent = application_makegpiobinarysensor.mqtt;
  Application::MakeGPIOBinarySensor application_makegpiobinarysensor_2 = App.make_gpio_binary_sensor("sp", 34);
  binary_sensor::GPIOBinarySensorComponent *binary_sensor_gpiobinarysensorcomponent_2 = application_makegpiobinarysensor_2.gpio;
  binary_sensor::MQTTBinarySensorComponent *binary_sensor_mqttbinarysensorcomponent_2 = application_makegpiobinarysensor_2.mqtt;
  Application::MakeGPIOBinarySensor application_makegpiobinarysensor_3 = App.make_gpio_binary_sensor("door", 23);
  binary_sensor::GPIOBinarySensorComponent *binary_sensor_gpiobinarysensorcomponent_3 = application_makegpiobinarysensor_3.gpio;
  binary_sensor::MQTTBinarySensorComponent *binary_sensor_mqttbinarysensorcomponent_3 = application_makegpiobinarysensor_3.mqtt;

  //switch.gpio
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

  //Status_led
  StatusLEDComponent *statusledcomponent = App.make_status_led(12);

  //SPI and max7219 display
  SPIComponent *spicomponent = App.init_spi(5);
  spicomponent->set_mosi(2);
  display::MAX7219Component *display_max7219component = App.make_max7219(spicomponent, 15);
  display_max7219component->set_num_chips(1);
  display_max7219component->set_writer([=](display::MAX7219Component & it) {
      it.print("01234567");    
  });




  // //automation::script
  // Script *my_script = new Script();

  // //automation:: prepare actions
  // switch_::GPIOSwitch *r1 = application_makegpioswitch.switch_;

  // //automation::trigger:: create,setting,register.
  // LambdaAction<NoArg> *action = new LambdaAction<NoArg>([=](NoArg x) {
  //     if(leaking->state >100)
  //     {
  //       r1->turn_on();
  //     }
  // });

  // //automation::trigger:: create,setting,register.
  // sensor::ValueRangeTrigger *sensor_valuerangetrigger = sensor_adcsensorcomponent_2->make_value_range_trigger();
  //     sensor_valuerangetrigger->set_min(100.000000f);
  // App.register_component(sensor_valuerangetrigger);
  // //automation::trigger:: create,setting,register.
  // sensor::ValueRangeTrigger *sensor_valuerangetrigger_2 = sensor_adcsensorcomponent_2->make_value_range_trigger();
  //     sensor_valuerangetrigger_2->set_max(80.000000f);
  // App.register_component(sensor_valuerangetrigger_2);
    

  // //automation:: bind_trigger
  // Automation<float> *automation = App.make_automation<float>(sensor_valuerangetrigger);
  // Automation<float> *automation_2 = App.make_automation<float>(sensor_valuerangetrigger_2);
  // Automation<NoArg> *automation_3 = App.make_automation<NoArg>(my_script);




  
  // //automation::actions   create,setting, 
  // switch_::TurnOnAction<NoArg> *action_3 = r1->make_turn_on_action<NoArg>();
  
  // switch_::TurnOffAction<NoArg> *action_5 = r1->make_turn_off_action<NoArg>();
  // ScriptExecuteAction<float> *action = my_script->make_execute_action<float>();
  // ScriptExecuteAction<float> *action_2 = my_script->make_execute_action<float>();

  // DelayAction<NoArg> *action_4 = App.register_component(new DelayAction<NoArg>());
  //     action_4->set_delay(1000);

  // //automation::  add_to/bind/register_to automation
  // automation->add_actions({action});
  // automation_2->add_actions({action_2});
  // automation_3->add_actions({action_3, action_4, action_5});




  App.setup();
}

void loop() {
  App.loop();
}