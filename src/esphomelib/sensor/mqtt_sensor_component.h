//
// Created by Otto Winter on 26.11.17.
//

#ifndef ESPHOMELIB_SENSOR_MQTT_SENSOR_COMPONENT_H
#define ESPHOMELIB_SENSOR_MQTT_SENSOR_COMPONENT_H

#include "esphomelib/sensor/sensor.h"
#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/helpers.h"
#include "esphomelib/sensor/filter.h"
#include "esphomelib/defines.h"

#ifdef USE_SENSOR

ESPHOMELIB_NAMESPACE_BEGIN

namespace sensor {

/// Class that exposes sensors to the MQTT frontend.
class MQTTSensorComponent : public mqtt::MQTTComponent {
 public:
  /** Construct this MQTTSensorComponent instance with the provided friendly_name and sensor
   *
   * Note the sensor is never stored and is only used for initializing some values of this class.
   * If sensor is nullptr, then automatic initialization of these fields is disabled.
   *
   * @param sensor The sensor, this can be null to disable automatic setup.
   */
  explicit MQTTSensorComponent(Sensor *sensor);

  /// Setup an expiry, 0 disables it
  void set_expire_after(uint32_t expire_after);
  /// Disable Home Assistant value exiry.
  void disable_expire_after();

  void send_discovery(JsonBuffer &buffer, JsonObject &root, mqtt::SendDiscoveryConfig &config) override;

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Override setup.
  void setup() override;

  /// Get the expire_after in milliseconds used for Home Assistant discovery, first checks override.
  uint32_t get_expire_after() const;

 protected:
  /// Override for MQTTComponent, returns "sensor".
  std::string component_type() const override;

  std::string friendly_name() const override;

 protected:
  Sensor *sensor_;
  Optional<uint32_t> expire_after_; // Override the expire after advertised to Home Assistant
};

} // namespace sensor

ESPHOMELIB_NAMESPACE_END

#endif //USE_SENSOR

#endif //ESPHOMELIB_SENSOR_MQTT_SENSOR_COMPONENT_H
