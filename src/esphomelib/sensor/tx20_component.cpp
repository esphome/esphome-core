#include "esphomelib/defines.h"

#ifdef USE_TX20

#include "esphomelib/log.h"
#include "esphomelib/sensor/tx20_component.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {  // namespace

static const char *TAG = "sensor.tx20";
static const uint8_t MAX_BUFFER_SIZE = 41;
static const uint16_t TX20_MAX_TIME = MAX_BUFFER_SIZE * 1220 + 5000;
static const uint16_t TX20_BIT_TIME = 1200;
static const char* DIRECTIONS[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
                                    "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};

TX20Component::TX20Component(const std::string &wind_speed_name, const std::string &wind_direction_degrees_name,
                             GPIOPin *pin)
    : Component(),
      wind_speed_sensor_(new TX20WindSpeedSensor(wind_speed_name)),
      wind_direction_degrees_sensor_(new TX20WindDirectionDegreesSensor(wind_direction_degrees_name)),
      pin_(pin) {
}

void TX20Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TX20...");
  this->pin_->setup();
  attachInterrupt(this->pin_->get_pin(), TX20Component::pin_change_, CHANGE);
}
void TX20Component::dump_config() {
  ESP_LOGCONFIG(TAG, "TX20:");
  LOG_PIN("  Pin:", this->pin_);
}
float TX20Component::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

void TX20Component::loop() {
  if (tx20_available_) {
    this->decode_and_publish_();
    tx20_available_ = false;
    buffer_index_ = 0;
    spent_time_ = 0;
    // rearm it!
    start_time_ = 0;
  }
}

void ICACHE_RAM_ATTR TX20Component::pin_change_() {
  uint32_t now = micros();
  if (!start_time_) {
    buffer_[buffer_index_] = 1;
    start_time_ = now;
    buffer_index_++;
    return;
  }
  uint16_t delay = 0;
  if (now < start_time_) {
    delay = (UINT32_MAX - start_time_ + now);
  } else {
    delay = (now - start_time_);
  }

  if (tx20_available_ || ((spent_time_ + delay > TX20_MAX_TIME) && start_time_)) {
    tx20_available_ = true;
    return;
  }
  if (buffer_index_ <= MAX_BUFFER_SIZE) {
    buffer_[buffer_index_] = delay;
  }
  spent_time_ += delay;
  start_time_ = now;
  buffer_index_++;
}
std::string TX20Component::get_wind_cardinal_direction() const {
  return this->wind_cardinal_direction_;
}

TX20WindSpeedSensor *TX20Component::get_wind_speed_sensor() const {
  return this->wind_speed_sensor_;
}
TX20WindDirectionDegreesSensor *TX20Component::get_wind_direction_degrees_sensor() const {
  return this->wind_direction_degrees_sensor_;
}

void TX20Component::decode_and_publish_() {
  ESP_LOGV(TAG, "DECODE TX20...");

  std::string string_buffer;
  std::vector<bool> bit_buffer;
  bool current_bit = true;

  for (int i = 1; i <= buffer_index_; i++) {
    uint8_t repeat = buffer_[i] / TX20_BIT_TIME;
    string_buffer.append(repeat, current_bit ? '1' : '0');
    bit_buffer.insert(bit_buffer.end(), repeat, current_bit);
    current_bit = !current_bit;
  }
  if (string_buffer.length() < MAX_BUFFER_SIZE) {
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
  // checksum alone is not good enough - the check for 40 m/s is still needed in order
  // to avoid incorrect values
  if ((chk == tx20_sd) && (tx20_sc < 400)) {  
    tx20_wind_speed_kmh = float(tx20_sc) * 0.36;
    tx20_wind_direction = tx20_sb;
    if (tx20_wind_direction >= 0 && tx20_wind_direction < 16) {
      wind_cardinal_direction_ = DIRECTIONS[tx20_wind_direction];
    }

    ESP_LOGV(TAG, "WindSpeed %f, WindDirection %d, WindDirection Text %s", tx20_wind_speed_kmh, tx20_wind_direction,
             direction.c_str());
    this->wind_direction_degrees_sensor_->publish_state(float(tx20_wind_direction) * 22.5f);
    this->wind_speed_sensor_->publish_state(tx20_wind_speed_kmh);

  } else {
    // sensor seems to produces quite a few checksum errors -> only in verbose mode
    ESP_LOGV(TAG, "Incorrect checksum!");
  }
}

uint16_t *TX20Component::buffer_ = new uint16_t[MAX_BUFFER_SIZE];
uint32_t TX20Component::start_time_ = 0;
uint8_t TX20Component::buffer_index_ = 0;
uint32_t TX20Component::spent_time_ = 0;
bool TX20Component::tx20_available_ = false;

}  // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif  // USE_TX20
