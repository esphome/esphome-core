// Auto generated code by esphomeyaml
// ========== AUTO GENERATED INCLUDE BLOCK BEGIN ===========
#include "esphomelib/application.h"
using namespace esphomelib;
// ========== AUTO GENERATED INCLUDE BLOCK END ==========="

void setup() {
  // ===== DO NOT EDIT ANYTHING BELOW THIS LINE =====
  // ========== AUTO GENERATED CODE BEGIN ===========
  App.set_name("feeder");
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
  mqtt::MQTTClientComponent *mqtt_mqttclientcomponent = App.init_mqtt("voicevon.vicp.io", 1883, "von", "von1970");
  mqtt::MQTTMessageTrigger *mqtt_mqttmessagetrigger = mqtt_mqttclientcomponent->make_message_trigger("abcd/efgh");
  api::APIServer *api_apiserver = App.init_api_server();
  api_apiserver->set_password("1234567890");
  Automation<std::string> *automation = App.make_automation<std::string>(mqtt_mqttmessagetrigger);
  Application::MakeGPIOSwitch application_makegpioswitch = App.make_gpio_switch("motor_p1", 5);
  switch_::GPIOSwitch *motor_p1 = application_makegpioswitch.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent = application_makegpioswitch.mqtt;
  motor_p1->set_internal(true);
  Application::MakeGPIOSwitch application_makegpioswitch_2 = App.make_gpio_switch("motor_p2", 4);
  switch_::GPIOSwitch *switch__gpioswitch = application_makegpioswitch_2.switch_;
  switch_::MQTTSwitchComponent *switch__mqttswitchcomponent_2 = application_makegpioswitch_2.mqtt;
  switch_::TurnOnAction<std::string> *action = motor_p1->make_turn_on_action<std::string>();
  automation->add_actions({action});
  // =========== AUTO GENERATED CODE END ============
  // ========= YOU CAN EDIT AFTER THIS LINE =========
  App.setup();
  
  mqtt_mqttmessagetrigger->setup();
}

void loop() {
  App.loop();
}
