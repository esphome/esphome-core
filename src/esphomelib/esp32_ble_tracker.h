//
//  esp32_ble_tracker.h
//  esphomelib
//
//  Created by Otto Winter on 12.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_ESP32_BLE_TRACKER_H
#define ESPHOMELIB_ESP32_BLE_TRACKER_H

#include "esphomelib/component.h"
#include "esphomelib/binary_sensor/binary_sensor.h"
#include "esphomelib/defines.h"

#ifdef USE_ESP32_BLE_TRACKER

#include <string>
#include <array>
#include <esp_gap_ble_api.h>

namespace esphomelib {

class ESP32BLEDevice;

/** The ESP32BLETracker class is a hub for all ESP32 Bluetooth Low Energy devices.
 *
 * This implementation is currently only quite basic and only supports using scan to
 * see if BLE beacons can be found. In the future, this class will support exposing the RSSI
 * service data and some GATTC standardized information like temperature.
 *
 * The implementation uses a lightweight version of the amazing ESP32 BLE Arduino library by
 * Neil Kolban. This was done because the ble library was quite huge and esphomelib would only
 * ever use a small part of it anyway. Still though, when this component is used, the required
 * flash size increases by a lot (about 500kB). As the BLE stack uses humongous amounts of
 * stack size, this component creates a separate FreeRTOS task to handle all BLE stuff with
 * its own stack.
 *
 * Also, currently only BLE beacons seem to be picked up by this component. In the future I will
 * try to also make this work with standard BLE devices (like smartwatches, phones, ...), but that
 * will require a bit of reading up on the (very complicated) Bluetooth standard and might take a while.
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
   * If the Address Type shown in the logs is "RANDOM", you unfortunately can't track that device at
   * the moment as the device constantly changes its MAC address as a "security" feature.
   *
   * @see set_scan_interval
   * @param name The name of the binary sensor to create.
   * @param address The MAC address to match.
   * @return
   */
  ESP32BLEDevice *make_device(const std::string &name, std::array<uint8_t, 6> address);

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

 protected:
  /// The FreeRTOS task managing the bluetooth interface.
  static void ble_core_task(void *params);
  /// Start a single scan by setting up the parameters and doing some esp-idf calls.
  void start_scan(bool first);
  /// Callback that will handle all GAP events and redistribute them to other callbacks.
  static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
  /// Called when a ESP_GAP_BLE_SCAN_RESULT_EVT event is received.
  void gap_scan_result(const esp_ble_gap_cb_param_t::ble_scan_result_evt_param &param);
  /// Called when a ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT event is received.
  void gap_scan_set_param_complete(const esp_ble_gap_cb_param_t::ble_scan_param_cmpl_evt_param &param);
  /// Called when a ESP_GAP_BLE_SCAN_START_COMPLETE_EVT event is received.
  void gap_scan_start_complete(const esp_ble_gap_cb_param_t::ble_scan_start_cmpl_evt_param &param);

  /// An array of registered devices to track
  std::vector<ESP32BLEDevice *> devices_;
  /// An array of MAC addresses discovered during this scan. Used to mark registered devices as undiscovered.
  std::vector<uint64_t> discovered_;
  /// A structure holding the ESP BLE scan parameters.
  esp_ble_scan_params_t scan_params_;
  /// The interval in seconds to perform scans.
  uint32_t scan_interval_{300};
};

/// Simple helper class to expose an BLE device as a binary sensor.
class ESP32BLEDevice : public binary_sensor::BinarySensor {
 public:
  ESP32BLEDevice(const std::string &name, uint64_t address);

 protected:
  friend ESP32BLETracker;

  uint64_t address_;
};

extern ESP32BLETracker *global_esp32_ble_tracker;
extern SemaphoreHandle_t semaphore_scan_end;

} // namespace esphomelib

#endif //USE_ESP32_BLE_TRACKER

#endif //ESPHOMELIB_ESP32_BLE_TRACKER_H
