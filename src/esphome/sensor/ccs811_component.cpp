#include "esphome/defines.h"

#ifdef USE_CCS811_SENSOR

#include "esphome/sensor/ccs811_component.h"
#include "esphome/mqtt/mqtt_client_component.h"
#include "esphome/log.h"

constexpr auto BASELINE_TIMEOUT_NAME = "BASELINE_TIMEOUT";

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

const char* CCS811StatusToString(CCS811Status status) {
  switch (status) {
    case CCS811Status::INITIALIZING:                  return "INITIALIZING";
    case CCS811Status::WARMING_UP:                    return "WARMING_UP";
    case CCS811Status::SEARCHING_FOR_BASELINE:        return "SEARCHING_FOR_BASELINE";
    case CCS811Status::WAITING_FOR_BASELINE_SETTING: return "WAITING_FOR_BASELINE_SETTING";
    case CCS811Status::MEASURING:                     return "MEASURING";
    default:                                          return "UNKNOWN";
  }
}

const char* CCS811StatusToHumanReadable(CCS811Status status) {
  switch (status) {
    case CCS811Status::INITIALIZING:                  return "Initializing ...";
    case CCS811Status::WARMING_UP:                    return "Warming up ...";
    case CCS811Status::SEARCHING_FOR_BASELINE:        return "Searching for baseline ...";
    case CCS811Status::WAITING_FOR_BASELINE_SETTING: return "Waiting for baseline setting ...";
    case CCS811Status::MEASURING:                     return "Measuring ...";
    default:                                          return "Unknown ...";
  }
}

void CCS811Component::setup() {
  setStatus(CCS811Status::INITIALIZING);
  auto returnCode = sensor_handle.begin();
  ESP_LOGCONFIG(CCS811Component::TAG, "Initializing of CCS811 finished with return code: %d!", returnCode);
  
  // Wait 30s to warm up
  setStatus(CCS811Status::WARMING_UP);
  set_timeout(30000, [this](){
    ESP_LOGCONFIG(CCS811Component::TAG, "Warming up finished!");
    
    // Allow subscribe for baseline
    setStatus(CCS811Status::SEARCHING_FOR_BASELINE);
    
    // Set 20s baseline searching timeout
    set_timeout(BASELINE_TIMEOUT_NAME, 20000, [this]() {
      ESP_LOGCONFIG(CCS811Component::TAG, "Baseline not found! Waiting for baseline setting!");
      setStatus(CCS811Status::WAITING_FOR_BASELINE_SETTING);
    });

    mqtt::global_mqtt_client->subscribe(baseline_mqtt_topic, [this](const std::string &topic, std::string payload){
      if (status == CCS811Status::SEARCHING_FOR_BASELINE) {
        ESP_LOGCONFIG(CCS811Component::TAG, "Baseline found! Baseline is being setting!");
        cancel_timeout(BASELINE_TIMEOUT_NAME);
        // Recv and set baseline
        uint16_t baseline = atoi(payload.c_str());
        sensor_handle.setBaseline(baseline);
        ESP_LOGCONFIG(CCS811Component::TAG, "Baseline %u set from topic %s!", baseline, topic.c_str());
        setStatus(CCS811Status::MEASURING);
      }
    });
  });
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
      ESP_LOGCONFIG(CCS811Component::TAG, "%s: %u ppm", eco2_.get_name().c_str(), eco2);
      tvoc_.publish_state(tvoc);
      ESP_LOGCONFIG(CCS811Component::TAG, "%s: %u ppb", tvoc_.get_name().c_str(), tvoc);
    }
  }
}

CCS811Component::BaselineSwitch::BaselineSwitch(const std::string& switch_name, CCS811Component* super): 
  switch_::Switch(switch_name), super(super) {}

void CCS811Component::BaselineSwitch::write_state(bool state) {
  if (state) {
    super->publishBaseline();
  }
  publish_state(false);
}

CCS811Component::CCS811Component
  (I2CComponent *parent, InitStruct init_struct, uint32_t update_interval, uint8_t address):
    PollingComponent(update_interval),
    I2CDevice(parent, address),
    sensor_handle(address),
    eco2_(init_struct.eco2_name, this),
    tvoc_(init_struct.tvoc_name, this),
    status_label(init_struct.status_name),
    baseline_switch(init_struct.switch_name, this),
    baseline_mqtt_topic(init_struct.baseline_topic)
  {
    status_label.set_icon(ICON_INFO);
    baseline_switch.set_icon(ICON_CLOUD_UPLOAD);
  }

void CCS811Component::setStatus(CCS811Status _status) {
  if (status != _status) {
    ESP_LOGCONFIG(CCS811Component::TAG, "Changing status from %s to %s!", CCS811StatusToString(status), CCS811StatusToString(_status));
    status = _status;
    status_label.publish_state(CCS811StatusToHumanReadable(status));
  }
}

void CCS811Component::publishBaseline() {
  if (status == CCS811Status::MEASURING || status == CCS811Status::WAITING_FOR_BASELINE_SETTING) {
    setStatus(CCS811Status::MEASURING);
    // Publish baseline
    uint16_t baseline = sensor_handle.getBaseline();
    char baseline_str [10] = {0};
    sprintf(baseline_str, "%u", (unsigned)baseline);
    mqtt::global_mqtt_client->publish(baseline_mqtt_topic, std::string(baseline_str), 0, true);
    ESP_LOGCONFIG(CCS811Component::TAG, "Baseline %s published to topic %s!", baseline_str, baseline_mqtt_topic.c_str());
  } else {
    ESP_LOGCONFIG(CCS811Component::TAG, "Publish baseline disabled!");
  }
}
} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_CCS811_SENSOR
