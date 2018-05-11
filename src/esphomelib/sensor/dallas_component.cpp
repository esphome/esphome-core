//
// Created by Otto Winter on 28.12.17.
//
// Based on:
//   - https://github.com/milesburton/Arduino-Temperature-Control-Library

#include "esphomelib/sensor/dallas_component.h"

#include "esphomelib/helpers.h"
#include "esphomelib/log.h"

#ifdef USE_DALLAS_SENSOR

namespace esphomelib {

namespace sensor {

static const char *TAG = "sensor.dallas";

static const uint8_t DALLAS_MODEL_DS18S20 = 0x10;
static const uint8_t DALLAS_MODEL_DS1822 = 0x22;
static const uint8_t DALLAS_MODEL_DS18B20 = 0x28;
static const uint8_t DALLAS_MODEL_DS1825 = 0x3B;
static const uint8_t DALLAS_MODEL_DS28EA00 = 0x42;
static const uint8_t DALLAS_COMMAND_START_CONVERSION = 0x44;
static const uint8_t DALLAS_COMMAND_READ_SCRATCH_PAD = 0xBE;
static const uint8_t DALLAS_COMMAND_WRITE_SCRATCH_PAD = 0x4E;

uint16_t DallasTemperatureSensor::millis_to_wait_for_conversion_() const {
  switch (this->resolution_) {
    case 9:return 94;
    case 10:return 188;
    case 11:return 375;
    default:return 750;
  }
}

void DallasComponent::set_one_wire(ESPOneWire *one_wire) {
  this->one_wire_ = one_wire;
}
void DallasComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up DallasComponent...");
  ESP_LOGCONFIG(TAG, "    Want device count: %u", this->sensors_.size());

  yield();
  disable_interrupts();
  std::vector<uint64_t> raw_sensors = this->one_wire_->search_vec();
  enable_interrupts();

  ESP_LOGD(TAG, "Found sensors:");
  std::vector<uint64_t> out;
  for (auto &address : raw_sensors) {
    std::string s = uint64_to_string(address);
    auto *address8 = reinterpret_cast<uint8_t *>(&address);
    if (crc8(address8, 7) != address8[7]) {
      ESP_LOGW(TAG, "Dallas device 0x%s has invalid CRC.", s.c_str());
      continue;
    }
    if (address8[0] != DALLAS_MODEL_DS18S20 && address8[0] != DALLAS_MODEL_DS1822 &&
        address8[0] != DALLAS_MODEL_DS18B20 && address8[0] != DALLAS_MODEL_DS1825 &&
        address8[0] != DALLAS_MODEL_DS28EA00) {
      ESP_LOGW(TAG, "Unknown device type %02X.", address8[0]);
      continue;
    }
    ESP_LOGD(TAG, "    0x%s", s.c_str());
    out.push_back(address);
  }
  for (auto sensor : this->sensors_) {
    ESP_LOGCONFIG(TAG, "Device '%s':", sensor->get_name().c_str());
    if (sensor->get_address() == 0) {
      ESP_LOGCONFIG(TAG, "    Index %u", sensor->get_index());
      if (sensor->get_index() >= out.size()) {
        ESP_LOGE(TAG, "Couldn't find sensor by index - not connected. Proceeding without it.");
        continue;
      }
      sensor->set_address(out[sensor->get_index()]);
      ESP_LOGCONFIG(TAG, "     -> Address: %s", sensor->get_address_name().c_str());
    } else {
      ESP_LOGCONFIG(TAG, "     -> Address: %s", sensor->get_address_name().c_str());
    }
    ESP_LOGCONFIG(TAG, "    Resolution: %u", sensor->get_resolution());

    sensor->setup_sensor_();
  }
}

DallasTemperatureSensor *DallasComponent::get_sensor_by_address(const std::string &name,
                                                                uint64_t address, uint8_t resolution) {
  auto s = new DallasTemperatureSensor(name, address, resolution, this);
  this->sensors_.push_back(s);
  return s;
}
float DallasComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
DallasTemperatureSensor *DallasComponent::get_sensor_by_index(const std::string &name,
                                                              uint8_t index, uint8_t resolution) {
  auto s = this->get_sensor_by_address(name, 0, resolution);
  s->set_index(index);
  return s;
}
void DallasComponent::update() {
  disable_interrupts();
  bool result;
  if (!this->one_wire_->reset()) {
    result = false;
  } else {
    result = true;
    this->one_wire_->skip();
    this->one_wire_->write8(DALLAS_COMMAND_START_CONVERSION);
  }
  enable_interrupts();

  if (!result) {
    ESP_LOGE(TAG, "Requesting conversion failed");
    return;
  }

  for (auto *sensor : this->sensors_) {
    this->set_timeout(sensor->get_address_name(), sensor->millis_to_wait_for_conversion_(), [sensor] {
      disable_interrupts();
      bool res = sensor->read_scratch_pad_();
      enable_interrupts();

      if (!res)
        return;
      if (!sensor->check_scratch_pad_())
        return;

      float tempc = sensor->get_temp_c();
      ESP_LOGD(TAG, "'%s': Got Temperature=%.1f°C", sensor->get_name().c_str(), tempc);
      sensor->push_new_value(tempc);
    });
  }
}
DallasComponent::DallasComponent(ESPOneWire *one_wire, uint32_t update_interval)
    : PollingComponent(update_interval), one_wire_(one_wire) {

}
ESPOneWire *DallasComponent::get_one_wire() const {
  return this->one_wire_;
}

DallasTemperatureSensor::DallasTemperatureSensor(const std::string &name,
                                                 uint64_t address, uint8_t resolution,
                                                 DallasComponent *parent)
    : EmptyPollingParentSensor(name, parent) {
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
const std::string &DallasTemperatureSensor::get_address_name() {
  if (this->address_name_.empty()) {
    this->address_name_ = std::string("0x") + uint64_to_string(this->address_);
  }

  return this->address_name_;
}
bool DallasTemperatureSensor::read_scratch_pad_() {
  ESPOneWire *wire = this->parent_->get_one_wire();
  if (!wire->reset()) {
    ESP_LOGE(TAG, "Reading scratchpad failed: reset");
    return false;
  }

  wire->select(this->address_);
  wire->write8(DALLAS_COMMAND_READ_SCRATCH_PAD);

  for (unsigned char &i : this->scratch_pad_) {
    i = wire->read8();
  }
  return true;
}
void DallasTemperatureSensor::setup_sensor_() {

  disable_interrupts();
  bool r = this->read_scratch_pad_();
  enable_interrupts();

  if (!r)
    return;
  if (!this->check_scratch_pad_())
    return;

  if (this->scratch_pad_[4] == this->resolution_)
    return;

  if (this->get_address8()[0] == DALLAS_MODEL_DS18S20) {
    // DS18S20 doesn't support resolution.
    ESP_LOGW(TAG, "DS18S20 doesn't support setting resolution.");
    return;
  }

  switch (this->resolution_) {
    case 12:this->scratch_pad_[4] = 0x7F;
      break;
    case 11:this->scratch_pad_[4] = 0x5F;
      break;
    case 10:this->scratch_pad_[4] = 0x3F;
      break;
    case 9:
    default:this->scratch_pad_[4] = 0x1F;
      break;
  }

  ESPOneWire *wire = this->parent_->get_one_wire();
  disable_interrupts();
  if (wire->reset()) {
    wire->select(this->address_);
    wire->write8(DALLAS_COMMAND_WRITE_SCRATCH_PAD);
    wire->write8(this->scratch_pad_[2]); // high alarm temp
    wire->write8(this->scratch_pad_[3]); // low alarm temp
    wire->write8(this->scratch_pad_[4]); // resolution
    wire->reset();

    // write value to EEPROM
    wire->select(this->address_);
    wire->write8(0x48);
  }
  enable_interrupts();

  delay(20);  // allow it to finish operation
  wire->reset();
}
bool DallasTemperatureSensor::check_scratch_pad_() {
  if_very_verbose {
    ESP_LOGVV(TAG, "Scratch pad: %02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X (%02X)",
              this->scratch_pad_[0], this->scratch_pad_[1], this->scratch_pad_[2],
              this->scratch_pad_[3], this->scratch_pad_[4], this->scratch_pad_[5],
              this->scratch_pad_[6], this->scratch_pad_[7], this->scratch_pad_[8],
              crc8(this->scratch_pad_, 8));
  }
  if (crc8(this->scratch_pad_, 8) != this->scratch_pad_[8]) {
    ESP_LOGE(TAG, "Reading scratch pad from Dallas Sensor failed");
    return false;
  }
  return true;
}
float DallasTemperatureSensor::get_temp_c() {
  int16_t temp = (int16_t(this->scratch_pad_[1]) << 11) | (int16_t(this->scratch_pad_[0]) << 3);
  if (this->get_address8()[0] == DALLAS_MODEL_DS18S20) {
    int diff = (this->scratch_pad_[7] - this->scratch_pad_[6]) << 7;
    temp = ((temp & 0xFFF0) << 3) - 16 + (diff / this->scratch_pad_[7]);
  }

  return temp / 128.0f;
}
std::string DallasTemperatureSensor::unique_id() {
  return "dallas-" + uint64_to_string(this->address_);
}

} // namespace sensor

} // namespace esphomelib

#endif //USE_DALLAS_SENSOR
