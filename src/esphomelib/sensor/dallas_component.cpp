//
// Created by Otto Winter on 28.12.17.
//

#include "esphomelib/sensor/dallas_component.h"

#include "esphomelib/helpers.h"
#include "esphomelib/log.h"

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor::dallas";

std::string uint64_to_string(uint64_t num) {
  char buffer[17];
  auto *address16 = reinterpret_cast<uint16_t *>(&num);
  snprintf(buffer, sizeof(buffer), "%04x%04x%04x%04x",
           address16[3], address16[2], address16[1], address16[0]);
  return std::string(buffer);
}

uint16_t DallasTemperatureSensor::millis_to_wait_for_conversion() const {
  switch (this->resolution_) {
    case 9:return 94;
    case 10:return 188;
    case 11:return 375;
    default:return 750;
  }
}

DallasTemperature &DallasComponent::get_dallas() {
  return this->dallas_;
}
DallasComponent::DallasComponent(OneWire *one_wire) {
  this->set_one_wire(one_wire);
}
void DallasComponent::set_one_wire(OneWire *one_wire) {
  this->one_wire_ = one_wire;
}
void DallasComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up DallasComponent...");
  ESP_LOGCONFIG(TAG, "    Want device count: %u", this->sensors_.size());

  this->dallas_.setOneWire(this->one_wire_);
  this->dallas_.setWaitForConversion(false);

  delayMicroseconds(10); // improves reliability
  std::vector<uint64_t> sensors = this->scan_devices();
  ESP_LOGD(TAG, "Found Dallas Sensors: %u", sensors.size());
  for (uint64_t i : sensors)
    ESP_LOGD(TAG, "    0x%s", uint64_to_string(i).c_str());
  delayMicroseconds(10); // improves reliability

  for (auto sensor : this->sensors_) {
    if (sensor->get_address() == 0) {
      ESP_LOGCONFIG(TAG, "Device with index %u", sensor->get_index());
      if (sensor->get_index() >= sensors.size()) {
        ESP_LOGE(TAG, "Couldn't find sensor by index - not connected. Proceeding without it.");
        continue;
      }
      sensor->set_address(sensors[sensor->get_index()]);
      ESP_LOGCONFIG(TAG, "     -> Address: %s", sensor->get_name().c_str());
    } else {
      ESP_LOGCONFIG(TAG, "Device %s:", sensor->get_name().c_str());
    }
    ESP_LOGCONFIG(TAG, "    Resolution: %u", sensor->get_resolution());

    bool result = this->dallas_.setResolution(sensor->get_address8(), sensor->get_resolution(), true);
    if (!result) {
      ESP_LOGE(TAG, "Setting resolution on Dallas failed. Proceeding without it.");
      continue;
    }
    ESP_LOGCONFIG(TAG, "    Update Interval: %u", sensor->get_update_interval());

    this->set_interval(sensor->get_name(), sensor->get_update_interval(), [this, sensor]{
      this->request_temperature(sensor);
    });
  }
}

std::vector<uint64_t> DallasComponent::scan_devices() {
  std::vector<uint64_t> v;

  run_without_interrupts([&] {
    uint64_t address = 0;
    auto *address8 = reinterpret_cast<uint8_t *>(&address);
    OneWire *ow = this->one_wire_;
    ow->reset_search();
    while (ow->search(address8))
      v.push_back(address);
  });

  return v;
}
DallasTemperatureSensor *DallasComponent::get_sensor_by_address(uint64_t address,
                                                                uint32_t update_interval,
                                                                uint8_t resolution) {
  auto s = new DallasTemperatureSensor(address, resolution, update_interval);
  this->sensors_.push_back(s);
  return s;
}
float DallasComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
DallasTemperatureSensor *DallasComponent::get_sensor_by_index(uint8_t index,
                                                              uint32_t update_interval,
                                                              uint8_t resolution) {
  auto s = this->get_sensor_by_address(0, update_interval, resolution);
  s->set_index(index);
  return s;
}
void DallasComponent::request_temperature(DallasTemperatureSensor *sensor) {
  run_without_interrupts([this, sensor] {
    this->dallas_.requestTemperaturesByAddress(sensor->get_address8());
    delayMicroseconds(10); // seems to be a lot more stable
  });

  this->set_timeout(sensor->get_name(), sensor->millis_to_wait_for_conversion(), [this, sensor] {
    delayMicroseconds(10); // seems to be a lot more stable
    auto temperature = run_without_interrupts<float>([this, sensor] {
      return this->dallas_.getTempC(sensor->get_address8());
    });

    if (temperature != DEVICE_DISCONNECTED_C) {
      ESP_LOGD(TAG, "%s: Got Temperature=%.1f°C", sensor->get_name().c_str(), temperature);
      sensor->push_new_value(temperature);
    } else {
      ESP_LOGW(TAG, "%s: Invalid Temperature: %f", sensor->get_name().c_str(), temperature);
    }
  });
}
OneWire *DallasComponent::get_one_wire() const {
  return this->one_wire_;
}

DallasTemperatureSensor::DallasTemperatureSensor(uint64_t address,
                                                 uint8_t resolution,
                                                 uint32_t update_interval)
    : sensor::Sensor(), update_interval_(update_interval) {
  this->set_address(address);
  this->set_resolution(resolution);
}
uint64_t DallasTemperatureSensor::get_address() const {
  return this->address_;
}
void DallasTemperatureSensor::set_address(uint64_t address) {
  this->address_ = address;
}
uint8_t DallasTemperatureSensor::get_resolution() const {
  return this->resolution_;
}
void DallasTemperatureSensor::set_resolution(uint8_t resolution) {
  assert(9 <= resolution && resolution <= 12);
  this->resolution_ = resolution;
}
uint8_t DallasTemperatureSensor::get_index() const {
  return this->index_;
}
void DallasTemperatureSensor::set_index(uint8_t index) {
  this->index_ = index;
}
uint8_t *DallasTemperatureSensor::get_address8() {
  return reinterpret_cast<uint8_t *>(&this->address_);
}
std::string DallasTemperatureSensor::get_name() {
  if (!this->name_.empty())
    return this->name_;

  return this->name_ = std::string("0x") + uint64_to_string(this->address_);
}
std::string DallasTemperatureSensor::unit_of_measurement() {
  return sensor::UNIT_OF_MEASUREMENT_CELSIUS;
}
std::string DallasTemperatureSensor::icon() {
  return sensor::ICON_TEMPERATURE;
}
uint32_t DallasTemperatureSensor::update_interval() {
  return this->get_update_interval();
}
uint32_t DallasTemperatureSensor::get_update_interval() const {
  return this->update_interval_;
}
void DallasTemperatureSensor::set_update_interval(uint32_t update_interval) {
  this->update_interval_ = update_interval;
}
int8_t DallasTemperatureSensor::accuracy_decimals() {
  return 1;
}

} // namespace sensor

} // namespace esphomelib