#include "esphome/defines.h"

#ifdef USE_CCS811_SENSOR

#include "esphome/sensor/ccs811_component.h"
#include "esphome/mqtt/mqtt_client_component.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.ccs811";
constexpr uint8_t MEAS_MODE = 0x01;
constexpr uint8_t ALG_RESULT_DAT = 0x02;
constexpr uint8_t DRIVE_MODE = 0x01 << 4;

CCS811Component::CCS811Component(I2CComponent *parent, const std::string &eco2_name,
                                 const std::string &tvoc_name, uint32_t update_interval)
  : switch_::Switch("BaselineSwitch"), PollingComponent(update_interval), I2CDevice(parent, SENSOR_ADDR),
    eco2_(new CCS811eCO2Sensor(eco2_name, this)), tvoc_(new CCS811TVOCSensor(tvoc_name, this)) {}

void CCS811Component::setup() {
  auto returnCode = this->sensor.begin();
  ESP_LOGCONFIG(TAG, "Setting up CCS811... Return code: %d", returnCode);
}

void CCS811Component::write_state(bool state) {
  ESP_LOGCONFIG(TAG, "State changed");
  publish_state(state);
}

void CCS811Component::update() {
  if (this->sensor.dataAvailable()) {
    this->sensor.readAlgorithmResults();
    const unsigned eco2 = this->sensor.getCO2();
    const unsigned tvoc = this->sensor.getTVOC();
    this->eco2_->publish_state(eco2);
    ESP_LOGCONFIG(TAG, "%s: %u ppm", this->eco2_->get_name().c_str(), eco2);
    this->tvoc_->publish_state(tvoc);
    ESP_LOGCONFIG(TAG, "%s: %u ppb", this->tvoc_->get_name().c_str(), tvoc);
  }
}

void CCS811Component::publish_baseline() {
  uint16_t baseline = this->sensor.getBaseline();
  char baseline_str [6];
  sprintf(baseline_str, "%u", baseline);
  mqtt::global_mqtt_client->publish("topic", baseline_str);
  ESP_LOGCONFIG(TAG, "Baseline %s published", baseline_str);
}

void CCS811Component::dump_config() {
  //TODO
  ESP_LOGCONFIG(TAG, "CCS811:");
}

float CCS811Component::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

CCS811eCO2Sensor *CCS811Component::get_eco2_sensor() const {
  return this->eco2_;
}
CCS811TVOCSensor *CCS811Component::get_tvoc_sensor() const {
  return this->tvoc_;
}

} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_CCS811_SENSOR
