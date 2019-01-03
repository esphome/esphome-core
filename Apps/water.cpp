// Auto generated code by esphomeyaml
// ========== AUTO GENERATED INCLUDE BLOCK BEGIN ===========
#include "esphomelib/application.h"
using namespace esphomelib;

sensor::HX711Sensor* weight;
sensor::ADCSensorComponent* tds;
sensor::ADCSensorComponent* adc_leaking;
sensor::ADCSensorComponent* adc_ap;

sensor::PulseCounterSensorComponent* pc_a;
sensor::PulseCounterSensorComponent* pc_b;
sensor::PulseCounterSensorComponent* pc_c;

binary_sensor::GPIOBinarySensorComponent*  bs_sp;
binary_sensor::GPIOBinarySensorComponent* bs_bp;
binary_sensor::GPIOBinarySensorComponent* bs_door;

binary_sensor::TemplateBinarySensor * leaking;

switch_::GPIOSwitch* r1;
switch_::GPIOSwitch*  r2;
switch_::GPIOSwitch*  r3;
switch_::GPIOSwitch*  r4;
switch_::GPIOSwitch*  r5;
switch_::GPIOSwitch*  r6;

// next version:  a component named weighing_scale_hx711
// action_1 :=  reset zero mass value (long new_value)
// action_2 :=  reset second mass and value (float second_mess, long new_value) , will change the g_slope
float g_value_of_zero_mass = 8388608.0f;
float g_slope = 0.000123;
// result = (value - g_value_of_zero_mass) * slope


void setup_apps_ondebug(){
  //sensor::HX711
  Application::MakeHX711Sensor hx711_weight = App.make_hx711_sensor("weight", 22, 21, 2000);
  weight = hx711_weight.hx711;
  weight->set_gain(sensor::HX711_GAIN_128);
  weight->set_unit_of_measurement("Kg");
  weight->set_filters({
      new sensor::LambdaFilter([=](float x) {
        auto first_mass = 0.0; // first known mass was 0kg
        auto first_value = g_value_of_zero_mass; // value for the first known mass was 120
        auto second_mass = 1000.0; // second mass was 1kg
        auto second_value = 8588609; // second value was 810
        return map(x, first_value, second_value, first_mass, second_mass);
    }),
  });
}

void setup3_gpioPin(){
  
  //sensors:  Dallas
  // sensor::DallasComponent *sensor_dallascomponent = App.make_dallas_component(19);
  // sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor = sensor_dallascomponent->get_sensor_by_address("temp", 0x00);
  // sensor::MQTTSensorComponent *sensor_mqttsensorcomponent = App.register_sensor(sensor_dallastemperaturesensor);



  //sensors: Adc
  Application::MakeADCSensor sensor_adc_tds = App.make_adc_sensor("tds", 39, 20000);
  tds = sensor_adc_tds.adc;
  tds->set_attenuation(ADC_11db);
  tds->set_filters({});
  tds->set_unit_of_measurement("ppm");

  Application::MakeADCSensor sensor_adc_leaking = App.make_adc_sensor("adc_leaking", 36, 20000);
  adc_leaking = sensor_adc_leaking.adc;
  adc_leaking->set_attenuation(ADC_11db);
  adc_leaking->set_filters({});
  adc_leaking->set_unit_of_measurement("V");

  Application::MakeADCSensor sensor_adc_ap = App.make_adc_sensor("ap", 33, 20000);
  adc_ap = sensor_adc_ap.adc;
  adc_ap->set_attenuation(ADC_11db);
  adc_ap->set_filters({});
 
  //sensors: Pulse_counter
  Application::MakePulseCounterSensor pc_pc_a = App.make_pulse_counter_sensor("pc_a", 18, 20000);
  pc_a = pc_pc_a.pcnt;
  pc_a->set_filters({});

  Application::MakePulseCounterSensor pc_pc_b = App.make_pulse_counter_sensor("pc_b", 17, 20000);
  pc_b = pc_pc_b.pcnt;
  pc_b->set_filters({});

  Application::MakePulseCounterSensor pc_pc_c = App.make_pulse_counter_sensor("pc_c", 16, 20000);
  pc_c = pc_pc_c.pcnt;
  pc_c->set_filters({});

  //binary_sensors: gpio
  Application::MakeGPIOBinarySensor sensor_gpio_binary_sp = App.make_gpio_binary_sensor("sp", 35);
  bs_sp = sensor_gpio_binary_sp.gpio;

  Application::MakeGPIOBinarySensor sensor_gpio_binary_bp = App.make_gpio_binary_sensor("bp", 34);
  bs_bp = sensor_gpio_binary_bp.gpio;

  Application::MakeGPIOBinarySensor sensor_gpio_binary_door = App.make_gpio_binary_sensor("door", GPIOInputPin(23, INPUT_PULLUP));
  bs_door = sensor_gpio_binary_door.gpio;

  //switch.gpio
  Application::MakeGPIOSwitch switch_gpio_r1 = App.make_gpio_switch("r1", 13);
  r1 = switch_gpio_r1.switch_;

  Application::MakeGPIOSwitch switch_gpio_r2 = App.make_gpio_switch("r2", 32);
  r2 = switch_gpio_r2.switch_;

  Application::MakeGPIOSwitch switch_gpio_r3 = App.make_gpio_switch("r3", 4);
  r3 = switch_gpio_r3.switch_;

  Application::MakeGPIOSwitch switch_gpio_r4 = App.make_gpio_switch("r4", 25);
  r4 = switch_gpio_r4.switch_;

  Application::MakeGPIOSwitch switch_gpio_r5 = App.make_gpio_switch("r5", 27);
  r5 = switch_gpio_r5.switch_;

  Application::MakeGPIOSwitch switch_gpio_r6 = App.make_gpio_switch("r6", 14);
  r6 = switch_gpio_r6.switch_;

}
void setup4_template(){
  //template::binary_sensor 
  Application::MakeTemplateBinarySensor temp_binary_sensor_leaking = App.make_template_binary_sensor("leaking");
  leaking = temp_binary_sensor_leaking.template_;
  leaking->set_template([=]() -> optional<bool> {
      if (adc_leaking->state < 0.7)
        return true;
      else
        return false;
  });
}

void setup5_display(){
  //SPI and max7219 display
  SPIComponent *spicomponent = App.init_spi(5);
  spicomponent->set_mosi(2);
  display::MAX7219Component *display_max7219 = App.make_max7219(spicomponent, 15);
  display_max7219->set_num_chips(1);
  display_max7219->set_writer([=](display::MAX7219Component & it) {
      it.print("01234567");    
  });
}

void setup1_servers(){
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
  mqtt_mqttclientcomponent->set_birth_message(mqtt::MQTTMessage{
      .topic = "water/status",
      .payload = "online",
      .qos = 0,
      .retain = true,
  });
  mqtt_mqttclientcomponent->set_last_will(mqtt::MQTTMessage{
      .topic = "water/status",
      .payload = "offline",
      .qos = 0,
      .retain = true,
  });

}

void setup2_common(){
  //Status_led
  StatusLEDComponent *status_led = App.make_status_led(12);

}


void setup() {
  App.set_name("water");
  setup1_servers();
  setup2_common();
  setup3_gpioPin();
  setup4_template();
  // setup5_display();
  setup_apps_ondebug();
  App.setup();
}


void automation_main(){

}
void loop() {
  App.loop();
  automation_main();

}