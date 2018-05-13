//
//  esp32_ble_tracker.cpp
//  esphomelib
//
//  Created by Otto Winter on 12.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#include "esphomelib/esp32_ble_tracker.h"

#ifdef USE_ESP32_BLE_TRACKER

#include <nvs_flash.h>
#include <freertos/FreeRTOSConfig.h>
#include <esp_bt_main.h>
#include <esp_bt.h>
#include <freertos/task.h>
#include <esp_gap_ble_api.h>
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

// bt_trace.h
#undef TAG

static const char *TAG = "esp32_ble";

ESP32BLETracker *global_esp32_ble_tracker = nullptr;
SemaphoreHandle_t semaphore_scan_end;

void ESP32BLETracker::setup() {
  global_esp32_ble_tracker = this;

  xTaskCreatePinnedToCore(
      ESP32BLETracker::ble_core_task,
      "ble_task", // name
      10000, // stack size (in words
      nullptr, // input params
      1, // priority
      nullptr, // Handle, not needed
      0
  );
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
    default: break;
  }
}

void ESP32BLETracker::ble_core_task(void *params) {
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
  esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));

  // BLE takes some time to be fully set up, 200ms should be more than enough
  delay(200);

  while (true) {
    global_esp32_ble_tracker->start_scan(true);
    // wait for result
    xSemaphoreTake(semaphore_scan_end, portMAX_DELAY);
    xSemaphoreGive(semaphore_scan_end);
  }
}

std::string ble_extract_name(const uint8_t *payload, uint8_t len) {
  uint8_t i = 0;
  while (i < len) {
    uint8_t field_length = payload[i++];
    if (field_length == 0)  // last field has length 0
      break;

    uint8_t field_type = payload[i++];
    field_length--; // remaining data without adv type

    if (field_type == ESP_BLE_AD_TYPE_NAME_CMPL) {
      return std::string(reinterpret_cast<const char *>(&payload[i]), field_length);
    }

    i += field_length;
  }

  return "";
}

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

void ESP32BLETracker::gap_scan_result(const esp_ble_gap_cb_param_t::ble_scan_result_evt_param &param) {
  if (param.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
    char mac[24];
    snprintf(mac, 24, "%02X:%02X:%02X:%02X:%02X:%02X",
             param.bda[0], param.bda[1], param.bda[2], param.bda[3], param.bda[4], param.bda[5]);

    uint64_t address = ble_addr_to_uint64(param.bda);
    // O(N^2), but won't be called that often anyway and this is probably better with memory allocation
    for (auto addr : this->discovered_) {
      if (addr == address) {
        ESP_LOGV(TAG, "Already discovered device %s", mac);
        return;
      }
    }
    this->discovered_.push_back(address);

    if_debug {
      ESP_LOGD(TAG, "Found device %s RSSI=%d", mac, param.rssi);

      const char *address_type_s;
      switch (param.ble_addr_type) {
        case BLE_ADDR_TYPE_PUBLIC: address_type_s = "PUBLIC"; break;
        case BLE_ADDR_TYPE_RANDOM: address_type_s = "RANDOM"; break;
        case BLE_ADDR_TYPE_RPA_PUBLIC: address_type_s = "RPA_PUBLIC"; break;
        case BLE_ADDR_TYPE_RPA_RANDOM: address_type_s = "RPA_RANDOM"; break;
        default: address_type_s = "UNKNOWN"; break;
      }
      ESP_LOGD(TAG, "    Address Type: %s", address_type_s);

      std::string name = ble_extract_name(param.ble_adv, param.adv_data_len);
      ESP_LOGD(TAG, "    Name: '%s'", name.c_str());
    }

    for (auto *dev : this->devices_) {
      if (dev->address_ == address) {
        dev->publish_state(true);

        // no break here - maybe someone has a use-case where they need several binary sensors with the same
        // address, shouldn't slow things down much anyway.
      }
    }
  } else if (param.search_evt == ESP_GAP_SEARCH_INQ_CMPL_EVT) {
    ESP_LOGD(TAG, "Scan complete.");
    xSemaphoreGive(semaphore_scan_end);
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
void ESP32BLETracker::start_scan(bool first) {
  xSemaphoreTake(semaphore_scan_end, portMAX_DELAY);

  ESP_LOGD(TAG, "Starting scan...");
  if (!first) {
    // also O(N^2), but will only be called once every minute or so
    for (auto *device : this->devices_) {
      bool found = false;
      for (auto discover : this->discovered_) {
        if (discover == device->address_) {
          found = true;
          break;
        }
      }
      if (!found) {
        device->publish_state(false);
      }
    }
  }
  this->discovered_.clear();
  this->scan_params_.scan_type = BLE_SCAN_TYPE_ACTIVE;
  this->scan_params_.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
  this->scan_params_.scan_filter_policy = BLE_SCAN_FILTER_ALLOW_UND_RPA_DIR;
  this->scan_params_.scan_interval = uint16_t(100 / 0.625f);
  this->scan_params_.scan_window = uint16_t(100 / 0.625f);

  esp_ble_gap_set_scan_params(&this->scan_params_);
  esp_ble_gap_start_scanning(this->scan_interval_);
}
ESP32BLEDevice *ESP32BLETracker::make_device(const std::string &name, std::array<uint8_t, 6> address) {
  uint64_t addr = ble_addr_to_uint64(address.cbegin());
  auto *dev = new ESP32BLEDevice(name, addr);
  this->devices_.push_back(dev);
  return dev;
}
void ESP32BLETracker::set_scan_interval(uint32_t scan_interval) {
  this->scan_interval_ = scan_interval;
}

ESP32BLEDevice::ESP32BLEDevice(const std::string &name, uint64_t address)
    : BinarySensor(name), address_(address) {

}

ESPHOMELIB_NAMESPACE_END

#endif //USE_ESP32_BLE_TRACKER
