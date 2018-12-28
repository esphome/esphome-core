#include "esphomelib/defines.h"

#ifdef USE_TX20

#include "esphomelib/espmath.h"
#include "esphomelib/log.h"
#include "esphomelib/sensor/tx20_component.h"
#include "esphomelib/text_sensor/text_sensor.h"

#ifdef ARDUINO_ARCH_ESP32
#define GPIO_STATUS_W1TC_ADDRESS 0x24
#endif

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.tx20";

const uint16_t TX20_BIT_TIME = 1220;   // microseconds
const uint8_t TX20_RESET_VALUES = 60;  // seconds

const std::string DIRECTIONS[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
                                  "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};

bool start_read = false;
uint8_t bit_count = 0;
uint32_t end_time = 0;
uint8_t tx20_data[6] = {0};

TX20Component::TX20Component(const std::string &wind_speed_name, const std::string &wind_direction_name,
                             const std::string &wind_direction_text_name, GPIOPin *pin)
    : Component(),
      wind_speed_sensor_(new TX20WindSpeedSensor(wind_speed_name)),
      wind_direction_sensor_(new TX20WindDirectionSensor(wind_direction_name)),
      wind_direction_text_sensor_(new TX20WindDirectionTextSensor(wind_direction_text_name)),
      pin_(pin) {
  assert(!global_tx20_);
  global_tx20_ = this;
}

void TX20Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TX20...");
  this->pin_->setup();

  attachInterrupt(this->pin_->get_pin(), TX20Component::start_read_, RISING);
}
void TX20Component::dump_config() {
  ESP_LOGCONFIG(TAG, "TX20:");
  LOG_PIN(" Pin:", this->pin_);
}
float TX20Component::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

void TX20Component::loop() {
  if (start_read && (micros() >= end_time)) {
    this->read_loop_();
  }
}
void TX20Component::start_read_() {
  global_tx20_->start_read_internal_();
}

void TX20Component::start_read_internal_() {
  /* La Crosse TX20 Anemometer datagram every 2 seconds
   * 0-0 11011 0011 111010101111 0101 1100 000101010000 0-0
   * Received pin data at 1200 uSec per bit
   * 00100 1100 000101010000 1010 1100 000101010000
   * - data[0] to data[3] inverted user data,
   * - LSB first data[0]
   * - Start frame always 00100 data[1]
   * - Wind direction 0 - 15 sc
   * - Wind speed 0 - 511 data[2]
   * - Checksum data[4]
   * - Wind direction 0 - 15 data[5] - Wind speed 0 - 511
   */
  std::fill(std::begin(tx20_data), std::end(tx20_data), 0);

  start_read = true;
  end_time = micros() + TX20_BIT_TIME / 2;
  bit_count = 41;
}

void TX20Component::read_loop_() {
  if (bit_count > 0) {
    bit_count--;

    uint8_t dpin = this->pin_->digital_read();
    if (bit_count > 41 - 5) {
      // start, inverted
      tx20_data[0] = (tx20_data[0] << 1) | (dpin ^ 1);
    } else if (bit_count > 41 - 5 - 4) {
      // wind dir, inverted
      tx20_data[1] = tx20_data[1] >> 1 | ((dpin ^ 1) << 3);
    } else if (bit_count > 41 - 5 - 4 - 12) {
      // windspeed, inverted
      tx20_data[2] = tx20_data[2] >> 1 | ((dpin ^ 1) << 11);
    } else if (bit_count > 41 - 5 - 4 - 12 - 4) {
      // checksum, inverted
      tx20_data[3] = tx20_data[3] >> 1 | ((dpin ^ 1) << 3);
    } else if (bit_count > 41 - 5 - 4 - 12 - 4 - 4) {
      // wind dir
      tx20_data[4] = tx20_data[4] >> 1 | (dpin << 3);
    } else {
      // windspeed
      tx20_data[5] = tx20_data[5] >> 1 | (dpin << 11);
    }
    if (bit_count == 0) {
      start_read = false;

      // Must clear this bit in the interrupt register,
      // it gets set even when interrupts are disabled
#ifdef ARDUINO_ARCH_ESP32
      GPIO_REG_WRITE(GPIO.status_w1tc, 1 << this->pin_->get_pin());
#endif
#ifdef ARDUINO_ARCH_ESP8266
      GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, 1 << this->pin_->get_pin());
#endif
      uint8_t chk = (tx20_data[1] + (tx20_data[2] & 0xf) + ((tx20_data[2] >> 4) & 0xf) + ((tx20_data[2] >> 8) & 0xf));
      chk &= 0xf;

      if ((chk == tx20_data[3]) && (tx20_data[2] < 400)) {  // if checksum seems to be ok and wind speed below 40 m/s
        const uint8_t tx20_wind_direction = tx20_data[1];
        const float tx20_wind_speed = tx20_data[2] * 0.36f;

        this->wind_direction_sensor_->publish_state(tx20_wind_direction);
        this->wind_speed_sensor_->publish_state(tx20_wind_speed);
        if (tx20_wind_direction >= 0 && tx20_wind_direction < (sizeof(DIRECTIONS) / sizeof(*DIRECTIONS))) {
          std::string direction = DIRECTIONS[tx20_wind_direction];
          this->wind_direction_text_sensor_->publish_state(direction);
        }
      }

    } else {
      end_time = micros() + TX20_BIT_TIME;
    }
  }
}
TX20WindSpeedSensor *TX20Component::get_wind_speed_sensor() const {
  return this->wind_speed_sensor_;
}
TX20WindDirectionSensor *TX20Component::get_wind_direction_sensor() const {
  return this->wind_direction_sensor_;
}
TX20WindDirectionTextSensor *TX20Component::get_wind_direction_text_sensor() const {
  return this->wind_direction_text_sensor_;
}
TX20Component *global_tx20_ = nullptr;

}  // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif  // USE_TX20
