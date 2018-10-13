//
//  esp32_ble_tracker.cpp
//  esphomelib
//
//  Created by Otto Winter on 12.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/defines.h"

#ifdef USE_ESP32_BLE_TRACKER

#include "esphomelib/esp32_ble_tracker.h"
#include <nvs_flash.h>
#include <freertos/FreeRTOSConfig.h>
#include <esp_bt_main.h>
#include <esp_bt.h>
#include <freertos/task.h>
#include <esp_gap_ble_api.h>
#include <btc_spp.h>
#include <btc_gap_bt.h>
#include <esp_bt_defs.h>
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

// bt_trace.h
#undef TAG

static const char *TAG = "esp32_ble_tracker";

ESP32BLETracker *global_esp32_ble_tracker = nullptr;
SemaphoreHandle_t semaphore_scan_end;

uint64_t ble_addr_to_uint64(const esp_bd_addr_t address) {
  uint64_t u = 0;
  u |= uint64_t(address[0] & 0xFF) << 40;
  u |= uint64_t(address[0] & 0xFF) << 32;
  u |= uint64_t(address[1] & 0xFF) << 24;
  u |= uint64_t(address[2] & 0xFF) << 16;
  u |= uint64_t(address[3] & 0xFF) << 8;
  u |= uint64_t(address[4] & 0xFF) << 0;
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

XiaomiMiJiaDevice *ESP32BLETracker::make_mijia_sensor(const std::string &temperature_name,
                                                      const std::string &humidity_name,
                                                      std::array<uint8_t, 6> address) {
  uint64_t addr = ble_addr_to_uint64(address.cbegin());
  auto *dev = new XiaomiMiJiaDevice(addr, temperature_name, humidity_name);
  this->mijia_sensors_.push_back(dev);
  return dev;
}

XiaomiMiFloraDevice *ESP32BLETracker::make_miflora_sensor(std::array<uint8_t, 6> address) {
  uint64_t addr = ble_addr_to_uint64(address.cbegin());
  auto *dev = new XiaomiMiFloraDevice(addr);
  this->miflora_sensors_.push_back(dev);
  return dev;
}

void ESP32BLETracker::setup() {
  global_esp32_ble_tracker = this;

  xTaskCreatePinnedToCore(
      ESP32BLETracker::ble_core_task,
      "ble_task", // name
      10000, // stack size (in words)
      nullptr, // input params
      1, // priority
      nullptr, // Handle, not needed
      0 // core
  );
}

void ESP32BLETracker::ble_core_task(void *params) {
  ble_setup();

  while (true) {
    delay(1000);
  }
}

void ESP32BLETracker::ble_setup() {
  semaphore_scan_end = xSemaphoreCreateMutex();

  // Initialize non-volatile storage for the bluetooth controller
  esp_err_t err = nvs_flash_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "nvs_flash_init failed: %d", err);
    return;
  }

  // Initialize the bluetooth controller with the default configuration
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  err = esp_bt_controller_init(&bt_cfg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_bt_controller_init failed: %d", err);
    return;
  }

  err = esp_bt_controller_enable(ESP_BT_MODE_BLE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_bt_controller_enable failed: %d", err);
    return;
  }

  err = esp_bluedroid_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_bluedroid_init failed: %d", err);
    return;
  }
  err = esp_bluedroid_enable();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_bluedroid_enable failed: %d", err);
    return;
  }
  err = esp_ble_gap_register_callback(ESP32BLETracker::gap_event_handler);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_ble_gap_register_callback failed: %d", err);
    return;
  }

  // Empty name
  esp_ble_gap_set_device_name("");

  esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;
  err = esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_ble_gap_set_security_param failed: %d", err);
    return;
  }

  // BLE takes some time to be fully set up, 200ms should be more than enough
  delay(200);

  bool first = true;

  while (true) {
    global_esp32_ble_tracker->start_scan(first);
    first = false;
    // wait for result
    xSemaphoreTake(semaphore_scan_end, portMAX_DELAY);
    xSemaphoreGive(semaphore_scan_end);
  }
}

void ESP32BLETracker::start_scan(bool first) {
  xSemaphoreTake(semaphore_scan_end, portMAX_DELAY);

  ESP_LOGD(TAG, "Starting scan...");
  if (!first) {
    // also O(N^2), but will only be called once every minute or so
    for (auto *device : this->presence_sensors_) {
      if (!this->has_already_discovered_(device->address_))
        device->publish_state(false);
    }
    for (auto *device : this->rssi_sensors_) {
      if (!this->has_already_discovered_(device->address_))
        device->push_new_value(NAN);
    }
  }
  this->already_discovered_.clear();

  this->scan_params_.scan_type = BLE_SCAN_TYPE_ACTIVE;
  this->scan_params_.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
  this->scan_params_.scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL;
  this->scan_params_.scan_interval = 1600; // 1 second
  this->scan_params_.scan_window = 1600;

  esp_ble_gap_set_scan_params(&this->scan_params_);
  esp_ble_gap_start_scanning(this->scan_interval_);
}

void ESP32BLETracker::gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
  switch (event) {
    case ESP_GAP_BLE_SCAN_RESULT_EVT:global_esp32_ble_tracker->gap_scan_result(param->scan_rst);
      break;
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:global_esp32_ble_tracker->gap_scan_set_param_complete(param->scan_param_cmpl);
      break;
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:global_esp32_ble_tracker->gap_scan_start_complete(param->scan_start_cmpl);
      break;
    default: break;
  }
}

void ESP32BLETracker::gap_scan_set_param_complete(const esp_ble_gap_cb_param_t::ble_scan_param_cmpl_evt_param &param) {
  if (param.status != ESP_BT_STATUS_SUCCESS) {
    ESP_LOGE(TAG, "Scan set_param failed: %d", param.status);
  }
}

void ESP32BLETracker::gap_scan_start_complete(const esp_ble_gap_cb_param_t::ble_scan_start_cmpl_evt_param &param) {
  if (param.status != ESP_BT_STATUS_SUCCESS) {
    ESP_LOGE(TAG, "Scan start failed: %d", param.status);
  }
}

void ESP32BLETracker::gap_scan_result(const esp_ble_gap_cb_param_t::ble_scan_result_evt_param &param) {
  if (param.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
    ESPBTDevice device;
    device.parse_scan_rst(param);

    this->parse_rssi_sensors_(device);
    this->parse_mijia_sensors_(device);
    this->parse_miflora_sensors_(device);

    if (this->parse_already_discovered_(device))
      return;
    this->parse_presence_sensors_(device);
  } else if (param.search_evt == ESP_GAP_SEARCH_INQ_CMPL_EVT) {
    ESP_LOGD(TAG, "Scan complete.");
    xSemaphoreGive(semaphore_scan_end);
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
  for (auto *dev : this->rssi_sensors_) {
    if (dev->address_ == address)
      dev->push_new_value(device.get_rssi());
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
  switch (data_type & 0x0F) {
    case 0x0D: { // temperature+humidity, 4 bytes, 16-bit signed integer (LE) each, 0.1 °C, 0.1 %
      if (data_length != 4)
        return XIAOMI_NO_DATA;
      const int16_t temperature = uint16_t(data[0]) | (uint16_t(data[1]) << 8);
      const int16_t humidity = uint16_t(data[2]) | (uint16_t(data[3]) << 8);
      *data1 = temperature / 10.0f;
      *data2 = humidity / 10.0f;
      return XIAOMI_TEMPERATURE_HUMIDITY;
    }
    case 0x0A: { // battery, 1 byte, 8-bit unsigned integer, 1 %
      if (data_length != 1)
        return XIAOMI_NO_DATA;
      *data1 = data[0];
      return XIAOMI_BATTERY_LEVEL;
    }
    case 0x06: { // humidity, 2 bytes, 16-bit signed integer (LE), 0.1 %
      if (data_length != 2)
        return XIAOMI_NO_DATA;
      const int16_t humidity = uint16_t(data[0]) | (uint16_t(data[1]) << 8);
      *data1 = humidity / 10.0f;
      return XIAOMI_HUMIDITY;
    }
    case 0x04: { // temperature, 2 bytes, 16-bit signed integer (LE), 0.1 °C
      if (data_length != 2)
        return XIAOMI_NO_DATA;
      const int16_t temperature = uint16_t(data[0]) | (uint16_t(data[1]) << 8);
      *data1 = temperature / 10.0f;
      return XIAOMI_TEMPERATURE;
    }
    case 0x09: { // conductivity, 2 bytes, 16-bit unsigned integer (LE), 1 µS/cm
      if (data_length != 2)
        return XIAOMI_NO_DATA;
      const uint16_t conductivity = uint16_t(data[0]) | (uint16_t(data[1]) << 8);
      *data1 = conductivity;
      return XIAOMI_CONDUCTIVITY;
    }
    case 0x07: { // illuminance, 4 bytes, 32-bit unsigned integer (LE), 1 lx
      if (data_length != 4)
        return XIAOMI_NO_DATA;
      const uint32_t illuminance =
          uint32_t(data[0]) | (uint32_t(data[1]) << 8) | (uint32_t(data[2]) << 16) | (uint32_t(data[3]) << 24);
      *data1 = illuminance;
      return XIAOMI_ILLUMINANCE;
    }
    case 0x08: { // soil moisture, 1 byte, 8-bit unsigned integer, 1 %
      if (data_length != 1)
        return XIAOMI_NO_DATA;
      *data1 = data[0];
      return XIAOMI_MOISTURE;
    }
    default:return XIAOMI_NO_DATA;
  }
}

void ESP32BLETracker::parse_mijia_sensors_(const ESPBTDevice &device) {
  const uint64_t address = device.address_uint64();

  if (!device.get_service_data_uuid().has_value()) {
    ESP_LOGVV(TAG, "Xiaomi MiJia no service data");
    return;
  }

  if (!device.get_service_data_uuid()->contains(0x95, 0xFE)) {
    ESP_LOGVV(TAG, "Xiaomi MiJia no service data UUID magic bytes");
    return;
  }

  const auto *raw = reinterpret_cast<const uint8_t *>(device.get_service_data().data());

  if (device.get_service_data().size() < 14 ||
      (raw[1] & 0x20) != 0x20 || raw[2] != 0xAA || raw[3] != 0x01) {
    ESP_LOGVV(TAG, "Xiaomi MiJia no magic bytes");
    return;
  }

  const uint8_t raw_type = raw[11];
  const uint8_t data_length = raw[13];
  const uint8_t *data = &raw[14];
  if (data_length + 14 != device.get_service_data().size()) {
    ESP_LOGW(TAG, "Xiaomi MiJia data length mismatch");
    return;
  }
  float data1, data2;
  XiaomiDataType data_type = parse_xiaomi(raw_type, data, data_length, &data1, &data2);
  switch (data_type) {
    case XIAOMI_TEMPERATURE_HUMIDITY:ESP_LOGD(TAG, "Xiaomi MiJia %s Got temperature=%.1f°C, humidity=%.1f%%",
                                              device.address_str().c_str(), data1, data2);
      break;
    case XIAOMI_TEMPERATURE:ESP_LOGD(TAG, "Xiaomi MiJia %s Got temperature=%.1f°C",
                                     device.address_str().c_str(), data1);
      break;
    case XIAOMI_HUMIDITY:ESP_LOGD(TAG, "Xiaomi MiJia %s Got humidity=%.1f%%",
                                  device.address_str().c_str(), data1);
      break;
    case XIAOMI_BATTERY_LEVEL:ESP_LOGD(TAG, "Xiaomi MiJia %s Got battery level=%.0f%%",
                                       device.address_str().c_str(), data1);
      break;
    case XIAOMI_NO_DATA:
    default:return;
  }

  for (auto *dev : this->mijia_sensors_) {
    if (dev->address_ == address) {
      switch (data_type) {
        case XIAOMI_TEMPERATURE_HUMIDITY:dev->get_temperature_sensor()->push_new_value(data1);
          dev->get_humidity_sensor()->push_new_value(data2);
          break;
        case XIAOMI_TEMPERATURE:dev->get_temperature_sensor()->push_new_value(data1);
          break;
        case XIAOMI_HUMIDITY:dev->get_humidity_sensor()->push_new_value(data1);
          break;
        case XIAOMI_BATTERY_LEVEL:
          if (dev->get_battery_level_sensor() != nullptr)
            dev->get_battery_level_sensor()->push_new_value(data1);
          break;
        default:
          break;
      }
    }
  }
}

void ESP32BLETracker::parse_miflora_sensors_(const ESPBTDevice &device) {

  const uint64_t address = device.address_uint64();

  if (!device.get_service_data_uuid().has_value()) {
    ESP_LOGVV(TAG, "Xiaomi MiFlora no service data");
    return;
  }

  if (!device.get_service_data_uuid()->contains(0x95, 0xFE)) {
    ESP_LOGVV(TAG, "Xiaomi MiFlora no service data UUID magic bytes");
    return;
  }

  const auto *raw = reinterpret_cast<const uint8_t *>(device.get_service_data().data());

  if (device.get_service_data().size() < 14 ||
      (raw[1] & 0x20) != 0x20 || raw[2] != 0x98 || raw[3] != 0x00) {
    ESP_LOGVV(TAG, "Xiaomi MiFlora no magic bytes");
    return;
  }

  const uint8_t raw_type = raw[12];
  const uint8_t data_length = raw[14];
  const uint8_t *data = &raw[15];
  if (data_length + 15 != device.get_service_data().size()) {
    ESP_LOGW(TAG, "Xiaomi MiFlora data length mismatch");
    return;
  }
  float data1, data2;
  XiaomiDataType data_type = parse_xiaomi(raw_type, data, data_length, &data1, &data2);
  switch (data_type) {
    case XIAOMI_TEMPERATURE:ESP_LOGD(TAG, "Xiaomi MiFlora %s Got temperature=%.1f°C",
                                     device.address_str().c_str(), data1);
      break;
    case XIAOMI_MOISTURE:ESP_LOGD(TAG, "Xiaomi MiFlora %s Got moisture=%.0f%%",
                                  device.address_str().c_str(), data1);
      break;
    case XIAOMI_BATTERY_LEVEL:ESP_LOGD(TAG, "Xiaomi MiFlora %s Got battery level=%.0f%%",
                                       device.address_str().c_str(), data1);
      break;
    case XIAOMI_ILLUMINANCE:ESP_LOGD(TAG, "Xiaomi MiFlora %s Got illuminance=%.0flx",
                                     device.address_str().c_str(), data1);
      break;
    case XIAOMI_CONDUCTIVITY:ESP_LOGD(TAG, "Xiaomi MiFlora %s Got soil conductivity=%.0fµS/cm",
                                      device.address_str().c_str(), data1);
      break;
    case XIAOMI_NO_DATA:
    default:return;
  }

  for (auto *dev : this->miflora_sensors_) {
    if (dev->address_ == address) {
      switch (data_type) {
        case XIAOMI_TEMPERATURE:
          if (dev->get_temperature_sensor() != nullptr)
            dev->get_temperature_sensor()->push_new_value(data1);
          break;
        case XIAOMI_MOISTURE:
          if (dev->get_moisture_sensor() != nullptr)
            dev->get_moisture_sensor()->push_new_value(data1);
          break;
        case XIAOMI_BATTERY_LEVEL:
          if (dev->get_battery_level_sensor() != nullptr)
            dev->get_battery_level_sensor()->push_new_value(data1);
          break;
        case XIAOMI_ILLUMINANCE:
          if (dev->get_illuminance_sensor() != nullptr)
            dev->get_illuminance_sensor()->push_new_value(data1);
          break;
        case XIAOMI_CONDUCTIVITY:
          if (dev->get_conductivity_sensor() != nullptr)
            dev->get_conductivity_sensor()->push_new_value(data1);
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

#ifdef ESPHOMELIB_LOG_HAS_DEBUG
  ESP_LOGD(TAG, "Found device %s RSSI=%d", device.address_str().c_str(), device.get_rssi());

  const char *address_type_s;
  switch (device.get_address_type()) {
    case BLE_ADDR_TYPE_PUBLIC: address_type_s = "PUBLIC";
      break;
    case BLE_ADDR_TYPE_RANDOM: address_type_s = "RANDOM";
      break;
    case BLE_ADDR_TYPE_RPA_PUBLIC: address_type_s = "RPA_PUBLIC";
      break;
    case BLE_ADDR_TYPE_RPA_RANDOM: address_type_s = "RPA_RANDOM";
      break;
    default: address_type_s = "UNKNOWN";
      break;
  }

  ESP_LOGD(TAG, "    Address Type: %s", address_type_s);
  ESP_LOGD(TAG, "    Name: '%s'", device.get_name().c_str());
  if (device.get_tx_power().has_value()) {
    ESP_LOGD(TAG, "    TX Power: %d", *device.get_tx_power());
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

void ESPBTDevice::parse_scan_rst(const esp_ble_gap_cb_param_t::ble_scan_result_evt_param &param) {
  for (uint8_t i = 0; i < ESP_BD_ADDR_LEN; i++)
    this->address_[i] = param.bda[i];
  this->address_type_ = param.ble_addr_type;
  this->rssi_ = param.rssi;
  this->parse_adv(param);
}
void ESPBTDevice::parse_adv(const esp_ble_gap_cb_param_t::ble_scan_result_evt_param &param) {
  size_t offset = 0;
  const uint8_t *payload = param.ble_adv;
  uint8_t len = param.adv_data_len;

#ifdef ESPHOMELIB_LOG_HAS_VERY_VERBOSE
  char buffer[200];
  size_t off = 0;
  for (uint8_t i = 0; i < len; i++) {
    int ret = snprintf(buffer + off, sizeof(buffer) - off, "%02X.", param.ble_adv[i]);
    if (ret < 0) {
      break;
    }
    off += ret;
  }
  ESP_LOGVV(TAG, "Adv data: %s (%u bytes)", buffer, len);
#endif

  while (offset + 2 < len) {
    const uint8_t field_length = payload[offset++]; // First byte is length of adv record
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
          this->service_data_ = std::string(reinterpret_cast<const char *>(record + 2), record_length - 2);
        break;
      }
      case ESP_BLE_AD_TYPE_32SERVICE_DATA: {
        if (record_length < 4) {
          ESP_LOGV(TAG, "Record length too small for ESP_BLE_AD_TYPE_32SERVICE_DATA");
          break;
        }
        this->service_data_uuid_ = ESPBTUUID::from_uint32(*reinterpret_cast<const uint32_t *>(record));
        if (record_length > 4)
          this->service_data_ = std::string(reinterpret_cast<const char *>(record + 4), record_length - 4);
        break;
      }
      case ESP_BLE_AD_TYPE_128SERVICE_DATA: {
        if (record_length < 16) {
          ESP_LOGV(TAG, "Record length too small for ESP_BLE_AD_TYPE_128SERVICE_DATA");
          break;
        }
        this->service_data_uuid_ = ESPBTUUID::from_raw(record);
        if (record_length > 16)
          this->service_data_ = std::string(reinterpret_cast<const char *>(record + 16), record_length - 16);
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
  snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
           this->address_[0], this->address_[1], this->address_[2],
           this->address_[3], this->address_[4], this->address_[5]);
  return mac;
}
uint64_t ESPBTDevice::address_uint64() const {
  return ble_addr_to_uint64(this->address_);
}
esp_ble_addr_type_t ESPBTDevice::get_address_type() const {
  return this->address_type_;
}
int ESPBTDevice::get_rssi() const {
  return this->rssi_;
}
const std::string &ESPBTDevice::get_name() const {
  return this->name_;
}
const optional<int8_t> &ESPBTDevice::get_tx_power() const {
  return this->tx_power_;
}
const optional<uint16_t> &ESPBTDevice::get_appearance() const {
  return this->appearance_;
}
const optional<uint8_t> &ESPBTDevice::get_ad_flag() const {
  return this->ad_flag_;
}
const std::vector<ESPBTUUID> &ESPBTDevice::get_service_uuids() const {
  return this->service_uuids_;
}
const std::string &ESPBTDevice::get_manufacturer_data() const {
  return this->manufacturer_data_;
}
const std::string &ESPBTDevice::get_service_data() const {
  return this->service_data_;
}
const optional<ESPBTUUID> &ESPBTDevice::get_service_data_uuid() const {
  return this->service_data_uuid_;
}

void ESP32BLETracker::set_scan_interval(uint32_t scan_interval) {
  this->scan_interval_ = scan_interval;
}
uint32_t ESP32BLETracker::get_scan_interval() const {
  return scan_interval_;
}

XiaomiMiFloraTemperatureSensor *XiaomiMiFloraDevice::get_temperature_sensor() const {
  return this->temperature_sensor_;
}
XiaomiMiFloraMoistureSensor *XiaomiMiFloraDevice::get_moisture_sensor() const {
  return this->moisture_sensor_;
}
XiaomiMiFloraIlluminanceSensor *XiaomiMiFloraDevice::get_illuminance_sensor() const {
  return this->illuminance_sensor_;
}
XiaomiMiFloraConductivitySensor *XiaomiMiFloraDevice::get_conductivity_sensor() const {
  return this->conductivity_sensor_;
}
XiaomiMiFloraBatteryLevelSensor *XiaomiMiFloraDevice::get_battery_level_sensor() const {
  return this->battery_level_sensor_;
}
XiaomiMiFloraTemperatureSensor *XiaomiMiFloraDevice::make_temperature_sensor(const std::string &name) {
  return this->temperature_sensor_ = new XiaomiMiFloraTemperatureSensor(name);
}
XiaomiMiFloraMoistureSensor *XiaomiMiFloraDevice::make_moisture_sensor(const std::string &name) {
  return this->moisture_sensor_ = new XiaomiMiFloraMoistureSensor(name);
}
XiaomiMiFloraIlluminanceSensor *XiaomiMiFloraDevice::make_illuminance_sensor(const std::string &name) {
  return this->illuminance_sensor_ = new XiaomiMiFloraIlluminanceSensor(name);
}
XiaomiMiFloraConductivitySensor *XiaomiMiFloraDevice::make_conductivity_sensor(const std::string &name) {
  return this->conductivity_sensor_ = new XiaomiMiFloraConductivitySensor(name);
}
XiaomiMiJiaBatteryLevelSensor *XiaomiMiFloraDevice::make_battery_level_sensor(const std::string &name) {
  return this->battery_level_sensor_ = new XiaomiMiFloraBatteryLevelSensor(name);
}
std::string ESP32BLERSSISensor::unit_of_measurement() {
  return "dB";
}

std::string ESP32BLERSSISensor::icon() {
  return "mdi:signal";
}
int8_t ESP32BLERSSISensor::accuracy_decimals() {
  return Sensor::accuracy_decimals();
}
std::string ESP32BLERSSISensor::unique_id() {
  char buffer[32];
  sprintf(buffer, "ble-%08X%08X-rssi", uint32_t(this->address_ >> 32), uint32_t(this->address_));
  return buffer;
}
uint32_t ESP32BLERSSISensor::update_interval() {
  return this->parent_->get_scan_interval() * 1000u;
}
ESP32BLERSSISensor::ESP32BLERSSISensor(ESP32BLETracker *parent, const std::string &name, uint64_t address)
    : Sensor(name), parent_(parent), address_(address) {

}

XiaomiMiJiaDevice::XiaomiMiJiaDevice(uint64_t address,
                                     const std::string &temperature_name,
                                     const std::string &humidity_name)
    : address_(address), temperature_sensor_(new XiaomiMiJiaTemperatureSensor(temperature_name)),
      humidity_sensor_(new XiaomiMiJiaHumiditySensor(humidity_name)) {

}
XiaomiMiJiaTemperatureSensor *XiaomiMiJiaDevice::get_temperature_sensor() const {
  return this->temperature_sensor_;
}
XiaomiMiJiaHumiditySensor *XiaomiMiJiaDevice::get_humidity_sensor() const {
  return this->humidity_sensor_;
}
XiaomiMiJiaBatteryLevelSensor *XiaomiMiJiaDevice::make_battery_level_sensor(const std::string &name) {
  return this->battery_level_sensor_ = new XiaomiMiJiaBatteryLevelSensor(name);
}
XiaomiMiJiaBatteryLevelSensor *XiaomiMiJiaDevice::get_battery_level_sensor() const {
  return this->battery_level_sensor_;
}

ESP32BLEPresenceDevice::ESP32BLEPresenceDevice(const std::string &name, uint64_t address)
    : BinarySensor(name), address_(address) {

}
std::string ESP32BLEPresenceDevice::device_class() {
  return "presence";
}

ESPHOMELIB_NAMESPACE_END

#endif //USE_ESP32_BLE_TRACKER
