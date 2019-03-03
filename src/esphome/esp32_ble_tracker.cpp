#include "esphome/defines.h"

#ifdef USE_ESP32_BLE_TRACKER

#include "esphome/esp32_ble_tracker.h"
#include <nvs_flash.h>
#include <freertos/FreeRTOSConfig.h>
#include <esp_bt_main.h>
#include <esp_bt.h>
#include <freertos/task.h>
#include <esp_gap_ble_api.h>
#include <esp_bt_defs.h>
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

// bt_trace.h
#undef TAG

static const char *TAG = "esp32_ble_tracker";

ESP32BLETracker *global_esp32_ble_tracker = nullptr;

uint64_t ble_addr_to_uint64(const esp_bd_addr_t address) {
  uint64_t u = 0;
  u |= uint64_t(address[0] & 0xFF) << 40;
  u |= uint64_t(address[1] & 0xFF) << 32;
  u |= uint64_t(address[2] & 0xFF) << 24;
  u |= uint64_t(address[3] & 0xFF) << 16;
  u |= uint64_t(address[4] & 0xFF) << 8;
  u |= uint64_t(address[5] & 0xFF) << 0;
  return u;
}

ESP32BLEPresenceDevice *ESP32BLETracker::make_presence_sensor(const std::string &name, std::array<uint8_t, 6> address) {
  uint64_t addr = ble_addr_to_uint64(address.cbegin());
  auto *dev = new ESP32BLEPresenceDevice(name, addr);
  this->presence_sensors_.push_back(dev);
  return dev;
}

ESP32BLERSSISensor *ESP32BLETracker::make_rssi_sensor(const std::string &name, std::array<uint8_t, 6> address) {
  uint64_t addr = ble_addr_to_uint64(address.cbegin());
  auto *dev = new ESP32BLERSSISensor(this, name, addr);
  this->rssi_sensors_.push_back(dev);
  return dev;
}

XiaomiDevice *ESP32BLETracker::make_xiaomi_device(std::array<uint8_t, 6> address) {
  uint64_t addr = ble_addr_to_uint64(address.cbegin());
  auto *dev = new XiaomiDevice(this, addr);
  this->xiaomi_devices_.push_back(dev);
  return dev;
}

void ESP32BLETracker::setup() {
  global_esp32_ble_tracker = this;
  this->scan_result_lock_ = xSemaphoreCreateMutex();
  this->scan_end_lock_ = xSemaphoreCreateMutex();

  if (!ESP32BLETracker::ble_setup()) {
    this->mark_failed();
    return;
  }

  global_esp32_ble_tracker->start_scan(true);
}

void ESP32BLETracker::loop() {
  if (xSemaphoreTake(this->scan_end_lock_, 0L)) {
    xSemaphoreGive(this->scan_end_lock_);
    global_esp32_ble_tracker->start_scan(false);
  }

  if (xSemaphoreTake(this->scan_result_lock_, 5L / portTICK_PERIOD_MS)) {
    uint32_t index = this->scan_result_index_;
    xSemaphoreGive(this->scan_result_lock_);

    if (index >= 16) {
      ESP_LOGW(TAG, "Too many BLE events to process. Some devices may not show up.");
    }
    for (size_t i = 0; i < index; i++) {
      ESPBTDevice device;
      device.parse_scan_rst(this->scan_result_buffer_[i]);

      this->parse_rssi_sensors_(device);
      this->parse_xiaomi_sensors_(device);

      if (this->parse_already_discovered_(device))
        continue;
      this->parse_presence_sensors_(device);
    }

    if (xSemaphoreTake(this->scan_result_lock_, 10L / portTICK_PERIOD_MS)) {
      this->scan_result_index_ = 0;
      xSemaphoreGive(this->scan_result_lock_);
    }
  }

  if (this->scan_set_param_failed_) {
    ESP_LOGE(TAG, "Scan set param failed: %d", this->scan_set_param_failed_);
    this->scan_set_param_failed_ = ESP_BT_STATUS_SUCCESS;
  }

  if (this->scan_start_failed_) {
    ESP_LOGE(TAG, "Scan start failed: %d", this->scan_start_failed_);
    this->scan_start_failed_ = ESP_BT_STATUS_SUCCESS;
  }
}

bool ESP32BLETracker::ble_setup() {
  // Initialize non-volatile storage for the bluetooth controller
  esp_err_t err = nvs_flash_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "nvs_flash_init failed: %d", err);
    return false;
  }

  // Initialize the bluetooth controller with the default configuration
  if (!btStart()) {
    ESP_LOGE(TAG, "btStart failed: %d", esp_bt_controller_get_status());
    return false;
  }

  esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);

  err = esp_bluedroid_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_bluedroid_init failed: %d", err);
    return false;
  }
  err = esp_bluedroid_enable();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_bluedroid_enable failed: %d", err);
    return false;
  }
  err = esp_ble_gap_register_callback(ESP32BLETracker::gap_event_handler);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_ble_gap_register_callback failed: %d", err);
    return false;
  }

  // Empty name
  esp_ble_gap_set_device_name("");

  esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;
  err = esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_ble_gap_set_security_param failed: %d", err);
    return false;
  }

  // BLE takes some time to be fully set up, 200ms should be more than enough
  delay(200);

  return true;
}

void ESP32BLETracker::start_scan(bool first) {
  if (!xSemaphoreTake(this->scan_end_lock_, 0L)) {
    ESP_LOGW("Cannot start scan!");
    return;
  }

  ESP_LOGD(TAG, "Starting scan...");
  for (auto *device : this->presence_sensors_) {
    if (!this->has_already_discovered_(device->address_))
      device->publish_state(false);
  }
  this->already_discovered_.clear();

  this->scan_params_.scan_type = BLE_SCAN_TYPE_ACTIVE;
  this->scan_params_.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
  this->scan_params_.scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL;
  // Values determined empirically, higher scan intervals and lower scan windows make the ESP more stable
  // Ideally, these values should both be quite low, especially scan window. 0x10/0x10 is the esp-idf
  // default and works quite well. 0x100/0x50 discovers a few less BLE broadcast packets but is a lot
  // more stable (order of several hours). The old ESPHome default (1600/1600) was terrible with
  // crashes every few minutes
  this->scan_params_.scan_interval = 0x200;
  this->scan_params_.scan_window = 0x30;

  esp_ble_gap_set_scan_params(&this->scan_params_);
  esp_ble_gap_start_scanning(this->scan_interval_);
}

void ESP32BLETracker::gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
  switch (event) {
    case ESP_GAP_BLE_SCAN_RESULT_EVT:
      global_esp32_ble_tracker->gap_scan_result(param->scan_rst);
      break;
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
      global_esp32_ble_tracker->gap_scan_set_param_complete(param->scan_param_cmpl);
      break;
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
      global_esp32_ble_tracker->gap_scan_start_complete(param->scan_start_cmpl);
      break;
    default:
      break;
  }
}

void ESP32BLETracker::gap_scan_set_param_complete(const esp_ble_gap_cb_param_t::ble_scan_param_cmpl_evt_param &param) {
  this->scan_set_param_failed_ = param.status;
}

void ESP32BLETracker::gap_scan_start_complete(const esp_ble_gap_cb_param_t::ble_scan_start_cmpl_evt_param &param) {
  this->scan_start_failed_ = param.status;
}

void ESP32BLETracker::gap_scan_result(const esp_ble_gap_cb_param_t::ble_scan_result_evt_param &param) {
  if (param.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
    if (xSemaphoreTake(this->scan_result_lock_, 0L)) {
      if (this->scan_result_index_ < 16) {
        this->scan_result_buffer_[this->scan_result_index_++] = param;
      }
      xSemaphoreGive(this->scan_result_lock_);
    }
  } else if (param.search_evt == ESP_GAP_SEARCH_INQ_CMPL_EVT) {
    xSemaphoreGive(this->scan_end_lock_);
  }
}

bool ESP32BLETracker::has_already_discovered_(uint64_t address) {
  for (auto addr : this->already_discovered_) {
    if (addr == address) {
      return true;
    }
  }
  return false;
}

void ESP32BLETracker::parse_rssi_sensors_(const ESPBTDevice &device) {
  const uint64_t address = device.address_uint64();
  int rssi = device.get_rssi();
  for (auto *dev : this->rssi_sensors_) {
    if (dev->address_ == address)
      dev->publish_state(rssi);
  }
}

enum XiaomiDataType {
  XIAOMI_NO_DATA = 0,
  XIAOMI_TEMPERATURE_HUMIDITY,
  XIAOMI_TEMPERATURE,
  XIAOMI_HUMIDITY,
  XIAOMI_BATTERY_LEVEL,
  XIAOMI_CONDUCTIVITY,
  XIAOMI_ILLUMINANCE,
  XIAOMI_MOISTURE,
};

XiaomiDataType parse_xiaomi(uint8_t data_type, const uint8_t *data, uint8_t data_length, float *data1, float *data2) {
  switch (data_type) {
    case 0x0D: {  // temperature+humidity, 4 bytes, 16-bit signed integer (LE) each, 0.1 °C, 0.1 %
      if (data_length != 4)
        return XIAOMI_NO_DATA;
      const int16_t temperature = uint16_t(data[0]) | (uint16_t(data[1]) << 8);
      const int16_t humidity = uint16_t(data[2]) | (uint16_t(data[3]) << 8);
      *data1 = temperature / 10.0f;
      *data2 = humidity / 10.0f;
      return XIAOMI_TEMPERATURE_HUMIDITY;
    }
    case 0x0A: {  // battery, 1 byte, 8-bit unsigned integer, 1 %
      if (data_length != 1)
        return XIAOMI_NO_DATA;
      *data1 = data[0];
      return XIAOMI_BATTERY_LEVEL;
    }
    case 0x06: {  // humidity, 2 bytes, 16-bit signed integer (LE), 0.1 %
      if (data_length != 2)
        return XIAOMI_NO_DATA;
      const int16_t humidity = uint16_t(data[0]) | (uint16_t(data[1]) << 8);
      *data1 = humidity / 10.0f;
      return XIAOMI_HUMIDITY;
    }
    case 0x04: {  // temperature, 2 bytes, 16-bit signed integer (LE), 0.1 °C
      if (data_length != 2)
        return XIAOMI_NO_DATA;
      const int16_t temperature = uint16_t(data[0]) | (uint16_t(data[1]) << 8);
      *data1 = temperature / 10.0f;
      return XIAOMI_TEMPERATURE;
    }
    case 0x09: {  // conductivity, 2 bytes, 16-bit unsigned integer (LE), 1 µS/cm
      if (data_length != 2)
        return XIAOMI_NO_DATA;
      const uint16_t conductivity = uint16_t(data[0]) | (uint16_t(data[1]) << 8);
      *data1 = conductivity;
      return XIAOMI_CONDUCTIVITY;
    }
    case 0x07: {  // illuminance, 3 bytes, 24-bit unsigned integer (LE), 1 lx
      if (data_length != 3)
        return XIAOMI_NO_DATA;
      const uint32_t illuminance = uint32_t(data[0]) | (uint32_t(data[1]) << 8) | (uint32_t(data[2]) << 16);
      *data1 = illuminance;
      return XIAOMI_ILLUMINANCE;
    }
    case 0x08: {  // soil moisture, 1 byte, 8-bit unsigned integer, 1 %
      if (data_length != 1)
        return XIAOMI_NO_DATA;
      *data1 = data[0];
      return XIAOMI_MOISTURE;
    }
    default:
      return XIAOMI_NO_DATA;
  }
}

void ESP32BLETracker::parse_xiaomi_sensors_(const ESPBTDevice &device) {
  const uint64_t address = device.address_uint64();

  if (!device.get_service_data_uuid().has_value()) {
    ESP_LOGVV(TAG, "Xiaomi no service data");
    return;
  }

  if (!device.get_service_data_uuid()->contains(0x95, 0xFE)) {
    ESP_LOGVV(TAG, "Xiaomi no service data UUID magic bytes");
    return;
  }

  const auto *raw = reinterpret_cast<const uint8_t *>(device.get_service_data().data());

  if (device.get_service_data().size() < 14) {
    ESP_LOGVV(TAG, "Xiaomi service data too short!");
    return;
  }

  bool is_mijia = (raw[1] & 0x20) == 0x20 && raw[2] == 0xAA && raw[3] == 0x01;
  bool is_miflora = (raw[1] & 0x20) == 0x20 && raw[2] == 0x98 && raw[3] == 0x00;

  if (!is_mijia && !is_miflora) {
    ESP_LOGVV(TAG, "Xiaomi no magic bytes");
    return;
  }

  const char *type = is_mijia ? "MiJia" : "MiFlora";
  uint8_t raw_offset = is_mijia ? 11 : 12;

  const uint8_t raw_type = raw[raw_offset];
  const uint8_t data_length = raw[raw_offset + 2];
  const uint8_t *data = &raw[raw_offset + 3];
  const uint8_t expected_length = data_length + raw_offset + 3;
  const uint8_t actual_length = device.get_service_data().size();
  if (expected_length != actual_length) {
    ESP_LOGV(TAG, "Xiaomi %s data length mismatch (%u != %d)", type, expected_length, actual_length);
    return;
  }
  float data1, data2;
  XiaomiDataType data_type = parse_xiaomi(raw_type, data, data_length, &data1, &data2);
  if (data_type == XIAOMI_NO_DATA)
    return;

  std::string address_str = device.address_str();
  switch (data_type) {
    case XIAOMI_TEMPERATURE_HUMIDITY:
      ESP_LOGD(TAG, "Xiaomi %s %s Got temperature=%.1f°C, humidity=%.1f%%", type, address_str.c_str(), data1, data2);
      break;
    case XIAOMI_TEMPERATURE:
      ESP_LOGD(TAG, "Xiaomi %s %s Got temperature=%.1f°C", type, address_str.c_str(), data1);
      break;
    case XIAOMI_HUMIDITY:
      ESP_LOGD(TAG, "Xiaomi %s %s Got humidity=%.1f%%", type, address_str.c_str(), data1);
      break;
    case XIAOMI_BATTERY_LEVEL:
      ESP_LOGD(TAG, "Xiaomi %s %s Got battery level=%.0f%%", type, address_str.c_str(), data1);
      break;
    case XIAOMI_MOISTURE:
      ESP_LOGD(TAG, "Xiaomi %s %s Got moisture=%.0f%%", type, address_str.c_str(), data1);
      break;
    case XIAOMI_ILLUMINANCE:
      ESP_LOGD(TAG, "Xiaomi %s %s Got illuminance=%.0flx", type, address_str.c_str(), data1);
      break;
    case XIAOMI_CONDUCTIVITY:
      ESP_LOGD(TAG, "Xiaomi %s %s Got soil conductivity=%.0fµS/cm", type, address_str.c_str(), data1);
      break;
    default:
      break;
  }

  for (auto *dev : this->xiaomi_devices_) {
    if (dev->address_ == address) {
      switch (data_type) {
        case XIAOMI_TEMPERATURE_HUMIDITY:
          if (dev->get_temperature_sensor() != nullptr)
            dev->get_temperature_sensor()->publish_state(data1);
          if (dev->get_humidity_sensor() != nullptr)
            dev->get_humidity_sensor()->publish_state(data2);
          break;
        case XIAOMI_HUMIDITY:
          if (dev->get_humidity_sensor() != nullptr)
            dev->get_humidity_sensor()->publish_state(data1);
          break;
        case XIAOMI_BATTERY_LEVEL:
          if (dev->get_battery_level_sensor() != nullptr)
            dev->get_battery_level_sensor()->publish_state(data1);
          break;
        case XIAOMI_TEMPERATURE:
          if (dev->get_temperature_sensor() != nullptr)
            dev->get_temperature_sensor()->publish_state(data1);
          break;
        case XIAOMI_MOISTURE:
          if (dev->get_moisture_sensor() != nullptr)
            dev->get_moisture_sensor()->publish_state(data1);
          break;
        case XIAOMI_ILLUMINANCE:
          if (dev->get_illuminance_sensor() != nullptr)
            dev->get_illuminance_sensor()->publish_state(data1);
          break;
        case XIAOMI_CONDUCTIVITY:
          if (dev->get_conductivity_sensor() != nullptr)
            dev->get_conductivity_sensor()->publish_state(data1);
          break;
        default:
          break;
      }
    }
  }
}

bool ESP32BLETracker::parse_already_discovered_(const ESPBTDevice &device) {
  const uint64_t address = device.address_uint64();
  // O(N^2), but won't be called that often anyway and this is probably better with memory allocation
  if (this->has_already_discovered_(address)) {
    ESP_LOGV(TAG, "Already discovered device %s", device.address_str().c_str());
    return true;
  }
  this->already_discovered_.push_back(address);

#ifdef ESPHOME_LOG_HAS_DEBUG
  ESP_LOGD(TAG, "Found device %s RSSI=%d", device.address_str().c_str(), device.get_rssi());

  const char *address_type_s;
  switch (device.get_address_type()) {
    case BLE_ADDR_TYPE_PUBLIC:
      address_type_s = "PUBLIC";
      break;
    case BLE_ADDR_TYPE_RANDOM:
      address_type_s = "RANDOM";
      break;
    case BLE_ADDR_TYPE_RPA_PUBLIC:
      address_type_s = "RPA_PUBLIC";
      break;
    case BLE_ADDR_TYPE_RPA_RANDOM:
      address_type_s = "RPA_RANDOM";
      break;
    default:
      address_type_s = "UNKNOWN";
      break;
  }

  ESP_LOGD(TAG, "  Address Type: %s", address_type_s);
  if (!device.get_name().empty())
    ESP_LOGD(TAG, "  Name: '%s'", device.get_name().c_str());
  if (device.get_tx_power().has_value()) {
    ESP_LOGD(TAG, "  TX Power: %d", *device.get_tx_power());
  }
#endif

  return false;
}

void ESP32BLETracker::parse_presence_sensors_(const ESPBTDevice &device) {
  const uint64_t address = device.address_uint64();
  for (auto *dev : this->presence_sensors_) {
    if (dev->address_ == address)
      dev->publish_state(true);
  }
}

ESPBTUUID::ESPBTUUID() : uuid_() {}
ESPBTUUID ESPBTUUID::from_uint16(uint16_t uuid) {
  ESPBTUUID ret;
  ret.uuid_.len = ESP_UUID_LEN_16;
  ret.uuid_.uuid.uuid16 = uuid;
  return ret;
}
ESPBTUUID ESPBTUUID::from_uint32(uint32_t uuid) {
  ESPBTUUID ret;
  ret.uuid_.len = ESP_UUID_LEN_32;
  ret.uuid_.uuid.uuid32 = uuid;
  return ret;
}
ESPBTUUID ESPBTUUID::from_raw(const uint8_t *data) {
  ESPBTUUID ret;
  ret.uuid_.len = ESP_UUID_LEN_128;
  for (size_t i = 0; i < ESP_UUID_LEN_128; i++)
    ret.uuid_.uuid.uuid128[i] = data[i];
  return ret;
}
bool ESPBTUUID::contains(uint8_t data1, uint8_t data2) const {
  if (this->uuid_.len == ESP_UUID_LEN_16) {
    return (this->uuid_.uuid.uuid16 >> 8) == data2 || (this->uuid_.uuid.uuid16 & 0xFF) == data1;
  } else if (this->uuid_.len == ESP_UUID_LEN_32) {
    for (uint8_t i = 0; i < 3; i++) {
      bool a = ((this->uuid_.uuid.uuid32 >> i * 8) & 0xFF) == data1;
      bool b = ((this->uuid_.uuid.uuid32 >> (i + 1) * 8) & 0xFF) == data2;
      if (a && b)
        return true;
    }
  } else {
    for (uint8_t i = 0; i < 15; i++) {
      if (this->uuid_.uuid.uuid128[i] == data1 && this->uuid_.uuid.uuid128[i + 1] == data2)
        return true;
    }
  }
  return false;
}
std::string ESPBTUUID::to_string() {
  char sbuf[64];
  switch (this->uuid_.len) {
    case ESP_UUID_LEN_16:
      sprintf(sbuf, "%02X:%02X", this->uuid_.uuid.uuid16 >> 8, this->uuid_.uuid.uuid16);
      break;
    case ESP_UUID_LEN_32:
      sprintf(sbuf, "%02X:%02X:%02X:%02X", this->uuid_.uuid.uuid32 >> 24, this->uuid_.uuid.uuid32 >> 16,
              this->uuid_.uuid.uuid32 >> 8, this->uuid_.uuid.uuid32);
      break;
    default:
    case ESP_UUID_LEN_128:
      for (uint8_t i = 0; i < 16; i++)
        sprintf(sbuf + i * 3, "%02X:", this->uuid_.uuid.uuid128[i]);
      sbuf[47] = '\0';
      break;
  }
  return sbuf;
}

void ESPBTDevice::parse_scan_rst(const esp_ble_gap_cb_param_t::ble_scan_result_evt_param &param) {
  for (uint8_t i = 0; i < ESP_BD_ADDR_LEN; i++)
    this->address_[i] = param.bda[i];
  this->address_type_ = param.ble_addr_type;
  this->rssi_ = param.rssi;
  this->parse_adv(param);

#ifdef ESPHOME_LOG_HAS_VERY_VERBOSE
  ESP_LOGVV(TAG, "Parse Result:");
  const char *address_type = "";
  switch (this->address_type_) {
    case BLE_ADDR_TYPE_PUBLIC:
      address_type = "PUBLIC";
      break;
    case BLE_ADDR_TYPE_RANDOM:
      address_type = "RANDOM";
      break;
    case BLE_ADDR_TYPE_RPA_PUBLIC:
      address_type = "RPA_PUBLIC";
      break;
    case BLE_ADDR_TYPE_RPA_RANDOM:
      address_type = "RPA_RANDOM";
      break;
  }
  ESP_LOGVV(TAG, "  Address: %02X:%02X:%02X:%02X:%02X:%02X (%s)", this->address_[0], this->address_[1],
            this->address_[2], this->address_[3], this->address_[4], this->address_[5], address_type);

  ESP_LOGVV(TAG, "  RSSI: %d", this->rssi_);
  ESP_LOGVV(TAG, "  Name: %s", this->name_.c_str());
  if (this->tx_power_.has_value()) {
    ESP_LOGVV(TAG, "  TX Power: %d", *this->tx_power_);
  }
  if (this->appearance_.has_value()) {
    ESP_LOGVV(TAG, "  Appearance: %u", *this->appearance_);
  }
  if (this->ad_flag_.has_value()) {
    ESP_LOGVV(TAG, "  Ad Flag: %u", *this->ad_flag_);
  }
  for (auto uuid : this->service_uuids_) {
    ESP_LOGVV(TAG, "  Service UUID: %s", uuid.to_string().c_str());
  }
  ESP_LOGVV(TAG, "  Manufacturer data: '%s'", this->manufacturer_data_.c_str());
  ESP_LOGVV(TAG, "  Service data: '%s'", this->service_data_.c_str());

  if (this->service_data_uuid_.has_value()) {
    ESP_LOGVV(TAG, "  Service Data UUID: %s", this->service_data_uuid_->to_string().c_str());
  }

  char buffer[200];
  size_t off = 0;
  for (uint8_t i = 0; i < param.adv_data_len; i++) {
    int ret = snprintf(buffer + off, sizeof(buffer) - off, "%02X.", param.ble_adv[i]);
    if (ret < 0) {
      break;
    }
    off += ret;
  }
  ESP_LOGVV(TAG, "Adv data: %s (%u bytes)", buffer, param.adv_data_len);
#endif
}
void ESPBTDevice::parse_adv(const esp_ble_gap_cb_param_t::ble_scan_result_evt_param &param) {
  size_t offset = 0;
  const uint8_t *payload = param.ble_adv;
  uint8_t len = param.adv_data_len;

  while (offset + 2 < len) {
    const uint8_t field_length = payload[offset++];  // First byte is length of adv record
    if (field_length == 0)
      break;

    // first byte of adv record is adv record type
    const uint8_t record_type = payload[offset++];
    const uint8_t *record = &payload[offset];
    const uint8_t record_length = field_length - 1;
    offset += record_length;

    switch (record_type) {
      case ESP_BLE_AD_TYPE_NAME_CMPL: {
        this->name_ = std::string(reinterpret_cast<const char *>(record), record_length);
        break;
      }
      case ESP_BLE_AD_TYPE_TX_PWR: {
        this->tx_power_ = *payload;
        break;
      }
      case ESP_BLE_AD_TYPE_APPEARANCE: {
        this->appearance_ = *reinterpret_cast<const uint16_t *>(record);
        break;
      }
      case ESP_BLE_AD_TYPE_FLAG: {
        this->ad_flag_ = *record;
        break;
      }
      case ESP_BLE_AD_TYPE_16SRV_CMPL:
      case ESP_BLE_AD_TYPE_16SRV_PART: {
        for (uint8_t i = 0; i < record_length / 2; i++) {
          this->service_uuids_.push_back(ESPBTUUID::from_uint16(*reinterpret_cast<const uint16_t *>(record + 2 * i)));
        }
        break;
      }
      case ESP_BLE_AD_TYPE_32SRV_CMPL:
      case ESP_BLE_AD_TYPE_32SRV_PART: {
        for (uint8_t i = 0; i < record_length / 4; i++) {
          this->service_uuids_.push_back(ESPBTUUID::from_uint32(*reinterpret_cast<const uint32_t *>(record + 4 * i)));
        }
        break;
      }
      case ESP_BLE_AD_TYPE_128SRV_CMPL:
      case ESP_BLE_AD_TYPE_128SRV_PART: {
        this->service_uuids_.push_back(ESPBTUUID::from_raw(record));
        break;
      }
      case ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE: {
        this->manufacturer_data_ = std::string(reinterpret_cast<const char *>(record), record_length);
        break;
      }
      case ESP_BLE_AD_TYPE_SERVICE_DATA: {
        if (record_length < 2) {
          ESP_LOGV(TAG, "Record length too small for ESP_BLE_AD_TYPE_SERVICE_DATA");
          break;
        }
        this->service_data_uuid_ = ESPBTUUID::from_uint16(*reinterpret_cast<const uint16_t *>(record));
        if (record_length > 2)
          this->service_data_ = std::string(reinterpret_cast<const char *>(record + 2), record_length - 2UL);
        break;
      }
      case ESP_BLE_AD_TYPE_32SERVICE_DATA: {
        if (record_length < 4) {
          ESP_LOGV(TAG, "Record length too small for ESP_BLE_AD_TYPE_32SERVICE_DATA");
          break;
        }
        this->service_data_uuid_ = ESPBTUUID::from_uint32(*reinterpret_cast<const uint32_t *>(record));
        if (record_length > 4)
          this->service_data_ = std::string(reinterpret_cast<const char *>(record + 4), record_length - 4UL);
        break;
      }
      case ESP_BLE_AD_TYPE_128SERVICE_DATA: {
        if (record_length < 16) {
          ESP_LOGV(TAG, "Record length too small for ESP_BLE_AD_TYPE_128SERVICE_DATA");
          break;
        }
        this->service_data_uuid_ = ESPBTUUID::from_raw(record);
        if (record_length > 16)
          this->service_data_ = std::string(reinterpret_cast<const char *>(record + 16), record_length - 16UL);
        break;
      }
      default: {
        ESP_LOGV(TAG, "Unhandled type: advType: 0x%02x", record_type);
        break;
      }
    }
  }
}
std::string ESPBTDevice::address_str() const {
  char mac[24];
  snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X", this->address_[0], this->address_[1], this->address_[2],
           this->address_[3], this->address_[4], this->address_[5]);
  return mac;
}
uint64_t ESPBTDevice::address_uint64() const { return ble_addr_to_uint64(this->address_); }
esp_ble_addr_type_t ESPBTDevice::get_address_type() const { return this->address_type_; }
int ESPBTDevice::get_rssi() const { return this->rssi_; }
const std::string &ESPBTDevice::get_name() const { return this->name_; }
const optional<int8_t> &ESPBTDevice::get_tx_power() const { return this->tx_power_; }
const optional<uint16_t> &ESPBTDevice::get_appearance() const { return this->appearance_; }
const optional<uint8_t> &ESPBTDevice::get_ad_flag() const { return this->ad_flag_; }
const std::vector<ESPBTUUID> &ESPBTDevice::get_service_uuids() const { return this->service_uuids_; }
const std::string &ESPBTDevice::get_manufacturer_data() const { return this->manufacturer_data_; }
const std::string &ESPBTDevice::get_service_data() const { return this->service_data_; }
const optional<ESPBTUUID> &ESPBTDevice::get_service_data_uuid() const { return this->service_data_uuid_; }

void ESP32BLETracker::set_scan_interval(uint32_t scan_interval) { this->scan_interval_ = scan_interval; }
uint32_t ESP32BLETracker::get_scan_interval() const { return this->scan_interval_; }
void ESP32BLETracker::dump_config() {
  ESP_LOGCONFIG(TAG, "BLE Tracker:");
  ESP_LOGCONFIG(TAG, "  Scan Interval: %u s", this->scan_interval_);
  for (auto *child : this->presence_sensors_) {
    LOG_BINARY_SENSOR("  ", "Presence", child);
  }
  for (auto *child : this->rssi_sensors_) {
    LOG_SENSOR("  ", "RSSI", child);
  }
  for (auto *child : this->xiaomi_devices_) {
    ESP_LOGCONFIG(TAG, "  Xiaomi %s", child->unique_id().c_str());
    LOG_SENSOR("    ", "Temperature ", child->get_temperature_sensor());
    LOG_SENSOR("    ", "Humidity ", child->get_humidity_sensor());
    LOG_SENSOR("    ", "Moisture ", child->get_moisture_sensor());
    LOG_SENSOR("    ", "Illuminance ", child->get_illuminance_sensor());
    LOG_SENSOR("    ", "Conductivity ", child->get_conductivity_sensor());
    LOG_SENSOR("    ", "Battery Level ", child->get_battery_level_sensor());
  }
}

std::string ESP32BLERSSISensor::unit_of_measurement() { return "dB"; }

std::string ESP32BLERSSISensor::icon() { return "mdi:signal"; }
int8_t ESP32BLERSSISensor::accuracy_decimals() { return Sensor::accuracy_decimals(); }
std::string ESP32BLERSSISensor::unique_id() {
  char buffer[32];
  sprintf(buffer, "ble-%08X%08X-rssi", uint32_t(this->address_ >> 32), uint32_t(this->address_));
  return buffer;
}
uint32_t ESP32BLERSSISensor::update_interval() { return this->parent_->get_scan_interval() * 1000u; }
ESP32BLERSSISensor::ESP32BLERSSISensor(ESP32BLETracker *parent, const std::string &name, uint64_t address)
    : Sensor(name), parent_(parent), address_(address) {}
uint32_t XiaomiDevice::update_interval() const {
  // Double the scan interval because Xiaomis don't send values too often.
  return this->parent_->get_scan_interval() * 2000;
}
std::string XiaomiDevice::unique_id() const { return uint64_to_string(this->address_); }

ESP32BLEPresenceDevice::ESP32BLEPresenceDevice(const std::string &name, uint64_t address)
    : BinarySensor(name), address_(address) {}
std::string ESP32BLEPresenceDevice::device_class() { return "presence"; }

std::string XiaomiSensor::unit_of_measurement() {
  switch (this->type_) {
    case TYPE_TEMPERATURE:
      return sensor::UNIT_C;
    case TYPE_CONDUCTIVITY:
      return sensor::UNIT_MICROSIEMENS_PER_CENTIMETER;
    case TYPE_ILLUMINANCE:
      return sensor::UNIT_LX;
    case TYPE_HUMIDITY:
    case TYPE_MOISTURE:
    case TYPE_BATTERY_LEVEL:
      return sensor::UNIT_PERCENT;
  }
  return "";
}
std::string XiaomiSensor::icon() {
  switch (this->type_) {
    case TYPE_TEMPERATURE:
      return sensor::ICON_EMPTY;
    case TYPE_HUMIDITY:
    case TYPE_MOISTURE:
      return sensor::ICON_WATER_PERCENT;
    case TYPE_BATTERY_LEVEL:
      return sensor::ICON_BATTERY;
    case TYPE_ILLUMINANCE:
      return sensor::ICON_BRIGHTNESS_5;
    case TYPE_CONDUCTIVITY:
      return sensor::ICON_FLOWER;
  }

  return "";
}
uint32_t XiaomiSensor::update_interval() { return this->parent_->update_interval(); }
int8_t XiaomiSensor::accuracy_decimals() {
  switch (this->type_) {
    case TYPE_TEMPERATURE:
    case TYPE_HUMIDITY:
      return 1;
    case TYPE_MOISTURE:
    case TYPE_BATTERY_LEVEL:
    case TYPE_ILLUMINANCE:
    case TYPE_CONDUCTIVITY:
      return 0;
  }
  return 0;
}
std::string XiaomiSensor::unique_id() {
  const char *suffix = "";
  switch (this->type_) {
    case TYPE_TEMPERATURE:
      suffix = "-temperature";
      break;
    case TYPE_HUMIDITY:
      suffix = "-humidity";
      break;
    case TYPE_BATTERY_LEVEL:
      suffix = "-battery";
      break;
    case TYPE_MOISTURE:
      suffix = "-moisture";
      break;
    case TYPE_ILLUMINANCE:
      suffix = "-illuminance";
      break;
    case TYPE_CONDUCTIVITY:
      suffix = "-conductivity";
      break;
  }
  return this->parent_->unique_id() + suffix;
}
XiaomiSensor::XiaomiSensor(XiaomiDevice *parent, XiaomiSensor::Type type, const std::string &name)
    : Sensor(name), parent_(parent), type_(type) {}

XiaomiSensor *XiaomiDevice::get_temperature_sensor() const { return this->temperature_sensor_; }
XiaomiSensor *XiaomiDevice::get_humidity_sensor() const { return this->humidity_sensor_; }
XiaomiSensor *XiaomiDevice::get_moisture_sensor() const { return this->moisture_sensor_; }
XiaomiSensor *XiaomiDevice::get_illuminance_sensor() const { return this->illuminance_sensor_; }
XiaomiSensor *XiaomiDevice::get_conductivity_sensor() const { return this->conductivity_sensor_; }
XiaomiSensor *XiaomiDevice::get_battery_level_sensor() const { return this->battery_level_sensor_; }
XiaomiSensor *XiaomiDevice::make_temperature_sensor(const std::string &name) {
  return this->temperature_sensor_ = new XiaomiSensor(this, XiaomiSensor::TYPE_TEMPERATURE, name);
}
XiaomiSensor *XiaomiDevice::make_humidity_sensor(const std::string &name) {
  return this->humidity_sensor_ = new XiaomiSensor(this, XiaomiSensor::TYPE_HUMIDITY, name);
}
XiaomiSensor *XiaomiDevice::make_moisture_sensor(const std::string &name) {
  return this->moisture_sensor_ = new XiaomiSensor(this, XiaomiSensor::TYPE_MOISTURE, name);
}
XiaomiSensor *XiaomiDevice::make_illuminance_sensor(const std::string &name) {
  return this->illuminance_sensor_ = new XiaomiSensor(this, XiaomiSensor::TYPE_ILLUMINANCE, name);
}
XiaomiSensor *XiaomiDevice::make_conductivity_sensor(const std::string &name) {
  return this->conductivity_sensor_ = new XiaomiSensor(this, XiaomiSensor::TYPE_CONDUCTIVITY, name);
}
XiaomiSensor *XiaomiDevice::make_battery_level_sensor(const std::string &name) {
  return this->battery_level_sensor_ = new XiaomiSensor(this, XiaomiSensor::TYPE_BATTERY_LEVEL, name);
}
XiaomiDevice::XiaomiDevice(ESP32BLETracker *parent, uint64_t address) : parent_(parent), address_(address) {}

ESPHOME_NAMESPACE_END

#endif  // USE_ESP32_BLE_TRACKER
