//
// Created by Otto Winter on 28.12.17.
//

#include "esphomelib/input/dallas_component.h"

#include "esphomelib/helpers.h"
#include "esphomelib/log.h"

namespace esphomelib {

namespace input {

static const char *TAG = "DallasComponent";

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
  this->dallas_.setOneWire(this->one_wire_);
  this->dallas_.begin();

  ESP_LOGI(TAG, "Device Count: %d", this->get_device_count());
  this->dallas_.setWaitForConversion(false);

  for (auto sensor : this->sensors_) {
    if (sensor->get_address() == 0) {
      bool result = this->dallas_.getAddress(sensor->get_address8(), sensor->get_index());
      assert(result && "Couldn't find sensor by index (probably because not all sensors are connected.)");
    }
    ESP_LOGD(TAG, "Device 0x%s:", sensor->get_name().c_str());
    ESP_LOGD(TAG, "    Resolution: %u", sensor->get_resolution());
    bool result = this->dallas_.setResolution(sensor->get_address8(), sensor->get_resolution(), true);
    assert(result && "Setting resolution on Dallas failed.");
    ESP_LOGD(TAG, "    Accuracy Decimals: %u", sensor->get_accuracy_decimals());
    ESP_LOGD(TAG, "    Update Interval: %u", sensor->get_update_interval());

    this->set_interval(sensor->get_name(), sensor->get_update_interval(), [this, sensor]{
      this->request_temperature(sensor);
    });
  }
}
uint8_t DallasComponent::get_device_count() {
  return this->dallas_.getDeviceCount();
}

std::vector<uint64_t> DallasComponent::scan_devices() {
  std::vector<uint64_t> v;

  run_without_interrupts<void>([&] {
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
  return setup_priority::HARDWARE;
}
DallasTemperatureSensor *DallasComponent::get_sensor_by_index(uint8_t index,
                                                              uint32_t update_interval,
                                                              uint8_t resolution) {
  auto s = this->get_sensor_by_address(0, update_interval, resolution);
  s->set_index(index);
  return s;
}
void DallasComponent::request_temperature(DallasTemperatureSensor *sensor) {
  run_without_interrupts<void>([this, sensor] {
    this->dallas_.requestTemperaturesByAddress(sensor->get_address8());
  });

  this->set_timeout(sensor->get_name(), sensor->millis_to_wait_for_conversion(), [this, sensor] {
    auto temperature = run_without_interrupts<float>([this, sensor] {
      return this->dallas_.getTempC(sensor->get_address8());
    });

    if (temperature != DEVICE_DISCONNECTED_C) {
      ESP_LOGV(TAG, "0x%s: Got Temperature=%.1f°C", sensor->get_name().c_str(), temperature);
      sensor->push_new_value(temperature, sensor->get_accuracy_decimals());
    } else {
      ESP_LOGE(TAG, "0x%s: Invalid Temperature: %f", sensor->get_name().c_str(), temperature);
    }
  });
}
OneWire *DallasComponent::get_one_wire() const {
  return this->one_wire_;
}

DallasTemperatureSensor::DallasTemperatureSensor(uint64_t address,
                                                 uint8_t resolution,
                                                 uint32_t update_interval)
    : TemperatureSensor(update_interval), accuracy_decimals_(1) {
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
uint8_t DallasTemperatureSensor::get_accuracy_decimals() const {
  return accuracy_decimals_;
}
void DallasTemperatureSensor::set_accuracy_decimals(uint8_t accuracy_decimals) {
  accuracy_decimals_ = accuracy_decimals;
}
uint8_t *DallasTemperatureSensor::get_address8() {
  return reinterpret_cast<uint8_t *>(&this->address_);
}
std::string DallasTemperatureSensor::get_name() {
  if (!this->name_.empty())
    return this->name_;

  char buffer[17];
  auto *address16 = reinterpret_cast<uint16_t *>(&this->address_);
  snprintf(buffer, sizeof(buffer), "%04x%04x%04x%04x",
           address16[3], address16[2], address16[1], address16[0]);
  return this->name_ = std::string(buffer);
}

} // namespace input

} // namespace esphomelib