#ifndef ESPHOME_ESP_32_BLE_BEACON_H
#define ESPHOME_ESP_32_BLE_BEACON_H

#include "esphome/defines.h"

#ifdef USE_ESP32_BLE_BEACON

#include "esphome/component.h"
#include <esp_gap_ble_api.h>

ESPHOME_NAMESPACE_BEGIN

typedef struct {
  uint8_t flags[3];
  uint8_t length;
  uint8_t type;
  uint16_t company_id;
  uint16_t beacon_type;
} __attribute__((packed)) esp_ble_ibeacon_head_t;

typedef struct {
  uint8_t proximity_uuid[16];
  uint16_t major;
  uint16_t minor;
  uint8_t measured_power;
} __attribute__((packed)) esp_ble_ibeacon_vendor_t;

typedef struct {
  esp_ble_ibeacon_head_t ibeacon_head;
  esp_ble_ibeacon_vendor_t ibeacon_vendor;
} __attribute__((packed)) esp_ble_ibeacon_t;

class ESP32BLEBeacon : public Component {
 public:
  explicit ESP32BLEBeacon(const std::array<uint8_t, 16> &uuid);

  void setup() override;
  float get_setup_priority() const override;

  void set_major(uint16_t major);
  void set_minor(uint16_t minor);

 protected:
  static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
  static void ble_core_task(void *params);
  static void ble_setup();

  std::array<uint8_t, 16> uuid;
  uint16_t major{10167};
  uint16_t minor{61958};
};

extern ESP32BLEBeacon *global_esp32_ble_beacon;

ESPHOME_NAMESPACE_END

#endif  // USE_ESP32_BLE_BEACON

#endif  // ESPHOME_ESP_32_BLE_BEACON_H
