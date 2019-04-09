#include "esphome/defines.h"

#ifdef USE_CLIMATE

#include "esphome/climate/climate_mode.h"

ESPHOME_NAMESPACE_BEGIN

namespace climate {

const char *climate_mode_to_string(ClimateMode mode) {
  switch (mode) {
    case CLIMATE_MODE_OFF:
      return "off";
    case CLIMATE_MODE_AUTO:
      return "auto";
    case CLIMATE_MODE_COOL:
      return "cool";
    case CLIMATE_MODE_HEAT:
      return "heat";
    default:
      return "UNKNOWN";
  }
}

}  // namespace climate

ESPHOME_NAMESPACE_END

#endif  // USE_CLIMATE
