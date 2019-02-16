// <WARNING>
//   For this example to compile, you'll first need to install the Adafruit BMP085 library
//   See  for more information
// </WARNING>

#include "esphome.h"
#include <Adafruit_BMP085.h>

using namespace esphome;

// SIMPLE EXAMPLE: just pressure
class BMP180Sensor : public sensor::PollingSensorComponent {
 public:
  Adafruit_BMP085 bmp;

  BMP180Sensor(const std::string &name, uint32_t update_interval)
      : sensor::PollingSensorComponent(name, update_interval) {}

  void setup() override {
    bmp.begin();
  }

  void update() override {
    int pressure = bmp.readPressure(); // in Pa, or 1/100 hPa
    publish_state(pressure / 100.0); // convert to hPa
  }

  std::string unit_of_measurement() override { return "hPa"; }
  int8_t accuracy_decimals() override { return 2; } // 2 decimal places of accuracy.
};



// ADVANCED EXAMPLE: temperature + pressure
// An empty sensor subclass that will "proxy" the temperature values
class BMP280TemperatureSensor : public sensor::Sensor {
 public:
  BMP280TemperatureSensor(const std::string &name) : sensor::Sensor(name) {}
  std::string unit_of_measurement() override { return "°C"; }
  int8_t accuracy_decimals() override { return 1; }
};

// An empty sensor subclass that will "proxy" the pressure values
class BMP280PressureSensor : public sensor::Sensor {
 public:
  BMP280PressureSensor(const std::string &name) : sensor::Sensor(name) {}
  std::string unit_of_measurement() override { return "hPa"; }
  int8_t accuracy_decimals() override { return 2; }
};

class BMP180Component : public PollingComponent {
 public:
  Adafruit_BMP085 bmp;
  BMP280TemperatureSensor *temperature_sensor;
  BMP280PressureSensor *pressure_sensor;

  BMP180Component(const std::string &temperature_name, const std::string &pressure_name, uint32_t update_interval)
      : PollingComponent(update_interval) {
    this->temperature_sensor = new BMP280TemperatureSensor(temperature_name);
    this->pressure_sensor = new BMP280PressureSensor(pressure_name);
  }

  void setup() override {
    bmp.begin();
  }

  void update() override {
    // This is the actual sensor reading logic.
    int pressure = bmp.readPressure();
    pressure_sensor->publish_state(pressure / 100.0);

    float temperature = bmp.readTemperature();
    temperature_sensor->publish_state(temperature);
  }
};

void setup() {
  App.set_name("test");
  App.init_log();

  App.init_wifi("YOUR_SSID", "YOUR_PASSWORD");
  App.init_ota()->start_safe_mode();
  App.init_mqtt("MQTT_HOST", "USERNAME", "PASSWORD");


  // simple example:
  auto *custom_sensor = new BMP180Sensor("My BMP180 sensor", 5000);
  App.register_component(custom_sensor);
  App.register_sensor(custom_sensor);



  // advanced example:
  auto *custom_bmp180 = new BMP180Component("BMP180 Temperature", "BMP180 Pressure", 5000);
  // The BMP180Component is a *component*, so it needs to be registered.
  App.register_component(custom_bmp180);

  // But the temperature&pressure classes are *sensors*, so each of them needs to be registered
  App.register_sensor(custom_bmp180->temperature_sensor);
  App.register_sensor(custom_bmp180->pressure_sensor);

  App.setup();
}

void loop() {
  App.loop();
}
