#include "esphomelib/defines.h"

#ifdef USE_TX20

#include "esphomelib/espmath.h"
#include "esphomelib/log.h"
#include "esphomelib/text_sensor/text_sensor.h"
#include "esphomelib/sensor/tx20_component.h"

#ifdef ARDUINO_ARCH_ESP32
  #define GPIO_STATUS_W1TC_ADDRESS 0x24
#endif

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.tx20";

const char UNIT_KMH[] = "km/h";
const char ICON_WIND_SPEED[] = "mdi:weather-windy";
const char ICON_WIND_DIRECTION[] = "mdi:sign-direction";
const char EMPTY[] = "";
const uint16_t TX20_BIT_TIME = 1220;   // microseconds
const uint8_t TX20_RESET_VALUES = 60;  // seconds

const std::string DIRECTIONS[] = { "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
                                   "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};

bool tx20_available = false;
float tx20_wind_speed_kmh = 0;
uint8_t tx20_wind_direction = 0;
static bool first_instance = false;

TX20Component::TX20Component(const std::string &wind_speed_name, const std::string &wind_direction_name,
                             const std::string &wind_direction_text_name, GPIOPin *pin, uint32_t update_interval)
    : PollingComponent(update_interval),
      wind_speed_sensor_(new TX20WindSpeedSensor(wind_speed_name, this)),
      wind_direction_sensor_(new TX20WindDirectionSensor(wind_direction_name, this)),
      wind_direction_text_sensor_(new TX20WindDirectionTextSensor(wind_direction_text_name)),
      pin_(pin) {
  assert(!first_instance);
  first_instance = true;
}

void TX20Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TX20...");
  this->pin_->setup();

  attachInterrupt(this->pin_->get_pin(), TX20Component::start_read_, RISING);
  global_tx20_ = this;
}
void TX20Component::dump_config() {
  ESP_LOGCONFIG(TAG, "TX20:");
  LOG_PIN(" Pin:", this->pin_);
  LOG_UPDATE_INTERVAL(this);
}
float TX20Component::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

void TX20Component::update() {
  if (tx20_available) {
    ESP_LOGV(TAG, "Updating TX20...");

    this->wind_direction_sensor_->publish_state(tx20_wind_direction);
    this->wind_speed_sensor_->publish_state(tx20_wind_speed_kmh);

    if(tx20_wind_direction >= DIRECTIONS && tx20_wind_direction < std::size(DIRECTIONS)){
      std::string direction = DIRECTIONS[tx20_wind_direction];
      this->wind_direction_text_sensor_->publish_state(direction);
    }
    tx20_available = false;
  }
}
void TX20Component::start_read_() {
  global_tx20_->start_read_internal_();
}

void TX20Component::start_read_internal_() {
  /* La Crosse TX20 Anemometer datagram every 2 seconds
   * 0-0 11011 0011 111010101111 0101 1100 000101010000 0-0 - Received pin data
   * at 1200 uSec per bit sa    sb   sc           sd   se   sf 00100 1100
   * 000101010000 1010 1100 000101010000     - sa to sd inverted user data,
   * LSB first sa - Start frame always 00100 sb - Wind direction 0 - 15 sc -
   * Wind speed 0 - 511 sd - Checksum se - Wind direction 0 - 15 sf - Wind speed
   * 0 - 511
   */

  tx20_available = false;

  uint8_t tx20_sa = 0;
  uint8_t tx20_sb = 0;
  uint8_t tx20_sd = 0;
  uint8_t tx20_se = 0;
  uint8_t tx20_sc = 0;
  uint8_t tx20_sf = 0;

  delayMicroseconds(TX20_BIT_TIME / 2);

  for (int bitcount = 41; bitcount > 0; bitcount--) {
    uint8_t dpin = this->pin_->digital_read();
    if (bitcount > 41 - 5) {
      // start, inverted
      tx20_sa = (tx20_sa << 1) | (dpin ^ 1);
    } else if (bitcount > 41 - 5 - 4) {
      // wind dir, inverted
      tx20_sb = tx20_sb >> 1 | ((dpin ^ 1) << 3);
    } else if (bitcount > 41 - 5 - 4 - 12) {
      // windspeed, inverted
      tx20_sc = tx20_sc >> 1 | ((dpin ^ 1) << 11);
    } else if (bitcount > 41 - 5 - 4 - 12 - 4) {
      // checksum, inverted
      tx20_sd = tx20_sd >> 1 | ((dpin ^ 1) << 3);
    } else if (bitcount > 41 - 5 - 4 - 12 - 4 - 4) {
      // wind dir
      tx20_se = tx20_se >> 1 | (dpin << 3);
    } else {
      // windspeed
      tx20_sf = tx20_sf >> 1 | (dpin << 11);
    }

    delayMicroseconds(TX20_BIT_TIME);
  }

  uint8_t chk = (tx20_sb + (tx20_sc & 0xf) + ((tx20_sc >> 4) & 0xf) + ((tx20_sc >> 8) & 0xf));
  chk &= 0xf;

  if ((chk == tx20_sd) && (tx20_sc < 400)) {  // if checksum seems to be ok and wind speed below 40 m/s
    tx20_available = true;
  }

  // Must clear this bit in the interrupt register,
  // it gets set even when interrupts are disabled
  tx20_wind_direction = tx20_sb;
  tx20_wind_speed_kmh = float(tx20_sc) * 0.36f;
#ifdef ARDUINO_ARCH_ESP32
  GPIO_REG_WRITE(GPIO.status_w1tc, 1 << this->pin_->get_pin());
#endif
#ifdef ARDUINO_ARCH_ESP8266
  GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, 1 << this->pin_->get_pin());
#endif
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
TX20Component *global_tx20_;

}  // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif  // USE_TX20
