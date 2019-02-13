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
  bool msb_first() override;

  void pn532_write_command_(uint8_t len);
  bool pn532_write_command_check_ack_(uint8_t len, bool ignore = false);

  void pn532_read_data_(uint8_t len);

  bool is_ready();

  bool read_ack();

  uint8_t buffer_[32];
  bool requested_read_{false};
  std::vector<PN532BinarySensor *> binary_sensors_;
  std::vector<PN532Trigger *> triggers_;
  std::vector<uint8_t> last_uid_;
  enum PN532Error {
    NONE = 0,
    WRITING_SAM_COMMAND_FAILED,
    READING_SAM_COMMAND_FAILED,
  } error_code_{NONE};
};

class PN532BinarySensor : public binary_sensor::BinarySensor {
 public:
  PN532BinarySensor(const std::string &name, const std::vector<uint8_t> &uid, uint32_t update_interval);

  bool process(uint8_t *data, uint8_t len);
 protected:
  std::vector<uint8_t> uid_;
};

class PN532Trigger : public Trigger<std::string> {
 public:
  void process(uint8_t *uid, uint8_t uid_length);
};

} // namespace binary_sensor

ESPHOME_NAMESPACE_END

#endif //USE_PN532

#endif //ESPHOME_PN_532_COMPONENT_H
