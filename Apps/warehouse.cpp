// Auto generated code by esphomeyaml
#include "esphomelib/application.h"
// 0  for detection
// 1 for #1
// 2 for #2
// 3 for #3 
#define MY_SID  3 
#define UPDATE_INTERVAL 5000   //ms 


using namespace esphomelib;


void setup() {
  App.set_name("heat_pipe");
  App.set_compilation_datetime(__DATE__ ", " __TIME__);
  ::LogComponent *_logcomponent = App.init_log(115200);
  _logcomponent->set_global_log_level(ESPHOMELIB_LOG_LEVEL_DEBUG);
  ::WiFiComponent *_wificomponent = App.init_wifi("FuckGFW","refuckgfw");
  _wificomponent->set_reboot_timeout(300000);
  ::OTAComponent *_otacomponent = App.init_ota();
  _otacomponent->set_auth_password("1234567890");
  _otacomponent->start_safe_mode();
  mqtt::MQTTClientComponent *mqtt_mqttclientcomponent = App.init_mqtt("192.168.123.41", 1883, "von", "von1970");
  mqtt_mqttclientcomponent->set_reboot_timeout(300000);
   App.make_status_led(::GPIOOutputPin(16, OUTPUT, false));
  sensor::DallasComponent *sensor_dallascomponent = App.make_dallas_component(32, UPDATE_INTERVAL);
  #if (MY_SID == 0)
    //To detect sensor address
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor = sensor_dallascomponent->get_sensor_by_address("t0", 0);
  #endif


  #if (MY_SID == 1)
    // #1 Heat pipe

  #endif


  #if (MY_SID == 2) 
    //#2 Heat pipe
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor = sensor_dallascomponent->get_sensor_by_address("t0", 0x2A01186AAC05FF28);
    App.register_sensor(sensor_dallastemperaturesensor);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_2 = sensor_dallascomponent->get_sensor_by_address("t1", 0x940213122B6DAA28);
    App.register_sensor(sensor_dallastemperaturesensor_2);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_3 = sensor_dallascomponent->get_sensor_by_address("t2", 0x51021891774C6A28);
    App.register_sensor(sensor_dallastemperaturesensor_3);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_4 = sensor_dallascomponent->get_sensor_by_address("t3", 0xA901186AD770FF28);
    App.register_sensor(sensor_dallastemperaturesensor_4);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_5 = sensor_dallascomponent->get_sensor_by_address("t4", 0x49021312C346AA28);
    App.register_sensor(sensor_dallastemperaturesensor_5);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_6 = sensor_dallascomponent->get_sensor_by_address("t5", 0x87021312A0BDAA28);
    App.register_sensor(sensor_dallastemperaturesensor_6);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_7 = sensor_dallascomponent->get_sensor_by_address("t6", 0x3001186AD8BCFF28);
    App.register_sensor(sensor_dallastemperaturesensor_7);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_8 = sensor_dallascomponent->get_sensor_by_address("t7", 0x8B01186AAEF5FF28);
    App.register_sensor(sensor_dallastemperaturesensor_8);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_9 = sensor_dallascomponent->get_sensor_by_address("t8", 0x8F01186AD9D0FF28);
    App.register_sensor(sensor_dallastemperaturesensor_9);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_10 = sensor_dallascomponent->get_sensor_by_address("t9", 0x1001186AACF3FF28);
    App.register_sensor(sensor_dallastemperaturesensor_10);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_11 = sensor_dallascomponent->get_sensor_by_address("t10", 0x34021312BBB9AA28);
    App.register_sensor(sensor_dallastemperaturesensor_11);

    sensor_dallastemperaturesensor->set_filters({});
    sensor_dallastemperaturesensor_2->set_filters({});
    sensor_dallastemperaturesensor_3->set_filters({});
    sensor_dallastemperaturesensor_4->set_filters({});
    sensor_dallastemperaturesensor_5->set_filters({});
    sensor_dallastemperaturesensor_6->set_filters({});
    sensor_dallastemperaturesensor_7->set_filters({});
    sensor_dallastemperaturesensor_8->set_filters({});
    sensor_dallastemperaturesensor_9->set_filters({});
    sensor_dallastemperaturesensor_10->set_filters({});
    sensor_dallastemperaturesensor_11->set_filters({});
  #endif

  #if (MY_SID == 3) 
    //#3 Heat pipe
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor = sensor_dallascomponent->get_sensor_by_address("t12", 0x56011853A570FF28);
    App.register_sensor(sensor_dallastemperaturesensor);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_2 = sensor_dallascomponent->get_sensor_by_address("t10", 0x8F01186AA5E8FF28);
    App.register_sensor(sensor_dallastemperaturesensor_2);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_3 = sensor_dallascomponent->get_sensor_by_address("t1", 0x4201186AAC38FF28);
    App.register_sensor(sensor_dallastemperaturesensor_3);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_4 = sensor_dallascomponent->get_sensor_by_address("t9", 0xFB01186AAA9AFF28);
    App.register_sensor(sensor_dallastemperaturesensor_4);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_5 = sensor_dallascomponent->get_sensor_by_address("t3", 0xAC01186AD8BAFF28);
    App.register_sensor(sensor_dallastemperaturesensor_5);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_6 = sensor_dallascomponent->get_sensor_by_address("t2", 0xF601186AAE56FF28);
    App.register_sensor(sensor_dallastemperaturesensor_6);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_7 = sensor_dallascomponent->get_sensor_by_address("t0", 0xBB01186AA9D6FF28);
    App.register_sensor(sensor_dallastemperaturesensor_7);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_8 = sensor_dallascomponent->get_sensor_by_address("t4", 0x8E01186AA8EEFF28);
    App.register_sensor(sensor_dallastemperaturesensor_8);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_9 = sensor_dallascomponent->get_sensor_by_address("t6", 0x9B01186AD9FDFF28);
    App.register_sensor(sensor_dallastemperaturesensor_9);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_10 = sensor_dallascomponent->get_sensor_by_address("t8", 0x4501186AAD13FF28);
    App.register_sensor(sensor_dallastemperaturesensor_10);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_11 = sensor_dallascomponent->get_sensor_by_address("t7", 0x6401186AA97BFF28);
    App.register_sensor(sensor_dallastemperaturesensor_11);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_12 = sensor_dallascomponent->get_sensor_by_address("t5", 0x1201186AD7FBFF28);
    App.register_sensor(sensor_dallastemperaturesensor_12);
    sensor::DallasTemperatureSensor *sensor_dallastemperaturesensor_13 = sensor_dallascomponent->get_sensor_by_address("t11", 0x0401186AA50FFF28);
    App.register_sensor(sensor_dallastemperaturesensor_13);
    sensor_dallastemperaturesensor->set_filters({});
    sensor_dallastemperaturesensor_2->set_filters({});
    sensor_dallastemperaturesensor_3->set_filters({});
    sensor_dallastemperaturesensor_4->set_filters({});
    sensor_dallastemperaturesensor_5->set_filters({});
    sensor_dallastemperaturesensor_6->set_filters({});
    sensor_dallastemperaturesensor_7->set_filters({});
    sensor_dallastemperaturesensor_8->set_filters({});
    sensor_dallastemperaturesensor_9->set_filters({});
    sensor_dallastemperaturesensor_10->set_filters({});
    sensor_dallastemperaturesensor_11->set_filters({});
    sensor_dallastemperaturesensor_12->set_filters({});
    sensor_dallastemperaturesensor_13->set_filters({});
  #endif

  App.setup();
}

void loop() {
  App.loop();
  delay(16);
}
