#ifndef ESPHOME_ESP32_BLE_TRACKER_H
#define ESPHOME_ESP32_BLE_TRACKER_H

#include "esphome/defines.h"

#ifdef USE_ESP32_BLE_TRACKER

#include "esphome/component.h"
#include "esphome/binary_sensor/binary_sensor.h"
#include "esphome/sensor/sensor.h"

#include <string>
#include <array>
#include <esp_gap_ble_api.h>
#include <esp_bt_defs.h>

ESPHOME_NAMESPACE_BEGIN

class ESP32BLEPresenceDevice;
class ESP32BLERSSISensor;
class XiaomiDevice;
class ESPBTDevice;

/** The ESP32BLETracker class is a hub for all ESP32 Bluetooth Low Energy devices.
 *
 * The implementation uses a lightweight version of the amazing ESP32 BLE Arduino library by
 * Neil Kolban. This was done because the ble library was quite huge and ESPHome would only
 * ever use a small part of it anyway. Still though, when this component is used, the required
 * flash size increases by a lot (about 500kB). As the BLE stack uses humongous amounts of
 * stack size, this component creates a separate FreeRTOS task to handle all BLE stuff with
 * its own stack.
 */
class ESP32BLETracker : public Component {
 public:
  /** Create a simple binary sensor for a MAC address.
   *
   * Each time a BLE scan yields the MAC address in the address parameter, the binary sensor
   * will immediately go to true. When a whole scan interval does not discover a device with this
   * MAC address, the binary sensor will be set to false.
   *
   * The address parameter accepts a MAC address as an array of length 6 with each MAC address
   * part in an unsigned int. To set it up, do something like this:
   *
   * ```cpp
   * // MAC address AC:37:43:77:5F:4C
   * App.register_binary_sensor(tracker->make_device("ESP32 BLE Device", {
   *    0xAC, 0x37, 0x43, 0x77, 0x5F, 0x4C
   * }));
   * ```
   *
   * You can get this MAC address by navigating to the bluetooth screen of your device and looking for
   * an advanced settings screen, you can usually find the MAC address there. Alternatively, you can
   * set the global debug level to DEBUG and observe the logs. Then you will find messages like this:
   *
   * ```
   * Found device AC:37:43:77:5F:4C RSSI=-80
   *     Address Type: PUBLIC
   *     Name: 'Google Home Mini'
   * ```
   *
   * If the Address Type shown in the logs is `RANDOM`, you unfortunately can't track that device at
   * the moment as the device constantly changes its MAC address as a "security" feature.
   *
   * @see set_scan_interval
   * @param name The name of the binary sensor to create.
   * @param address The MAC address to match.
   * @return
   */
  ESP32BLEPresenceDevice *make_presence_sensor(const std::string &name, std::array<uint8_t, 6> address);

  ESP32BLERSSISensor *make_rssi_sensor(const std::string &name, std::array<uint8_t, 6> address);

  XiaomiDevice *make_xiaomi_device(std::array<uint8_t, 6> address);

  /** Set the number of seconds (!) that a single BLE scan should take.
   *
   * This parameter is useful when adjusting how long it should take for a bluetooth device
   * to be marked as disconnected.
   *
   * @param scan_interval The interval in seconds to reset the scan.
   */
  void set_scan_interval(uint32_t scan_interval);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Setup the FreeRTOS task and the Bluetooth stack.
  void setup() override;
  void dump_config() override;

  void loop() override;

  uint32_t get_scan_interval() const;

 protected:
  /// The FreeRTOS task managing the bluetooth interface.
  static bool ble_setup();
  /// Start a single scan by setting up the parameters and doing some esp-idf calls.
  void start_scan(bool first);
  /// Callback that will handle all GAP events and redistribute them to other callbacks.
  static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
  /// Called when a `ESP_GAP_BLE_SCAN_RESULT_EVT` event is received.
  void gap_scan_result(const esp_ble_gap_cb_param_t::ble_scan_result_evt_param &param);
  /// Called when a `ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT` event is received.
  void gap_scan_set_param_complete(const esp_ble_gap_cb_param_t::ble_scan_param_cmpl_evt_param &param);
  /// Called when a `ESP_GAP_BLE_SCAN_START_COMPLETE_EVT` event is received.
  void gap_scan_start_complete(const esp_ble_gap_cb_param_t::ble_scan_start_cmpl_evt_param &param);

  void parse_presence_sensors_(const ESPBTDevice &device);
  void parse_rssi_sensors_(const ESPBTDevice &device);
  void parse_xiaomi_sensors_(const ESPBTDevice &device);

  bool parse_already_discovered_(const ESPBTDevice &device);
  bool has_already_discovered_(uint64_t address);

  /// An array of MAC addresses discovered during this scan. Used to mark registered devices as undiscovered.
  std::vector<uint64_t> already_discovered_;

  /// An array of registered devices to track
  std::vector<ESP32BLEPresenceDevice *> presence_sensors_;
  std::vector<ESP32BLERSSISensor *> rssi_sensors_;
  std::vector<XiaomiDevice *> xiaomi_devices_;
  /// A structure holding the ESP BLE scan parameters.
  esp_ble_scan_params_t scan_params_;
  /// The interval in seconds to perform scans.
  uint32_t scan_interval_{300};
  SemaphoreHandle_t scan_result_lock_;
  SemaphoreHandle_t scan_end_lock_;
  size_t scan_result_index_{0};
  esp_ble_gap_cb_param_t::ble_scan_result_evt_param scan_result_buffer_[16];
  esp_bt_status_t scan_start_failed_{ESP_BT_STATUS_SUCCESS};
  esp_bt_status_t scan_set_param_failed_{ESP_BT_STATUS_SUCCESS};
};

/// Simple helper class to expose an BLE device as a binary sensor.
class ESP32BLEPresenceDevice : public binary_sensor::BinarySensor {
 public:
  ESP32BLEPresenceDevice(const std::string &name, uint64_t address);

 protected:
  friend ESP32BLETracker;

  std::string device_class() override;

  uint64_t address_;
};

class ESP32BLERSSISensor : public sensor::Sensor {
 public:
  ESP32BLERSSISensor(ESP32BLETracker *parent, const std::string &name, uint64_t address);

  std::string unit_of_measurement() override;
  std::string icon() override;
  int8_t accuracy_decimals() override;
  std::string unique_id() override;
  uint32_t update_interval() override;

 protected:
  friend ESP32BLETracker;

  uint64_t address_;
  ESP32BLETracker *parent_;
};

class XiaomiSensor : public sensor::Sensor {
 public:
  enum Type {
    TYPE_TEMPERATURE = 0,
    TYPE_HUMIDITY,
    TYPE_MOISTURE,
    TYPE_ILLUMINANCE,
    TYPE_CONDUCTIVITY,
    TYPE_BATTERY_LEVEL,
  };

  XiaomiSensor(XiaomiDevice *parent, Type type, const std::string &name);

  std::string unit_of_measurement() override;
  std::string icon() override;
  uint32_t update_interval() override;
  int8_t accuracy_decimals() override;
  std::string unique_id() override;

 protected:
  XiaomiDevice *parent_;
  Type type_;
};

class XiaomiDevice {
 public:
  XiaomiDevice(ESP32BLETracker *parent, uint64_t address);

  XiaomiSensor *get_temperature_sensor() const;
  XiaomiSensor *get_humidity_sensor() const;
  XiaomiSensor *get_moisture_sensor() const;
  XiaomiSensor *get_illuminance_sensor() const;
  XiaomiSensor *get_conductivity_sensor() const;
  XiaomiSensor *get_battery_level_sensor() const;
  XiaomiSensor *make_temperature_sensor(const std::string &name);
  XiaomiSensor *make_humidity_sensor(const std::string &name);
  XiaomiSensor *make_moisture_sensor(const std::string &name);
  XiaomiSensor *make_illuminance_sensor(const std::string &name);
  XiaomiSensor *make_conductivity_sensor(const std::string &name);
  XiaomiSensor *make_battery_level_sensor(const std::string &name);

  uint32_t update_interval() const;

  std::string unique_id() const;

 protected:
  friend ESP32BLETracker;

  ESP32BLETracker *parent_;
  uint64_t address_;
  XiaomiSensor *temperature_sensor_{nullptr};
  XiaomiSensor *humidity_sensor_{nullptr};
  XiaomiSensor *moisture_sensor_{nullptr};
  XiaomiSensor *illuminance_sensor_{nullptr};
  XiaomiSensor *conductivity_sensor_{nullptr};
  XiaomiSensor *battery_level_sensor_{nullptr};
};

class ESPBTUUID {
 public:
  ESPBTUUID();

  static ESPBTUUID from_uint16(uint16_t uuid);

  static ESPBTUUID from_uint32(uint32_t uuid);

  static ESPBTUUID from_raw(const uint8_t *data);

  bool contains(uint8_t data1, uint8_t data2) const;

  std::string to_string();

 protected:
  esp_bt_uuid_t uuid_;
};

class ESPBTDevice {
 public:
  void parse_scan_rst(const esp_ble_gap_cb_param_t::ble_scan_result_evt_param &param);

  void parse_adv(const esp_ble_gap_cb_param_t::ble_scan_result_evt_param &param);

  std::string address_str() const;

  uint64_t address_uint64() const;

  esp_ble_addr_type_t get_address_type() const;
  int get_rssi() const;
  const std::string &get_name() const;
  const optional<int8_t> &get_tx_power() const;
  const optional<uint16_t> &get_appearance() const;
  const optional<uint8_t> &get_ad_flag() const;
  const std::vector<ESPBTUUID> &get_service_uuids() const;
  const std::string &get_manufacturer_data() const;
  const std::string &get_service_data() const;
  const optional<ESPBTUUID> &get_service_data_uuid() const;

 protected:
  esp_bd_addr_t address_{
      0,
  };
  esp_ble_addr_type_t address_type_{BLE_ADDR_TYPE_PUBLIC};
  int rssi_{0};
  std::string name_{};
  optional<int8_t> tx_power_{};
  optional<uint16_t> appearance_{};
  optional<uint8_t> ad_flag_{};
  std::vector<ESPBTUUID> service_uuids_;
  std::string manufacturer_data_{};
  std::string service_data_{};
  optional<ESPBTUUID> service_data_uuid_{};
};

extern ESP32BLETracker *global_esp32_ble_tracker;

ESPHOME_NAMESPACE_END

#endif  // USE_ESP32_BLE_TRACKER

#endif  // ESPHOME_ESP32_BLE_TRACKER_H
