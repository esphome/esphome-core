//
//  esp_one_wire.h
//  esphomelib
//
//  Created by Otto Winter on 31.03.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_ONE_WIRE_H
#define ESPHOMELIB_ONE_WIRE_H

#include "esphomelib/esphal.h"

#include <vector>

namespace esphomelib {

const uint8_t ONE_WIRE_ROM_SELECT = 0x55;
const int ONE_WIRE_ROM_SEARCH = 0xF0;

/** This is esphomelib's own (minimal) implementation of 1-Wire that improves timing for ESP boards
 *
 * It's more or less the same as Arduino's internal library but uses some fancy C++ and 64 bit
 * unsigned integers to make our lives easier.
 */
class ESPOneWire {
 public:
  /// Construct a OneWire instance for the specified pin. There should only exist one instance per pin.
  explicit ESPOneWire(uint8_t pin);

  /** Reset the bus, should be done before all write operations.
   *
   * Takes approximately 1ms.
   *
   * @return Whether the operation was successful.
   */
  bool reset();

  /// Write a single bit to the bus, takes about 70µs.
  void write_bit(bool bit);

  /// Read a single bit from the bus, takes about 70µs
  bool read_bit();

  /// Write a word to the bus. LSB first.
  void write8(uint8_t val);

  /// Write a 64 bit unsigned integer to the bus. LSB first.
  void write64(uint64_t val);

  /// Write a command to the bus that addresses all devices by skipping the ROM.
  void skip();

  /// Read an 8 bit word from the bus.
  uint8_t read8();

  /// Read an 64-bit unsigned integer from the bus.
  uint64_t read64();

  /// Select a specific address on the bus for the following command.
  void select(uint64_t address);

  /// Reset the device search.
  void reset_search();

  /// Search for a 1-Wire device on the bus. Returns 0 if all devices have been found.
  uint64_t search();

  /// Helper that wraps search in a std::vector.
  std::vector<uint64_t> search_vec();

  /// Helper to create a CRC8 bit checksum for the specified address and length.
  static uint8_t crc8(uint8_t *addr, uint8_t len);

 protected:
  /// Helper to read the value on the bus.
  inline bool digital_read_() { return digitalRead(this->pin_) == HIGH; }
  /// Helper to pull the bus low/high.
  inline void digital_write_(uint8_t state) { digitalWrite(this->pin_, state); }
  /// Helper to set the pin mode on the bus.
  inline void pin_mode_(uint8_t mode) { pinMode(this->pin_, mode); }
  /// Helper to get the internal 64-bit unsigned rom number as a 8-bit integer pointer.
  inline uint8_t *rom_number8_() { return reinterpret_cast<uint8_t *>(&this->rom_number); }

  uint8_t pin_;
  uint8_t last_discrepancy_{0};
  uint8_t last_family_discrepancy_{0};
  bool last_device_flag_{false};
  uint64_t rom_number{0};

};

} // namespace esphomelib

#endif //ESPHOMELIB_ONE_WIRE_H
