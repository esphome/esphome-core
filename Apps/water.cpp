// Auto generated code by esphomeyaml
// ========== AUTO GENERATED INCLUDE BLOCK BEGIN ===========
#include "esphomelib/application.h"
using namespace esphomelib;

Application::MakeADCSensor* adc_tds;
Application::MakeADCSensor* adc_leaking;
Application::MakeADCSensor* adc_ap;

Application::MakePulseCounterSensor* pc_a;
Application::MakePulseCounterSensor* pc_b;
Application::MakePulseCounterSensor* pc_c;

Application::MakeGPIOBinarySensor* bs_sp;
Application::MakeGPIOBinarySensor* bs_bp;
Application::MakeGPIOBinarySensor* bs_door;

Application::MakeGPIOSwitch* r1;
Application::MakeGPIOSwitch* r2;
Application::MakeGPIOSwitch* r3;
Application::MakeGPIOSwitch* r4;
Application::MakeGPIOSwitch* r5;
Application::MakeGPIOSwitch* r6;


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
  Application::MakeADCSensor sensor_adc_tds = App.make_adc_sensor("tds", 39, 10000);
  adc_tds = &sensor_adc_tds;
  sensor_adc_tds.adc->set_attenuation(ADC_11db);
  adc_tds->adc->set_filters({});

  Application::MakeADCSensor sensor_adc_leaking = App.make_adc_sensor("leaking", 36, 10000);
  adc_leaking = &sensor_adc_leaking;
  adc_leaking->adc->set_attenuation(ADC_11db);
  adc_leaking->adc->set_filters({});

  Application::MakeADCSensor sensor_adc_ap = App.make_adc_sensor("ap", 33, 10000);
  adc_ap = &sensor_adc_ap;
  adc_ap->adc->set_attenuation(ADC_11db);
  adc_ap->adc->set_filters({});
 
  //sensors: Pulse_counter
  Application::MakePulseCounterSensor pc_pc_a = App.make_pulse_counter_sensor("pc_a", 18, 10000);
  pc_a = &pc_pc_a;
  pc_a->pcnt->set_filters({});

  Application::MakePulseCounterSensor pc_pc_b = App.make_pulse_counter_sensor("pc_b", 17, 10000);
  pc_b = &pc_pc_b;
  pc_b->pcnt->set_filters({});

  Application::MakePulseCounterSensor pc_pc_c = App.make_pulse_counter_sensor("pc_c", 16, 10000);
  pc_c = &pc_pc_c;
  pc_c->pcnt->set_filters({});

  //sensors: gpio_binary
  Application::MakeGPIOBinarySensor sensor_gpio_binary_sp = App.make_gpio_binary_sensor("sp", 35);
  bs_sp = &sensor_gpio_binary_sp;

  Application::MakeGPIOBinarySensor sensor_gpio_binary_bp = App.make_gpio_binary_sensor("bp", 34);
  bs_bp = &sensor_gpio_binary_bp;

  Application::MakeGPIOBinarySensor sensor_gpio_binary_door = App.make_gpio_binary_sensor("door", 23);
  bs_door = &sensor_gpio_binary_door;

  //switch.gpio
  Application::MakeGPIOSwitch switch_gpio_r1 = App.make_gpio_switch("r1", 14);
  r1 = &switch_gpio_r1;

  Application::MakeGPIOSwitch switch_gpio_r2 = App.make_gpio_switch("r2", 13);
  r2 = &switch_gpio_r2;

  Application::MakeGPIOSwitch switch_gpio_r3 = App.make_gpio_switch("r3", 32);
  r3 = &switch_gpio_r3;

  Application::MakeGPIOSwitch switch_gpio_r4 = App.make_gpio_switch("r4", 4);
  r4 = &switch_gpio_r4;

  Application::MakeGPIOSwitch switch_gpio_r5 = App.make_gpio_switch("r5", 25);
  r5 = &switch_gpio_r5;

  Application::MakeGPIOSwitch switch_gpio_r6 = App.make_gpio_switch("r6", 27);
  r6 = &switch_gpio_r6;


  //Status_led
  StatusLEDComponent *status_led = App.make_status_led(12);

  //SPI and max7219 display
  SPIComponent *spicomponent = App.init_spi(5);
  spicomponent->set_mosi(2);
  display::MAX7219Component *display_max7219 = App.make_max7219(spicomponent, 15);
  display_max7219->set_num_chips(1);
  display_max7219->set_writer([=](display::MAX7219Component & it) {
      it.print("01234567");    
  });

  App.setup();
}

void myloop()
{
  //automation logic.
  bool br1 = true;
  if(adc_leaking->adc->state > 100)
    br1 = false;
  if(adc_ap->adc->state > 800)
    br1 = false;

  //output to relay.
  if(br1)
    r1->switch_->turn_on();  //will mqtt public the new state?
  else
    r1->switch_->turn_off();
  
}
void loop() {
  App.loop();
  myloop();
}