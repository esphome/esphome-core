#include "esphome/defines.h"

#ifdef USE_CCS811_SENSOR

#include "esphome/sensor/ccs811_component.h"
#include "esphome/mqtt/mqtt_client_component.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

constexpr auto TAG = "sensor.ccs811";
constexpr auto BASELINE_TIMEOUT_NAME = "BASELINE_TIMEOUT";
constexpr auto ECO2_NAME = " eCO2";
constexpr auto TVOC_NAME = " TVOC";
constexpr auto BASELINE_SWITCH_NAME = " Publish baseline";

const char* CCS811StatusToString(CCS811Status status) {
  switch (status) {
    case CCS811Status::INITIALIZING:                  return "INITIALIZING";
    case CCS811Status::WARMING_UP:                    return "WARMING_UP";
    case CCS811Status::SEARCHING_FOR_BASELINE:        return "SEARCHING_FOR_BASELINE";
    case CCS811Status::WAINTING_FOR_BASELINE_SETTING: return "WAINTING_FOR_BASELINE_SETTING";
    case CCS811Status::MEASURING:                     return "MEASURING";
    default:                                          return "UNKNOWN";
  }
}

const char* CCS811StatusToHumanReadable(CCS811Status status) {
  switch (status) {
    case CCS811Status::INITIALIZING:                  return "Initializing ...";
    case CCS811Status::WARMING_UP:                    return "Warming up ...";
    case CCS811Status::SEARCHING_FOR_BASELINE:        return "Searching for baseline ...";
    case CCS811Status::WAINTING_FOR_BASELINE_SETTING: return "Waiting for baseline setting ...";
    case CCS811Status::MEASURING:                     return "Measuring ...";
    default:                                          return "Unknown ...";
  }
}

CCS811Component::CCS811Component
  (I2CComponent *parent, InitStruct init_struct, uint32_t update_interval, uint8_t address):
    switch_::Switch(init_struct.switch_name),
    PollingComponent(update_interval),
    I2CDevice(parent, SENSOR_ADDR),
    sensor_handle(address),
    eco2_(init_struct.eco2_name, this),
    tvoc_(init_struct.tvoc_name, this),
    status_label(init_struct.status_name),
    baseline_mqtt_topic(init_struct.baseline_topic)
  {
    status_label.set_icon(ICON_INFO);
    this->set_icon(ICON_CLOUD_UPLOAD);
  }

void CCS811Component::setup() {
  setStatus(CCS811Status::INITIALIZING);
  auto returnCode = sensor_handle.begin();
  ESP_LOGCONFIG(TAG, "Initializing of CCS811 finished with return code: %d!", returnCode);
  
  // Wait 30s to warm up
  setStatus(CCS811Status::WARMING_UP);
  set_timeout(30000, [this](){
    ESP_LOGCONFIG(TAG, "Warming up finished!");
    
    // Allow subscribe for baseline
    setStatus(CCS811Status::SEARCHING_FOR_BASELINE);
    
    // Set 20s baseline searching timeout
    set_timeout(BASELINE_TIMEOUT_NAME, 20000, [this]() {
      ESP_LOGCONFIG(TAG, "Baseline not found! Waiting for baseline setting!");
      setStatus(CCS811Status::WAINTING_FOR_BASELINE_SETTING);
    });

    mqtt::global_mqtt_client->subscribe(baseline_mqtt_topic, [this](const std::string &topic, std::string payload){
      if (status == CCS811Status::SEARCHING_FOR_BASELINE) {
        ESP_LOGCONFIG(TAG, "Baseline found! Baseline is being setting!");
        cancel_timeout(BASELINE_TIMEOUT_NAME);
        // Recv and set baseline
        uint16_t baseline = atoi(payload.c_str());
        sensor_handle.setBaseline(baseline);
        ESP_LOGCONFIG(TAG, "Baseline %u set from topic %s!", baseline, topic.c_str());
        setStatus(CCS811Status::MEASURING);
      }
    });
  });
}

void CCS811Component::setStatus(CCS811Status _status) {
  if (status != _status) {
    ESP_LOGCONFIG(TAG, "Changing status from %s to %s!", CCS811StatusToString(status), CCS811StatusToString(_status));
    status = _status;
    status_label.publish_state(CCS811StatusToHumanReadable(status));
  }
}

void CCS811Component::write_state(bool state) {
  if (state) {
    publishBaseline();
  }
  publish_state(false);
}

void CCS811Component::publishBaseline() {
  if (status == CCS811Status::MEASURING || status == CCS811Status::WAINTING_FOR_BASELINE_SETTING) {
    setStatus(CCS811Status::MEASURING);
    // Publish baseline
    uint16_t baseline = sensor_handle.getBaseline();
    char baseline_str [10] = {0};
    sprintf(baseline_str, "%u", (unsigned)baseline);
    mqtt::global_mqtt_client->publish(baseline_mqtt_topic, std::string(baseline_str), 0, true);
    ESP_LOGCONFIG(TAG, "Baseline %s published to topic %s!", baseline_str, baseline_mqtt_topic.c_str());
  } else {
    ESP_LOGCONFIG(TAG, "Publish baseline disabled!");
  }
}

void CCS811Component::update() {
  if (status != CCS811Status::MEASURING) {
    eco2_.publish_state(-1);
    tvoc_.publish_state(-1);
  } else { 
    if (sensor_handle.dataAvailable()) {
      sensor_handle.readAlgorithmResults();
      const unsigned eco2 = sensor_handle.getCO2();
      const unsigned tvoc = sensor_handle.getTVOC();
      eco2_.publish_state(eco2);
      ESP_LOGCONFIG(TAG, "%s: %u ppm", eco2_.get_name().c_str(), eco2);
      tvoc_.publish_state(tvoc);
      ESP_LOGCONFIG(TAG, "%s: %u ppb", tvoc_.get_name().c_str(), tvoc);
    }
  }
}

void CCS811Component::dump_config() {
  //TODO
  ESP_LOGCONFIG(TAG, "CONFIG TODO");
}

float CCS811Component::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

CCS811eCO2Sensor *CCS811Component::get_eco2_sensor() {
  return &eco2_;
}
CCS811TVOCSensor *CCS811Component::get_tvoc_sensor() {
  return &tvoc_;
}
text_sensor::TextSensor *CCS811Component::get_status_label() {
  return &status_label;
}

} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_CCS811_SENSOR
