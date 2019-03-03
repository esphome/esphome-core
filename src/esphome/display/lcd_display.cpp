#include "esphome/defines.h"

#ifdef USE_LCD_DISPLAY

#include "esphome/display/lcd_display.h"
#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace display {

static const char *TAG = "display.lcd";

// First set bit determines command, bits after that are the data.
static const uint8_t LCD_DISPLAY_COMMAND_CLEAR_DISPLAY = 0x01;
static const uint8_t LCD_DISPLAY_COMMAND_RETURN_HOME = 0x02;
static const uint8_t LCD_DISPLAY_COMMAND_ENTRY_MODE_SET = 0x04;
static const uint8_t LCD_DISPLAY_COMMAND_DISPLAY_CONTROL = 0x08;
static const uint8_t LCD_DISPLAY_COMMAND_CURSOR_SHIFT = 0x10;
static const uint8_t LCD_DISPLAY_COMMAND_FUNCTION_SET = 0x20;
static const uint8_t LCD_DISPLAY_COMMAND_SET_CGRAM_ADDR = 0x40;
static const uint8_t LCD_DISPLAY_COMMAND_SET_DDRAM_ADDR = 0x80;

static const uint8_t LCD_DISPLAY_ENTRY_SHIFT_INCREMENT = 0x01;
static const uint8_t LCD_DISPLAY_ENTRY_LEFT = 0x02;

static const uint8_t LCD_DISPLAY_DISPLAY_BLINK_ON = 0x01;
static const uint8_t LCD_DISPLAY_DISPLAY_CURSOR_ON = 0x02;
static const uint8_t LCD_DISPLAY_DISPLAY_ON = 0x04;

static const uint8_t LCD_DISPLAY_FUNCTION_8_BIT_MODE = 0x10;
static const uint8_t LCD_DISPLAY_FUNCTION_2_LINE = 0x08;
static const uint8_t LCD_DISPLAY_FUNCTION_5X10_DOTS = 0x04;

void LCDDisplay::setup() {
  this->buffer_ = new uint8_t[this->rows_ * this->columns_];
  for (uint8_t i = 0; i < this->rows_ * this->columns_; i++)
    this->buffer_[i] = ' ';

  uint8_t display_function = 0;

  if (!this->is_four_bit_mode())
    display_function |= LCD_DISPLAY_FUNCTION_8_BIT_MODE;

  if (this->rows_ > 1)
    display_function |= LCD_DISPLAY_FUNCTION_2_LINE;

  // TODO dotsize

  // Commands can only be sent 40ms after boot-up, so let's wait if we're close
  const uint8_t now = millis();
  if (now < 40)
    delay(40u - now);

  if (this->is_four_bit_mode()) {
    this->write_n_bits(0x03, 4);
    delay(5);  // 4.1ms
    this->write_n_bits(0x03, 4);
    delay(5);
    this->write_n_bits(0x03, 4);
    delayMicroseconds(150);
    this->write_n_bits(0x02, 4);
  } else {
    this->command_(LCD_DISPLAY_COMMAND_FUNCTION_SET | display_function);
    delay(5);  // 4.1ms
    this->command_(LCD_DISPLAY_COMMAND_FUNCTION_SET | display_function);
    delayMicroseconds(150);
    this->command_(LCD_DISPLAY_COMMAND_FUNCTION_SET | display_function);
  }

  this->command_(LCD_DISPLAY_COMMAND_FUNCTION_SET | display_function);
  uint8_t display_control = LCD_DISPLAY_DISPLAY_ON;
  this->command_(LCD_DISPLAY_COMMAND_DISPLAY_CONTROL | display_control);

  // clear display, also sets DDRAM address to 0 (home)
  this->command_(LCD_DISPLAY_COMMAND_CLEAR_DISPLAY);
  delay(2);  // 1.52ms

  uint8_t entry_mode = LCD_DISPLAY_ENTRY_LEFT;
  this->command_(LCD_DISPLAY_COMMAND_ENTRY_MODE_SET | entry_mode);  // 37µs

  this->command_(LCD_DISPLAY_COMMAND_RETURN_HOME);
  delay(2);  // 1.52ms
}

float LCDDisplay::get_setup_priority() const { return setup_priority::POST_HARDWARE; }
void HOT LCDDisplay::display() {
  this->command_(LCD_DISPLAY_COMMAND_SET_DDRAM_ADDR | 0);

  for (uint8_t i = 0; i < this->columns_; i++)
    this->send(this->buffer_[i], true);

  if (this->rows_ >= 3) {
    for (uint8_t i = 0; i < this->columns_; i++)
      this->send(this->buffer_[this->columns_ * 2 + i], true);
  }

  if (this->rows_ >= 1) {
    this->command_(LCD_DISPLAY_COMMAND_SET_DDRAM_ADDR | 0x40);

    for (uint8_t i = 0; i < this->columns_; i++)
      this->send(this->buffer_[this->columns_ + i], true);

    if (this->rows_ >= 4) {
      for (uint8_t i = 0; i < this->columns_; i++)
        this->send(this->buffer_[this->columns_ * 3 + i], true);
    }
  }
}
void LCDDisplay::update() {
  for (uint8_t i = 0; i < this->rows_ * this->columns_; i++)
    this->buffer_[i] = ' ';

  this->writer_(*this);
  this->display();
}
void LCDDisplay::command_(uint8_t value) { this->send(value, false); }
void LCDDisplay::print(uint8_t column, uint8_t row, const char *str) {
  uint8_t pos = column + row * this->columns_;
  for (; *str != '\0'; str++) {
    if (*str == '\n') {
      pos = ((pos / this->columns_) + 1) * this->columns_;
      continue;
    }
    if (pos >= this->rows_ * this->columns_) {
      ESP_LOGW(TAG, "LCDDisplay writing out of range!");
      break;
    }

    this->buffer_[pos] = *reinterpret_cast<const uint8_t *>(str);
    pos++;
  }
}
void LCDDisplay::print(uint8_t column, uint8_t row, const std::string &str) { this->print(column, row, str.c_str()); }
void LCDDisplay::print(const char *str) { this->print(0, 0, str); }
void LCDDisplay::print(const std::string &str) { this->print(0, 0, str.c_str()); }
void LCDDisplay::printf(uint8_t column, uint8_t row, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[256];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    this->print(column, row, buffer);
}
void LCDDisplay::printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[256];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    this->print(0, 0, buffer);
}
LCDDisplay::LCDDisplay(uint8_t columns, uint8_t rows, uint32_t update_interval)
    : PollingComponent(update_interval), columns_(columns), rows_(rows) {}
void LCDDisplay::set_writer(lcd_writer_t &&writer) { this->writer_ = std::move(writer); }
#ifdef USE_TIME
void LCDDisplay::strftime(uint8_t column, uint8_t row, const char *format, time::ESPTime time) {
  char buffer[64];
  size_t ret = time.strftime(buffer, sizeof(buffer), format);
  if (ret > 0)
    this->print(column, row, buffer);
}
void LCDDisplay::strftime(const char *format, time::ESPTime time) { this->strftime(0, 0, format, time); }
#endif

void GPIOLCDDisplay::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GPIO LCD Display...");
  this->rs_pin_->setup();  // OUTPUT
  this->rs_pin_->digital_write(false);
  if (this->rw_pin_ != nullptr) {
    this->rw_pin_->setup();  // OUTPUT
    this->rw_pin_->digital_write(false);
  }
  this->enable_pin_->setup();  // OUTPUT
  this->enable_pin_->digital_write(false);

  for (uint8_t i = 0; i < (this->is_four_bit_mode() ? 4 : 8); i++) {
    this->data_pins_[i]->setup();  // OUTPUT
    this->data_pins_[i]->digital_write(false);
  }
  LCDDisplay::setup();
}
void GPIOLCDDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "GPIO LCD Display:");
  ESP_LOGCONFIG(TAG, "  Columns: %u, Rows: %u", this->columns_, this->rows_);
  LOG_PIN("  RS Pin: ", this->rs_pin_);
  LOG_PIN("  RW Pin: ", this->rw_pin_);
  LOG_PIN("  Enable Pin: ", this->enable_pin_);

  for (uint8_t i = 0; i < (this->is_four_bit_mode() ? 4 : 8); i++) {
    ESP_LOGCONFIG(TAG, "  Data Pin %u" LOG_PIN_PATTERN, i, LOG_PIN_ARGS(this->data_pins_[i]));
  }

  LOG_UPDATE_INTERVAL(this);
}
void GPIOLCDDisplay::set_data_pins(const GPIOOutputPin &d0, const GPIOOutputPin &d1, const GPIOOutputPin &d2,
                                   const GPIOOutputPin &d3) {
  this->data_pins_[0] = d0.copy();
  this->data_pins_[1] = d1.copy();
  this->data_pins_[2] = d2.copy();
  this->data_pins_[3] = d3.copy();
}
void GPIOLCDDisplay::set_data_pins(const GPIOOutputPin &d0, const GPIOOutputPin &d1, const GPIOOutputPin &d2,
                                   const GPIOOutputPin &d3, const GPIOOutputPin &d4, const GPIOOutputPin &d5,
                                   const GPIOOutputPin &d6, const GPIOOutputPin &d7) {
  this->data_pins_[0] = d0.copy();
  this->data_pins_[1] = d1.copy();
  this->data_pins_[2] = d2.copy();
  this->data_pins_[3] = d3.copy();
  this->data_pins_[4] = d4.copy();
  this->data_pins_[5] = d5.copy();
  this->data_pins_[6] = d6.copy();
  this->data_pins_[7] = d7.copy();
}
void GPIOLCDDisplay::set_enable_pin(const GPIOOutputPin &enable) { this->enable_pin_ = enable.copy(); }
void GPIOLCDDisplay::set_rs_pin(const GPIOOutputPin &rs) { this->rs_pin_ = rs.copy(); }
void GPIOLCDDisplay::set_rw_pin(const GPIOOutputPin &rw) { this->rw_pin_ = rw.copy(); }
bool GPIOLCDDisplay::is_four_bit_mode() { return this->data_pins_[4] == nullptr; }
void GPIOLCDDisplay::write_n_bits(uint8_t value, uint8_t n) {
  for (uint8_t i = 0; i < n; i++)
    this->data_pins_[i]->digital_write(value & (1 << i));

  this->enable_pin_->digital_write(true);
  delayMicroseconds(1);  // >450ns
  this->enable_pin_->digital_write(false);
  delayMicroseconds(40);  // >37us
}
void GPIOLCDDisplay::send(uint8_t value, bool rs) {
  this->rs_pin_->digital_write(rs);

  if (this->is_four_bit_mode()) {
    this->write_n_bits(value >> 4, 4);
    this->write_n_bits(value, 4);
  } else {
    this->write_n_bits(value, 8);
  }
}
GPIOLCDDisplay::GPIOLCDDisplay(uint8_t columns, uint8_t rows, uint32_t update_interval)
    : LCDDisplay(columns, rows, update_interval) {}

#ifdef USE_LCD_DISPLAY_PCF8574
void PCF8574LCDDisplay::setup() {
  ESP_LOGCONFIG(TAG, "Setting up PCF8574 LCD Display...");
  if (!this->write_bytes(0x08, nullptr, 0)) {
    this->mark_failed();
    return;
  }

  LCDDisplay::setup();
}
void PCF8574LCDDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "PCF8574 LCD Display:");
  ESP_LOGCONFIG(TAG, "  Columns: %u, Rows: %u", this->columns_, this->rows_);
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with LCD Display failed!");
  }
}
bool PCF8574LCDDisplay::is_four_bit_mode() { return true; }
void PCF8574LCDDisplay::write_n_bits(uint8_t value, uint8_t n) {
  if (n == 4) {
    // Ugly fix: in the super setup() with n == 4 value needs to be shifted left
    value <<= 4;
  }
  uint8_t data = value | 0x08;  // Enable backlight
  this->write_bytes(data, nullptr, 0);
  // Pulse ENABLE
  this->write_bytes(data | 0x04, nullptr, 0);
  delayMicroseconds(1);  // >450ns
  this->write_bytes(data, nullptr, 0);
  delayMicroseconds(100);  // >37us
}
void PCF8574LCDDisplay::send(uint8_t value, bool rs) {
  this->write_n_bits((value & 0xF0) | rs, 0);
  this->write_n_bits(((value << 4) & 0xF0) | rs, 0);
}
PCF8574LCDDisplay::PCF8574LCDDisplay(I2CComponent *parent, uint8_t columns, uint8_t rows, uint8_t address,
                                     uint32_t update_interval)
    : LCDDisplay(columns, rows, update_interval), I2CDevice(parent, address) {}
#endif  // USE_LCD_DISPLAY_PCF8574

}  // namespace display

ESPHOME_NAMESPACE_END

#endif  // USE_LCD_DISPLAY
