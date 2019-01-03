#include <esphomelib.h>

using namespace esphomelib;

void setup() {
  App.set_name("tx20-sample");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  App.init_ota()->start_safe_mode();

  //auto *template = App.make_template_text_sensor("Wind Direction").template_;
  auto *tx20 = App.make_tx20_sensor("Windspeed", "Winddirection", ::GPIOInputPin(4, INPUT)).tx20;

  // template->set_icon("mdi:direction");
  // template
  //     ->set_template([=]() -> ::optional<std::string> { return tx20.get_wind_cardinal_direction(); });
}

void loop() {
  App.loop();
}
