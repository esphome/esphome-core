// Auto generated code by esphomeyaml
// ========== AUTO GENERATED INCLUDE BLOCK BEGIN ===========
#include "esphomelib/application.h"
using namespace esphomelib;


switch_::GPIOSwitch* motor_pa;
switch_::GPIOSwitch* motor_pb;
sensor::PulseCounterSensorComponent* weight;
sensor::MQTTSubscribeSensor* max_weight;


void setup_apps(){
  //sensor_mqtt_subcribe create, registring?
  Application::MakeMQTTSubscribeSensor subcribe_max_weight = App.make_mqtt_subscribe_sensor("max_weight", "feeder/config/max_weight/command");
  max_weight = subcribe_max_weight.sensor;
 
  //binary_sensor::gpio   create,register,
  Application::MakeGPIOSwitch switch_gpio_motor_pa = App.make_gpio_switch("motor_pa", 5);
  motor_pa = switch_gpio_motor_pa.switch_;

  Application::MakeGPIOSwitch switch_gpio_motor_pb = App.make_gpio_switch("motor_pb", 4);
  motor_pb = switch_gpio_motor_pb.switch_;
  
  //sensor::pulse_counter create,register,set up,
  Application::MakePulseCounterSensor pc_weight = App.make_pulse_counter_sensor("weight",GPIOInputPin(14, INPUT_PULLUP));
  weight = pc_weight.pcnt;
  weight->set_filters({});
}
void setup_servers(){
  App.set_name("feeder");
  App.set_compilation_datetime(__DATE__ ", " __TIME__);
  LogComponent *logcomponent = App.init_log(115200);
  logcomponent->set_global_log_level(ESPHOMELIB_LOG_LEVEL_DEBUG);

  WiFiComponent *wificomponent = App.init_wifi();
  WiFiAP wifiap = WiFiAP();
  wifiap.set_ssid("FuckGFW");
  wifiap.set_password("refuckgfw");
  wificomponent->add_sta(wifiap);
  // OTAComponent *otacomponent = App.init_ota();
  // otacomponent->set_auth_password("1234567890");
  // otacomponent->start_safe_mode();
  mqtt::MQTTClientComponent *mqtt_mqttclientcomponent = App.init_mqtt("voicevon.vicp.io", 1883, "von", "von1970");
  // api::APIServer *api_apiserver = App.init_api_server();
  // api_apiserver->set_password("1234567890");
}

void setup() {
  setup_servers();
  setup_apps();
  App.setup(); 
}
// ========== AUTO GENERATED INCLUDE BLOCK END ==========="


void automation_1(){
  bool next  = false;

  if(weight->state < max_weight->state)
    next = true;

  if(motor_pa->state != next)
    motor_pa->toggle();
}


void loop() {
  delay(16);
  App.loop();

  automation_1();
}
