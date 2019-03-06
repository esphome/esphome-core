// Implementation based on https://github.com/adafruit/MAX6675-library/blob/master/max6675.cpp

#include "esphome/defines.h"

#ifdef USE_MAX6675_SENSOR

#include "esphome/sensor/max6675_sensor.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.max6675";

void MAX6675Sensor::update() {
  this->enable();
  delay(1);
  // conversion initiated by rising edge
  this->disable();

  // Conversion time typ: 170ms, max: 220ms
  auto f = std::bind(&MAX6675Sensor::read_data_, this);
  this->set_timeout("value", 220, f);
}

void MAX6675Sensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MAX6675Sensor '%s'...", this->name_.c_str());
  this->spi_setup();
}
void MAX6675Sensor::dump_config() {
  LOG_SENSOR("", "MAX6675", this);
  LOG_PIN("  CS Pin: ", this->cs_);
  LOG_UPDATE_INTERVAL(this);
}
float MAX6675Sensor::get_setup_priority() const { return setup_priority::HARDWARE_LATE; }
std::string MAX6675Sensor::unit_of_measurement() { return UNIT_C; }
std::string MAX6675Sensor::icon() { return ICON_EMPTY; }
int8_t MAX6675Sensor::accuracy_decimals() { return 1; }
void MAX6675Sensor::read_data_() {
  this->enable();
  delay(1);
  uint8_t data[2];
  this->read_array(data, 2);
  uint16_t val = data[1] | (uint16_t(data[0]) << 8);
  this->disable();

  if ((val & 0x04) != 0) {
    // Thermocouple open
    ESP_LOGW(TAG, "Got invalid value from MAX6675Sensor (0x%04X)", val);
    this->status_set_warning();
    return;
  }

  float temperature = float(val >> 3) / 4.0f;
  ESP_LOGD(TAG, "'%s': Got temperature=%.1f°C", this->name_.c_str(), temperature);
  this->publish_state(temperature);
  this->status_clear_warning();
}

MAX6675Sensor::MAX6675Sensor(const std::string &name, SPIComponent *parent, GPIOPin *cs, uint32_t update_interval)
    : PollingSensorComponent(name, update_interval), SPIDevice(parent, cs) {}

bool MAX6675Sensor::is_device_msb_first() { return true; }

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_MAX6675_SENSOR
