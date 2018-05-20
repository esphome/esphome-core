//
//  mqtt_cover.h
//  esphomelib
//
//  Created by Otto Winter on 20.05.18.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_COVER_MQTT_COVER_COMPONENT_H
#define ESPHOMELIB_COVER_MQTT_COVER_COMPONENT_H

#include "esphomelib/mqtt/mqtt_component.h"
#include "esphomelib/cover/cover.h"
#include "esphomelib/defines.h"

#ifdef USE_COVER

ESPHOMELIB_NAMESPACE_BEGIN

namespace cover {

class MQTTCoverComponent : public mqtt::MQTTComponent {
 public:
  explicit MQTTCoverComponent(Cover *cover);

  void setup() override;
  void send_discovery(JsonBuffer &buffer, JsonObject &root, mqtt::SendDiscoveryConfig &config) override;

 protected:
  std::string component_type() const override;
  std::string friendly_name() const override;

  Cover *cover_;
};

} // namespace cover

ESPHOMELIB_NAMESPACE_END

#endif //USE_COVER

#endif //ESPHOMELIB_COVER_MQTT_COVER_COMPONENT_H
