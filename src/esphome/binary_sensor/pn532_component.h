#ifndef ESPHOME_PN_532_COMPONENT_H
#define ESPHOME_PN_532_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_PN532

#include <vector>
#include "esphome/component.h"
#include "esphome/binary_sensor/binary_sensor.h"
#include "esphome/spi_component.h"

ESPHOME_NAMESPACE_BEGIN

namespace binary_sensor {

class PN532BinarySensor;
class PN532Trigger;

class PN532Component : public PollingComponent, public SPIDevice {
 public:
  PN532Component(SPIComponent *parent, GPIOPin *cs, uint32_t update_interval = 1000);

  void setup() override;

  void dump_config() override;

  void update() override;
  float get_setup_priority() const override;

  void loop() override;

  PN532BinarySensor *make_tag(const std::string &name, const std::vector<uint8_t> &uid);
  PN532Trigger *make_trigger();

 protected:
  bool is_device_msb_first() override;

  /// Write the full command given in data to the PN532
  void pn532_write_command_(const std::vector<uint8_t> &data);
  bool pn532_write_command_check_ack_(const std::vector<uint8_t> &data);

  /** Read a data frame from the PN532 and return the result as a vector.
   *
   * Note that is_ready needs to be checked first before requesting this method.
   *
   * On failure, an empty vector is returned.
   */
  std::vector<uint8_t> pn532_read_data_();

  /** Checks if the PN532 has set its ready status flag.
   *
   * Procedure goes as follows:
   * - Host sends command to PN532 "write data"
   * - Wait for readiness (until PN532 has processed command) by polling "read status"/is_ready_
   * - Parse ACK/NACK frame with "read data" byte
   *
   * - If data required, wait until device reports readiness again
   * - Then call "read data" and read certain number of bytes (length is given at offset 4 of frame)
   */
  bool is_ready_();
  bool wait_ready_();

  bool read_ack_();

  bool requested_read_{false};
  std::vector<PN532BinarySensor *> binary_sensors_;
  std::vector<PN532Trigger *> triggers_;
  enum PN532Error {
    NONE = 0,
    WAKEUP_FAILED,
    SAM_COMMAND_FAILED,
  } error_code_{NONE};
};

class PN532BinarySensor : public binary_sensor::BinarySensor {
 public:
  PN532BinarySensor(const std::string &name, const std::vector<uint8_t> &uid, uint32_t update_interval);

  bool process(const uint8_t *data, uint8_t len);

 protected:
  std::vector<uint8_t> uid_;
};

class PN532Trigger : public Trigger<std::string> {
 public:
  void process(const uint8_t *uid, uint8_t uid_length);
};

}  // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PN532

#endif  // ESPHOME_PN_532_COMPONENT_H
