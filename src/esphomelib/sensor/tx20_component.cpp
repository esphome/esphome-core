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

volatile bool start_read = false;
uint16_t read_interval = 40 * TX20_BIT_TIME + 10000; // add a bit of overhead
uint8_t bit_count = 0;
uint8_t tx20_data[6] = {0};

volatile uint8_t pin_status[40] = {0};

volatile uint8_t last_pin = 1;
volatile uint8_t last_index = 0;
volatile uint32_t last_interrupt = 0;
volatile uint32_t interrupt = 0;
volatile uint32_t start_time = 0;
volatile uint8_t chk = 0;



bool tx20_available = false;

TX20Component::TX20Component(const std::string &wind_speed_name, const std::string &wind_direction_name,
                             const std::string &wind_direction_text_name, GPIOPin *pin)
    : Component(),
      wind_speed_sensor_(new TX20WindSpeedSensor(wind_speed_name)),
      wind_direction_sensor_(new TX20WindDirectionSensor(wind_direction_name)),
      wind_direction_text_sensor_(new TX20WindDirectionTextSensor(wind_direction_text_name)),
      pin_(pin) {
  if(global_tx20_){
    this->mark_failed();
  }
  global_tx20_ = this;
}

void TX20Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TX20...");
  this->pin_->setup();  
  
  ESP_LOGI(TAG, "START ATTACHED INTERRUPTS!");
  attachInterrupt(this->pin_->get_pin(), TX20Component::tx20_black_change_static_, CHANGE);
  ESP_LOGI(TAG, "END ATTACHED INTERRUPTS!");
}
void TX20Component::dump_config() {
  ESP_LOGCONFIG(TAG, "TX20:");
  LOG_PIN(" Pin:", this->pin_);
}
float TX20Component::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}

void TX20Component::loop() {
  if(last_interrupt != interrupt){
    ESP_LOGI(TAG, "CHANGE!");
    last_interrupt = interrupt;
  }
  if(start_read && micros() - start_time >= read_interval){
    ESP_LOGI(TAG, "FINISHED READING ROUND!");
    // finish off the reading
    this->tx20_black_change_();
    start_read = false;

    for(int i = 41; i > 0; i--){
      uint8_t dpin = pin_status[i];
      //ESP_LOGI(TAG, "PIN %d -> %d", i, unsigned(dpin));
      if (i > 41 - 5) {
        // start, inverted
        tx20_data[0] = (tx20_data[0] << 1) | (dpin ^ 1);
      } else if (i > 41 - 5 - 4) {
        // wind dir, inverted
        tx20_data[1] = tx20_data[1] >> 1 | ((dpin ^ 1) << 3);
      } else if (i > 41 - 5 - 4 - 12) {
        // windspeed, inverted
        tx20_data[2] = tx20_data[2] >> 1 | ((dpin ^ 1) << 11);
      } else if (i > 41 - 5 - 4 - 12 - 4) {
        // checksum, inverted
        tx20_data[3] = tx20_data[3] >> 1 | ((dpin ^ 1) << 3);
      } else if (i > 41 - 5 - 4 - 12 - 4 - 4) {
        // wind dir
        tx20_data[4] = tx20_data[4] >> 1 | (dpin << 3);
      } else {
        // windspeed
        tx20_data[5] = tx20_data[5] >> 1 | (dpin << 11);
      }
    }
    chk &= 0xf;


      
    if ((chk == tx20_data[3]) && (tx20_data[2] < 400)) {  // if checksum seems to be ok and wind speed below 40 m/s
      tx20_available = true;
    }
// #ifdef ARDUINO_ARCH_ESP32
//       GPIO_REG_WRITE(GPIO.status_w1tc, 1 << this->pin_->get_pin());
// #endif
// #ifdef ARDUINO_ARCH_ESP8266
//       GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, 1 << this->pin_->get_pin());
// #endif

  }

  if(tx20_available){
    tx20_available = false;
    const uint8_t tx20_wind_direction = tx20_data[1];
    const float tx20_wind_speed = float(tx20_data[2]) * 0.36f;

    this->wind_direction_sensor_->publish_state(tx20_wind_direction);
    this->wind_speed_sensor_->publish_state(tx20_wind_speed);
    ESP_LOGI(TAG, "WindSpeed %f, WindDirection %d", tx20_wind_speed, unsigned(tx20_wind_direction));

    std::string direction = DIRECTIONS[tx20_wind_direction];
    this->wind_direction_text_sensor_->publish_state(direction);
    ESP_LOGI(TAG, "WindDirectionText %s", direction.c_str());
    std::fill(std::begin(tx20_data), std::end(tx20_data), 0);

  }
}

void ICACHE_RAM_ATTR TX20Component::tx20_black_change_static_() {
  global_tx20_->tx20_black_change_();
}


void ICACHE_RAM_ATTR TX20Component::tx20_black_change_(){
  uint32_t now = micros();
  uint8_t pin = this->pin_->digital_read();
  if(!start_read && pin){

    delayMicroseconds(TX20_BIT_TIME / 2);
  //   // init
    start_read = true;
    start_time = now;
    std::fill(std::begin(pin_status), std::end(pin_status), 0);
   }else{
     uint16_t delay = now - last_interrupt - (TX20_BIT_TIME / 2);
     uint8_t index = delay / TX20_BIT_TIME;
    // ESP_LOGI(TAG, "index: %d!", unsigned(index));
     std::fill(std::begin(pin_status) + last_index + 1, std::begin(pin_status) + index + 1, last_pin);

     last_index = index;
     last_pin = pin;
  }
  // ESP_LOGI(TAG, "PIN VALUE: %d!", unsigned(pin));

  last_interrupt = now;
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
