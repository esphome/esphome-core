#include "esphome/defines.h"

#ifdef USE_CCS811_SENSOR

#include "esphome/sensor/ccs811_component.h"
#include "esphome/mqtt/mqtt_client_component.h"
#include "esphome/log.h"

constexpr auto BASELINE_TIMEOUT_NAME = "BASELINE_TIMEOUT";

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

const char* ccs811_status_to_string(CCS811Status status) {
  switch (status) {
    case CCS811Status::INITIALIZING:                  return "INITIALIZING";
    case CCS811Status::WARMING_UP:                    return "WARMING_UP";
    case CCS811Status::SEARCHING_FOR_BASELINE:        return "SEARCHING_FOR_BASELINE";
    case CCS811Status::WAITING_FOR_BASELINE_SETTING: return "WAITING_FOR_BASELINE_SETTING";
    case CCS811Status::MEASURING:                     return "MEASURING";
    default:                                          return "UNKNOWN";
  }
}

const char* ccs811_status_to_human_readable(CCS811Status status) {
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
  this->set_status_(CCS811Status::INITIALIZING);
  auto return_code = this->sensor_handle_.begin();
  ESP_LOGI(CCS811Component::TAG, "Initializing of CCS811 finished with return code: %d!", return_code);

  // Wait 30s to warm up
  this->set_status_(CCS811Status::WARMING_UP);
  set_timeout(30000, [this](){
    ESP_LOGD(CCS811Component::TAG, "Warming up finished!");

    // Allow subscribe for baseline
    this->set_status_(CCS811Status::SEARCHING_FOR_BASELINE);

    // Set 20s baseline searching timeout
    set_timeout(BASELINE_TIMEOUT_NAME, 20000, [this]() {
      ESP_LOGD(CCS811Component::TAG, "Baseline not found! Waiting for baseline setting!");
      this->set_status_(CCS811Status::WAITING_FOR_BASELINE_SETTING);
    });

    mqtt::global_mqtt_client->subscribe(this->baseline_mqtt_topic_, [this](const std::string &topic, std::string payload){
      if (this->status_ == CCS811Status::SEARCHING_FOR_BASELINE) {
        ESP_LOGD(CCS811Component::TAG, "Baseline found! Baseline is being setting!");
        cancel_timeout(BASELINE_TIMEOUT_NAME);
        // Recv and set baseline
        uint16_t baseline = atoi(payload.c_str());
        this->sensor_handle_.setBaseline(baseline);
        ESP_LOGD(CCS811Component::TAG, "Baseline %u set from topic %s!", baseline, topic.c_str());
        this->set_status_(CCS811Status::MEASURING);
      }
    });
  });
}

void CCS811Component::dump_config() {
  ESP_LOGCONFIG(CCS811Component::TAG, "CCS811:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with CCS811 failed!");
  }
  ESP_LOGCONFIG(CCS811Component::TAG, "%s: %u ppm", this->eco2_.get_name().c_str(), this->sensor_handle_.getCO2());
  ESP_LOGCONFIG(CCS811Component::TAG, "%s: %u ppb", this->tvoc_.get_name().c_str(), this->sensor_handle_.getTVOC());
}

void CCS811Component::update() {
  if (this->status_ != CCS811Status::MEASURING) {
    this->eco2_.publish_state(-1);
    this->tvoc_.publish_state(-1);
  } else {
    if (this->sensor_handle_.dataAvailable()) {
      this->sensor_handle_.readAlgorithmResults();
      this->eco2_.publish_state(this->sensor_handle_.getCO2());
      this->tvoc_.publish_state(this->sensor_handle_.getTVOC());
    }
  }
}

CCS811Component::BaselineSwitch::BaselineSwitch(const std::string& switch_name, CCS811Component* super):
  switch_::Switch(switch_name), super(super) {}

void CCS811Component::BaselineSwitch::write_state(bool state) {
  if (state) {
    super->publish_baseline_();
  }
  publish_state(false);
}

CCS811Component::CCS811Component
  (I2CComponent *parent, init_struct init_struct, uint32_t update_interval, uint8_t address):
    PollingComponent(update_interval),
    I2CDevice(parent, address),
    sensor_handle_(address),
    eco2_(init_struct.eco2_name, this),
    tvoc_(init_struct.tvoc_name, this),
    status_label_(init_struct.status_name),
    baseline_switch_(init_struct.switch_name, this),
    baseline_mqtt_topic_(init_struct.baseline_topic)
  {
    this->status_label_.set_icon(ICON_INFO);
    this->baseline_switch_.set_icon(ICON_CLOUD_UPLOAD);
  }

void CCS811Component::set_status_(CCS811Status status) {
  if (this->status_ != status) {
    ESP_LOGCONFIG(CCS811Component::TAG, "Changing status from %s to %s!", ccs811_status_to_string(this->status_), ccs811_status_to_string(status));
    this->status_ = status;
    this->status_label_.publish_state(ccs811_status_to_human_readable(this->status_));
  }
}

void CCS811Component::publish_baseline_() {
  if (this->status_ == CCS811Status::MEASURING || this->status_ == CCS811Status::WAITING_FOR_BASELINE_SETTING) {
    this->set_status_(CCS811Status::MEASURING);
    // Publish baseline
    uint16_t baseline = this->sensor_handle_.getBaseline();
    char baseline_str [10] = {0};
    sprintf(baseline_str, "%u", (unsigned)baseline);
    mqtt::global_mqtt_client->publish(this->baseline_mqtt_topic_, std::string(baseline_str), 0, true);
    ESP_LOGCONFIG(CCS811Component::TAG, "Baseline %s published to topic %s!", baseline_str, this->baseline_mqtt_topic_.c_str());
  } else {
    ESP_LOGCONFIG(CCS811Component::TAG, "Publish baseline disabled!");
  }
}
} // namespace sensor

ESPHOME_NAMESPACE_END

#endif //USE_CCS811_SENSOR
