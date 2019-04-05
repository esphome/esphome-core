#include "esphome/defines.h"

#ifdef USE_PPD42X_SENSOR

#include "esphome/sensor/ppd42x_sensor.h"

#include "esphome/log.h"
#include "esphome/helpers.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

static const char *TAG = "sensor.ppd42x";

Ppd42xSensorComponent::Ppd42xSensorComponent(const std::string &name, GPIOPin *pm_10_0_pin, GPIOPin *pm_02_5_pin,
                                             uint32_t update_interval)
    : PollingSensorComponent(name, update_interval), pm_10_0_pin_(pm_10_0_pin), pm_02_5_pin_(pm_02_5_pin) {}




i
}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_PPD42X_SENSOR
