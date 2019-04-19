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
constexpr auto BASELINE_TIMEOUT_NAME = "BASELINE_TIMEOUT";

const char* CCS811StateToString(CCS811State state) {
  switch (state) {
    case CCS811State::INITIALIZING:                  return "INITIALIZING";
    case CCS811State::WARMING_UP:                    return "WARMING_UP";
    case CCS811State::SEARCHING_FOR_BASELINE:        return "SEARCHING_FOR_BASELINE";
    case CCS811State::WAINTING_FOR_BASELINE_SETTING: return "WAINTING_FOR_BASELINE_SETTING";
    case CCS811State::MEASURING:                     return "MEASURING";
    default:                                         return "UNKNOWN";
  }
}

CCS811Component::CCS811Component(I2CComponent *parent, const std::string &eco2_name,
                                 const std::string &tvoc_name, uint32_t update_interval)
  : switch_::Switch("BaselineSwitch"), PollingComponent(update_interval), I2CDevice(parent, SENSOR_ADDR),
    eco2_(new CCS811eCO2Sensor(eco2_name, this)), tvoc_(new CCS811TVOCSensor(tvoc_name, this)) {}

void CCS811Component::setup() {
  setState(CCS811State::INITIALIZING);
  auto returnCode = sensor.begin();
  ESP_LOGCONFIG(TAG, "Initializing of CCS811 finished with return code: %d!", returnCode);
  
  // Wait 30s to warm up
  setState(CCS811State::WARMING_UP);
  set_timeout(30000, [this](){
    ESP_LOGCONFIG(TAG, "Warming up finished!");
    
    // Allow subscribe for baseline
    setState(CCS811State::SEARCHING_FOR_BASELINE);
    
    // Set 20s baseline searching timeout
    set_timeout(BASELINE_TIMEOUT_NAME, 20000, [this]() {
      ESP_LOGCONFIG(TAG, "Baseline not found! Waiting for baseline setting!");
      setState(CCS811State::WAINTING_FOR_BASELINE_SETTING);
    });

    mqtt::global_mqtt_client->subscribe("baseline", [this](const std::string &topic, std::string payload){
      if (state == CCS811State::SEARCHING_FOR_BASELINE) {
        ESP_LOGCONFIG(TAG, "Baseline found! Baseline is being setting!");
        cancel_timeout(BASELINE_TIMEOUT_NAME);
        // Recv and set baseline
        uint16_t baseline = atoi(payload.c_str());
        sensor.setBaseline(baseline);
        ESP_LOGCONFIG(TAG, "Baseline %u set!", baseline);
        setState(CCS811State::MEASURING);
      }
    });
  });
}

void CCS811Component::setState(CCS811State _state) {
  if (state != _state) {
    ESP_LOGCONFIG(TAG, "Changing state from %s to %s!", CCS811StateToString(state), CCS811StateToString(_state));
    state = _state;
  }
}

void CCS811Component::write_state(bool state) {
  if (state) publishBaseline();
  publish_state(false);
}

void CCS811Component::publishBaseline() {
  if (state == CCS811State::MEASURING || state == CCS811State::WAINTING_FOR_BASELINE_SETTING) {
    setState(CCS811State::MEASURING);
    // Publish baseline
    uint16_t baseline = sensor.getBaseline();
    char baseline_str [10] = {0};
    sprintf(baseline_str, "%u", (unsigned)baseline);
    mqtt::global_mqtt_client->publish("baseline", std::string(baseline_str), 0, true);
    ESP_LOGCONFIG(TAG, "Baseline %s published!", baseline_str);
  } else {
    ESP_LOGCONFIG(TAG, "Publish baseline disabled!");
  }
}

void CCS811Component::update() {
  if (state != CCS811State::MEASURING) {
    eco2_->publish_state(-1);
    tvoc_->publish_state(-1);
  } else { 
    if (sensor.dataAvailable()) {
      sensor.readAlgorithmResults();
      const unsigned eco2 = sensor.getCO2();
      const unsigned tvoc = sensor.getTVOC();
      eco2_->publish_state(eco2);
      ESP_LOGCONFIG(TAG, "%s: %u ppm", eco2_->get_name().c_str(), eco2);
      tvoc_->publish_state(tvoc);
      ESP_LOGCONFIG(TAG, "%s: %u ppb", tvoc_->get_name().c_str(), tvoc);
    }
  }
}

void CCS811Component::dump_config() {
  //TODO
  ESP_LOGCONFIG(TAG, "CCS811:");
}

float CCS811Component::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

CCS811eCO2Sensor *CCS811Component::get_eco2_sensor() const {
  return eco2_;
}
CCS811TVOCSensor *CCS811Component::get_tvoc_sensor() const {
  return tvoc_;
}

} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_CCS811_SENSOR
