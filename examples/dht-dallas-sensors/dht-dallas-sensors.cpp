#include <esphomelib.h>

using namespace esphomelib;

void setup() {
  App.set_name("outside");
  App.init_log();

  auto *wifi = App.init_wifi();
  // *all* parameters have to be declared in *exact order*, as defined in declared structures.
  // otherwise, compiler will complain with "sorry, unimplemented: non-trivial designated initializers not supported"
  auto ap = WiFiAP();
  ap.set_ssid("MySSID");
  ap.set_password("MyPassword");
  ap.set_manual_ip(ManualIP{
      .static_ip = IPAddress(192, 168, 178, 42),
      .gateway = IPAddress(192, 168, 178, 1),
      .subnet = IPAddress(255, 255, 255, 0)
  });
  wifi->add_sta(ap);
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");
  auto *ota = App.init_ota();
  ota->set_auth_plaintext_password("PASSWORD"); // set an optional password
  ota->set_port(3232); // This is the default for ESP32
  ota->start_safe_mode();

  auto *dallas = App.make_dallas_component(15);

  App.register_sensor(dallas->get_sensor_by_address("Ambient Temperature", 0xfe0000031f1eaf29));
  App.register_sensor(dallas->get_sensor_by_address("Heatpump Temperature", 0x710000031f0e7e28));

  auto dht = App.make_dht_sensor("Outside Temperature", "Outside Humidity", 12);
  dht.dht->get_temperature_sensor()->set_filters({
      // Take average of 30 last values; report average on every 20th value
      new sensor::SlidingWindowMovingAverageFilter(30, 20),
      // Convert to Fahrenheit
      new sensor::LambdaFilter([](float celsius) -> optional<float> {
        return celsius * 9.0/5.0 + 32.0;
      }),
  });
  dht.dht->get_temperature_sensor()->set_unit_of_measurement("°F");

  // Built-in Analog to Digital converter, can be used to measure things like luminosity.
  App.make_adc_sensor("Analog Voltage", 13);

  // Ultrasonic sensor like HC-SR04, can be used to measure distance.
  App.make_ultrasonic_sensor("Ultrasonic Sensor", 23, 22);

  App.setup();
}

void loop() {
  App.loop();
}
