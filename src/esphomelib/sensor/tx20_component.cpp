// Based on:

#include "esphomelib/defines.h"

#ifdef USE_TX20

#include "esphomelib/espmath.h"
#include "esphomelib/log.h"
#include "esphomelib/text_sensor/text_sensor.h"
#include "tx20_component.h"
#ifdef ARDUINO_ARCH_ESP32
  #define GPIO_STATUS_W1TC_ADDRESS 0x24
#endif
ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.tx20";
static const uint8_t MAX_BUFFER_SIZE = 41;
static const uint16_t TX20_MAX_TIME = MAX_BUFFER_SIZE * 1220 + 5000;
const uint16_t TX20_BIT_TIME = 1200;   // microseconds
const uint8_t TX20_RESET_VALUES = 60;  // seconds
const std::vector<std::string> DIRECTIONS = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
                                             "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
static uint16_t *buffer;
static uint32_t start_time = 0;
static uint8_t buffer_index = 0;
static uint32_t spent_time = 0;
static bool tx20_available = false;

TX20Component::TX20Component(const std::string &wind_speed_name, const std::string &wind_direction_name,
                             const std::string &wind_direction_text_name, GPIOPin *pin)
    : Component(),
      wind_speed_sensor_(new TX20WindSpeedSensor(wind_speed_name)),
      wind_direction_sensor_(new TX20WindDirectionSensor(wind_direction_name)),
      wind_direction_text_sensor_(new TX20WindDirectionTextSensor(wind_direction_text_name)),
      pin_(pin) {
}

void TX20Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TX20...");
  this->pin_->setup();
  buffer = new uint16_t[MAX_BUFFER_SIZE];
  attachInterrupt(this->pin_->get_pin(), TX20Component::pin_change_, CHANGE);
}
void TX20Component::dump_config() {
  ESP_LOGCONFIG(TAG, "TX20:");
  LOG_PIN(" Pin:", this->pin_);
}
float TX20Component::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

void TX20Component::loop() {
  if (tx20_available) {
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, 1 << this->pin_->get_pin());
    this->decodeAndPublish_();
    // rearm it!
    tx20_available = false;
    buffer_index = 0;
    spent_time = 0;
    // rearm it!
    start_time = 0;
  }
}

void ICACHE_RAM_ATTR TX20Component::pin_change_() {
  uint32_t now = micros();
  if (!start_time) {
    buffer[buffer_index] = 1;
    start_time = now;
    buffer_index++;
    return;
  }
  uint16_t delay = 0;
  if (now < start_time) {
    delay = (UINT32_MAX - start_time + now);
  } else {
    delay = (now - start_time);
  }

  if (tx20_available || ((spent_time + delay > TX20_MAX_TIME) && start_time)) {
    tx20_available = true;
    return;
  }
  if (buffer_index <= MAX_BUFFER_SIZE) {
    buffer[buffer_index] = delay;
  }
  spent_time += delay;
  start_time = now;
  buffer_index++;
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

void TX20Component::decodeAndPublish_() {
  ESP_LOGV(TAG, "DECODE TX20...");

  std::string string_buffer;
  std::vector<bool> bit_buffer;
  uint8_t index_to = 0;
  bool current_bit = true;

  for (int i = 1; i <= buffer_index; i++) {
    uint8_t repeat = buffer[i] / TX20_BIT_TIME;
    string_buffer.append(repeat, current_bit ? '1' : '0');
    bit_buffer.insert(bit_buffer.end(), repeat, current_bit);
    current_bit = !current_bit;
  }
  if(string_buffer.length() < MAX_BUFFER_SIZE){
    uint8_t remain = MAX_BUFFER_SIZE - string_buffer.length();
    string_buffer.append(remain, current_bit ? '1' : '0');
    bit_buffer.insert(bit_buffer.end(), remain, current_bit);
  }

  ESP_LOGV(TAG, "Decoded bits %s", string_buffer.c_str());

  uint8_t tx20_sa = 0;
  uint8_t tx20_sb = 0;
  uint8_t tx20_sd = 0;
  uint8_t tx20_se = 0;
  uint16_t tx20_sc = 0;
  uint16_t tx20_sf = 0;
  uint8_t tx20_wind_direction = 0;
  float tx20_wind_speed_kmh = 0;

  for (int i = 0; i < 40; i++) {
    uint8_t bit = bit_buffer.at(i);
    if (i < 5) {
      // start, inverted
      tx20_sa = (tx20_sa << 1) | (bit ^ 1);
    } else if (i < 5 + 4) {
      // wind dir, inverted
      tx20_sb = tx20_sb >> 1 | ((bit ^ 1) << 3);
    } else if (i < 5 + 4 + 12) {
      // windspeed, inverted
      tx20_sc = tx20_sc >> 1 | ((bit ^ 1) << 11);
    } else if (i < 5 + 4 + 12 + 4) {
      // checksum, inverted
      tx20_sd = tx20_sd >> 1 | ((bit ^ 1) << 3);
    } else if (i < 41 + 5 + 4 + 12 + 4 + 4) {
      // wind dir
      tx20_se = tx20_se >> 1 | (bit << 3);
    } else {
      // windspeed
      tx20_sf = tx20_sf >> 1 | (bit << 11);
    }
  }

  uint8_t chk = (tx20_sb + (tx20_sc & 0xf) + ((tx20_sc >> 4) & 0xf) + ((tx20_sc >> 8) & 0xf));
  chk &= 0xf;

  if ((chk == tx20_sd) && (tx20_sc < 400)) {  // if checksum seems to be ok and wind speed below 40 m/s
    tx20_wind_speed_kmh = float(tx20_sc) * 0.36;
    tx20_wind_direction = tx20_sb;
    std::string direction = DIRECTIONS.at(tx20_wind_direction);
    ESP_LOGV(TAG, "WindSpeed %f, WindDirection %d, WindDirection Text %s", tx20_wind_speed_kmh, tx20_wind_direction,
             direction.c_str());
    this->wind_direction_sensor_->publish_state(tx20_wind_direction);
    this->wind_speed_sensor_->publish_state(tx20_wind_speed_kmh);
    this->wind_direction_text_sensor_->publish_state(direction);
  } else {
    ESP_LOGV(TAG, "Incorrect checksum!");
  }
}

}  // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif  // USE_TX20