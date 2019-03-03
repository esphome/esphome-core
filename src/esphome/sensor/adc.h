#ifndef ESPHOME_SENSOR_ADC_SENSOR_COMPONENT_H
#define ESPHOME_SENSOR_ADC_SENSOR_COMPONENT_H

#include "esphome/defines.h"

#ifdef USE_ADC_SENSOR

#include "esphome/component.h"
#include "esphome/sensor/sensor.h"
#include "esphome/esphal.h"

ESPHOME_NAMESPACE_BEGIN

namespace sensor {

/** This class allows using the integrated Analog to Digital converts of the ESP32 and ESP8266.
 *
 * Internally it uses the existing `analogRead` methods for doing this.
 *
 * The ESP8266 only has one pin where this can be used: A0
 *
 * The ESP32 has multiple pins that can be used with this component: GPIO32-GPIO39
 * Note you can't use the ADC2 here on the ESP32 because that's already used by WiFi internally.
 * Additionally on the ESP32 you can set an using `set_attenuation`.
 */
class ADCSensorComponent : public PollingSensorComponent {
 public:
  /// Construct the ADCSensor with the provided pin and update interval in ms.
  explicit ADCSensorComponent(const std::string &name, GPIOInputPin pin, uint32_t update_interval = 60000);

#ifdef ARDUINO_ARCH_ESP32
  /// Set the attenuation for this pin. Only available on the ESP32.
  void set_attenuation(adc_attenuation_t attenuation);
#endif

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Get the pin used for this ADC sensor.
#ifdef ARDUINO_ARCH_ESP32
  /// Get the attenuation used for this sensor.
  adc_attenuation_t get_attenuation() const;
#endif

  /// Update adc values.
  void update() override;
  /// Setup ADc
  void setup() override;
  void dump_config() override;
  /// Unit of measurement: "V".
  std::string unit_of_measurement() override;
  /// Icon: "mdi:flash".
  std::string icon() override;
  /// Accuracy decimals: 2.
  int8_t accuracy_decimals() override;
  /// `HARDWARE_LATE` setup priority.
  float get_setup_priority() const override;

#ifdef ARDUINO_ARCH_ESP8266
  std::string unique_id() override;
#endif

 protected:
  GPIOInputPin pin_;

#ifdef ARDUINO_ARCH_ESP32
  adc_attenuation_t attenuation_{ADC_0db};
#endif
};

}  // namespace sensor

ESPHOME_NAMESPACE_END

#endif  // USE_ADC_SENSOR

#endif  // ESPHOME_SENSOR_ADC_SENSOR_COMPONENT_H
