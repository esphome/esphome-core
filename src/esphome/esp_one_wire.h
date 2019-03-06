#ifndef ESPHOME_ONE_WIRE_H
#define ESPHOME_ONE_WIRE_H

#include "esphome/defines.h"

#ifdef USE_ONE_WIRE

#include "esphome/esphal.h"
#include <vector>

ESPHOME_NAMESPACE_BEGIN

extern const uint8_t ONE_WIRE_ROM_SELECT;
extern const int ONE_WIRE_ROM_SEARCH;

/** This is ESPHome's own (minimal) implementation of 1-Wire that improves timing for ESP boards
 *
 * It's more or less the same as Arduino's internal library but uses some fancy C++ and 64 bit
 * unsigned integers to make our lives easier.
 */
class ESPOneWire {
 public:
  /// Construct a OneWire instance for the specified pin. There should only exist one instance per pin.
  explicit ESPOneWire(GPIOPin *pin);

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

  GPIOPin *get_pin();

 protected:
  /// Helper to get the internal 64-bit unsigned rom number as a 8-bit integer pointer.
  inline uint8_t *rom_number8_();

  GPIOPin *pin_;
  uint8_t last_discrepancy_{0};
  uint8_t last_family_discrepancy_{0};
  bool last_device_flag_{false};
  uint64_t rom_number_{0};
};

ESPHOME_NAMESPACE_END

#endif  // USE_ONE_WIRE

#endif  // ESPHOME_ONE_WIRE_H
