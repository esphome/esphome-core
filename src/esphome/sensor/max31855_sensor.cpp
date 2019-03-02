// Implementation based on https://github.com/adafruit/MAX31855-library/blob/master/max31855.cpp and
// https://github.com/adafruit/Adafruit_CircuitPython_MAX31855/blob/master/adafruit_max31855.py

#include "esphome/defines.h"

#ifdef USE_MAX31855_SENSOR

#include "esphome/sensor/max31855_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.max31855";

void MAX31855Sensor::update() {
  this->enable();
  delay(1);
  // conversion initiated by rising edge
  this->disable();

  // Conversion time typ: 170ms, max: 220ms
  auto f = std::bind(&MAX31855Sensor::read_data_, this);
  this->set_timeout("value", 220, f);
}

void MAX31855Sensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MAX31855Sensor '%s'...", this->name_.c_str());
  this->spi_setup();
}
void MAX31855Sensor::dump_config() {
  LOG_SENSOR("", "MAX31855", this);
  LOG_PIN("  CS Pin: ", this->cs_);
  LOG_UPDATE_INTERVAL(this);
}
float MAX31855Sensor::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
std::string MAX31855Sensor::unit_of_measurement() { return UNIT_C; }
std::string MAX31855Sensor::icon() { return ICON_EMPTY; }
int8_t MAX31855Sensor::accuracy_decimals() { return 1; }
void MAX31855Sensor::read_data_() {
  this->enable();
  delay(1);
  uint8_t data[4];
  this->read_array(data, 4);

  // val is 14 bits of signed temperature data followed by 2 bits of status flags
  int16_t val = data[1] | data[0] << 8;

  // test data from MAX31855 datasheet
  // val = 0x6400 // 1600.00°C
  // val = 0x3E80 // 1000.00°C
  // val = 0x064C // 100.75°C
  // val = 0x0190 // 25.00°C
  // val = 0x0000 // 0.00°C
  // val = 0xFFFC // -0.25°C
  // val = 0xFFF0 // -1.00°C
  // val = 0xF060 // -250.00°C

  this->disable();
  if ((data[3] & 0x01) != 0) {
    ESP_LOGW(TAG, "Got thermocouple not connected from MAX31855Sensor (0x%04X) (0x%04X)", val, data[3] | data[2] << 8);
    this->status_set_warning();
    return;
  }
  if ((data[3] & 0x02) != 0) {
    ESP_LOGW(TAG, "Got short circuit to ground from MAX31855Sensor (0x%04X) (0x%04X)", val, data[3] | data[2] << 8);
    this->status_set_warning();
    return;
  }
  if ((data[3] & 0x04) != 0) {
    ESP_LOGW(TAG, "Got short circuit to power from MAX31855Sensor (0x%04X) (0x%04X)", val, data[3] | data[2] << 8);
    this->status_set_warning();
    return;
  }
  if ((data[1] & 0x01) != 0) {
    ESP_LOGW(TAG, "Got faulty reading from MAX31855Sensor (0x%04X) (0x%04X)", val, data[3] | data[2] << 8);
    this->status_set_warning();
    return;
  }

  if ((val & 0x8000) != 0) {
    // Negative value, drop the lower 2 bits and explicitly extend sign bits.
    val = 0xE000 | ((val >> 2) & 0x1FFF);
  } else {
    // Positive value, just drop the lower 2 bits.
    val >>= 2;
  }

  float temperature = float(val) / 4.0f;
  ESP_LOGD(TAG, "'%s': Got temperature=%.1f°C", this->name_.c_str(), temperature);
  this->publish_state(temperature);
  this->status_clear_warning();
}

MAX31855Sensor::MAX31855Sensor(const std::string &name, SPIComponent *parent, GPIOPin *cs, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval), SPIDevice(parent, cs) {}

bool MAX31855Sensor::is_device_msb_first() { return true; }

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MAX31855_SENSOR
