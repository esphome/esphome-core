#include "esphome/defines.h"

#ifdef USE_SENSOR

#include <utility>
#include "esphome/sensor/sensor.h"

#include "esphome/log.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.sensor";

void Sensor::publish_state(float state) {
  this->raw_state = state;
  this->raw_callback_.call(state);

  ESP_LOGV(TAG, "'%s': Received new state %f", this->name_.c_str(), state);

  if (this->filter_list_ == nullptr) {
    this->internal_send_state_to_frontend(state);
  } else {
    this->filter_list_->input(state);
  }
}
void Sensor::push_new_value(float state) { this->publish_state(state); }
std::string Sensor::unit_of_measurement() { return ""; }
std::string Sensor::icon() { return ""; }
uint32_t Sensor::update_interval() { return 0; }
int8_t Sensor::accuracy_decimals() { return 0; }
Sensor::Sensor(const std::string &name) : Nameable(name), state(NAN), raw_state(NAN) {}
Sensor::Sensor() : Sensor("") {}

void Sensor::set_unit_of_measurement(const std::string &unit_of_measurement) {
  this->unit_of_measurement_ = unit_of_measurement;
}
void Sensor::set_icon(const std::string &icon) { this->icon_ = icon; }
void Sensor::set_accuracy_decimals(int8_t accuracy_decimals) { this->accuracy_decimals_ = accuracy_decimals; }
void Sensor::add_on_state_callback(std::function<void(float)> &&callback) { this->callback_.add(std::move(callback)); }
void Sensor::add_on_raw_state_callback(std::function<void(float)> &&callback) {
  this->raw_callback_.add(std::move(callback));
}
std::string Sensor::get_icon() {
  if (this->icon_.has_value())
    return *this->icon_;
  return this->icon();
}
std::string Sensor::get_unit_of_measurement() {
  if (this->unit_of_measurement_.has_value())
    return *this->unit_of_measurement_;
  return this->unit_of_measurement();
}
int8_t Sensor::get_accuracy_decimals() {
  if (this->accuracy_decimals_.has_value())
    return *this->accuracy_decimals_;
  return this->accuracy_decimals();
}
void Sensor::add_filter(Filter *filter) {
  // inefficient, but only happens once on every sensor setup and nobody's going to have massive amounts of
  // filters
  ESP_LOGVV(TAG, "Sensor(%p)::add_filter(%p)", this, filter);
  if (this->filter_list_ == nullptr) {
    this->filter_list_ = filter;
  } else {
    Filter *last_filter = this->filter_list_;
    while (last_filter->next_ != nullptr)
      last_filter = last_filter->next_;
    last_filter->initialize(this, filter);
  }
  filter->initialize(this, nullptr);
}
void Sensor::add_filters(const std::vector<Filter *> &filters) {
  for (Filter *filter : filters) {
    this->add_filter(filter);
  }
}
void Sensor::set_filters(const std::vector<Filter *> &filters) {
  this->clear_filters();
  this->add_filters(filters);
}
void Sensor::clear_filters() {
  if (this->filter_list_ != nullptr) {
    ESP_LOGVV(TAG, "Sensor(%p)::clear_filters()", this);
  }
  this->filter_list_ = nullptr;
}
float Sensor::get_value() const { return this->state; }
float Sensor::get_state() const { return this->state; }
float Sensor::get_raw_value() const { return this->raw_state; }
float Sensor::get_raw_state() const { return this->raw_state; }
std::string Sensor::unique_id() { return ""; }

void Sensor::internal_send_state_to_frontend(float state) {
  this->has_state_ = true;
  this->state = state;
  if (this->filter_list_ != nullptr) {
    ESP_LOGD(TAG, "'%s': Sending state %.5f %s with %d decimals of accuracy", this->get_name().c_str(), state,
             this->get_unit_of_measurement().c_str(), this->get_accuracy_decimals());
  }
  this->callback_.call(state);
}
SensorStateTrigger *Sensor::make_state_trigger() { return new SensorStateTrigger(this); }
SensorRawStateTrigger *Sensor::make_raw_state_trigger() { return new SensorRawStateTrigger(this); }
ValueRangeTrigger *Sensor::make_value_range_trigger() { return new ValueRangeTrigger(this); }
bool Sensor::has_state() const { return this->has_state_; }
uint32_t Sensor::calculate_expected_filter_update_interval() {
  uint32_t interval = this->update_interval();
  if (interval == 4294967295UL)
    // update_interval: never
    return 0;

  if (this->filter_list_ == nullptr) {
    return interval;
  }

  return this->filter_list_->calculate_remaining_interval(interval);
}
uint32_t Sensor::hash_base() { return 2455723294UL; }

#ifdef USE_MQTT_SENSOR
MQTTSensorComponent *Sensor::get_mqtt() const { return this->mqtt_; }
void Sensor::set_mqtt(MQTTSensorComponent *mqtt) { this->mqtt_ = mqtt; }
#endif

PollingSensorComponent::PollingSensorComponent(const std::string &name, uint32_t update_interval)
    : PollingComponent(update_interval), Sensor(name) {}

uint32_t PollingSensorComponent::update_interval() { return this->get_update_interval(); }

const char ICON_FLASH[] = "mdi:flash";
const char UNIT_V[] = "V";
const char ICON_EMPTY[] = "";
const char UNIT_C[] = "°C";
const char ICON_GAUGE[] = "mdi:gauge";
const char UNIT_HPA[] = "hPa";
const char ICON_WATER_PERCENT[] = "mdi:water-percent";
const char UNIT_PERCENT[] = "%";
const char ICON_SCREEN_ROTATION[] = "mdi:screen-rotation";
const char ICON_BRIEFCASE_DOWNLOAD[] = "mdi:briefcase-download";
const char UNIT_DEGREES_PER_SECOND[] = "°/s";
const char UNIT_M_PER_S_SQUARED[] = "m/s²";
const char ICON_BRIGHTNESS_5[] = "mdi:brightness-5";
const char UNIT_LX[] = "lx";
const char UNIT_OHM[] = "Ω";
const char ICON_GAS_CYLINDER[] = "mdi:gas-cylinder";
const char ICON_PERIODIC_TABLE_CO2[] = "mdi:periodic-table-co2";
const char UNIT_PPM[] = "ppm";
const char UNIT_A[] = "A";
const char UNIT_W[] = "W";
const char ICON_MAGNET[] = "mdi:magnet";
const char ICON_THERMOMETER[] = "mdi:thermometer";
const char ICON_LIGHTBULB[] = "mdi:lightbulb";
const char ICON_BATTERY[] = "mdi:battery";
const char ICON_FLOWER[] = "mdi:flower";
const char UNIT_UT[] = "µT";
const char UNIT_DEGREES[] = "°";
const char UNIT_K[] = "K";
const char UNIT_MICROSIEMENS_PER_CENTIMETER[] = "µS/cm";
const char UNIT_MICROGRAMS_PER_CUBIC_METER[] = "µg/m³";
const char ICON_CHEMICAL_WEAPON[] = "mdi:chemical-weapon";

SensorStateTrigger::SensorStateTrigger(Sensor *parent) {
  parent->add_on_state_callback([this](float value) { this->trigger(value); });
}

SensorRawStateTrigger::SensorRawStateTrigger(Sensor *parent) {
  parent->add_on_raw_state_callback([this](float value) { this->trigger(value); });
}

ValueRangeTrigger::ValueRangeTrigger(Sensor *parent) : parent_(parent) {}
void ValueRangeTrigger::on_state_(float state) {
  if (isnan(state))
    return;

  float local_min = this->min_.value(state);
  float local_max = this->max_.value(state);

  bool in_range;
  if (isnan(local_min) && isnan(local_max)) {
    in_range = this->previous_in_range_;
  } else if (isnan(local_min)) {
    in_range = state <= local_max;
  } else if (isnan(local_max)) {
    in_range = state >= local_min;
  } else {
    in_range = local_min <= state && state <= local_max;
  }

  if (in_range != this->previous_in_range_ && in_range) {
    this->trigger(state);
  }

  this->previous_in_range_ = in_range;
  this->rtc_.save(&in_range);
}
void ValueRangeTrigger::setup() {
  this->rtc_ = global_preferences.make_preference<bool>(this->parent_->get_object_id_hash());
  bool initial_state;
  if (this->rtc_.load(&initial_state)) {
    this->previous_in_range_ = initial_state;
  }

  this->parent_->add_on_state_callback([this](float state) { this->on_state_(state); });
}
float ValueRangeTrigger::get_setup_priority() const { return setup_priority::HARDWARE; }

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_SENSOR
