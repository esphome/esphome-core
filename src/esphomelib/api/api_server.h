#ifndef ESPHOMELIB_HOMEASSISTANT_API_SERVER_H
#define ESPHOMELIB_HOMEASSISTANT_API_SERVER_H

#include "esphomelib/defines.h"

#ifdef USE_API

#include "esphomelib/component.h"
#include "esphomelib/controller.h"
#include "esphomelib/api/util.h"
#include "esphomelib/api/api_message.h"
#include "esphomelib/api/basic_messages.h"
#include "esphomelib/api/list_entities.h"
#include "esphomelib/api/subscribe_state.h"
#include "esphomelib/api/subscribe_logs.h"
#include "esphomelib/api/command_messages.h"
#include "esphomelib/api/service_call_message.h"
#include "esphomelib/log.h"

#ifdef ARDUINO_ARCH_ESP32
  #include <AsyncTCP.h>
#endif
#ifdef ARDUINO_ARCH_ESP8266
  #include <ESPAsyncTCP.h>
#endif

ESPHOMELIB_NAMESPACE_BEGIN

namespace api {

class APIServer;

class APIConnection {
 public:
  APIConnection(AsyncClient *client, APIServer *parent);
  ~APIConnection();

  void disconnect_client_();
  bool send_buffer(APIMessageType type, APIBuffer &buf);
  template<typename T>
  bool send_buffer(T func, APIMessageType type);
  bool send_message(APIMessage &msg);
  bool send_message_resize(APIMessage &msg);
  bool send_empty_message(APIMessageType type);
  void loop();

#ifdef USE_BINARY_SENSOR
  bool send_binary_sensor_state(binary_sensor::BinarySensor *binary_sensor, bool state);
#endif
#ifdef USE_COVER
  bool send_cover_state(cover::Cover *cover);
#endif
#ifdef USE_FAN
  bool send_fan_state(fan::FanState *fan);
#endif
#ifdef USE_LIGHT
  bool send_light_state(light::LightState *light);
#endif
#ifdef USE_SENSOR
  bool send_sensor_state(sensor::Sensor *sensor, float state);
#endif
#ifdef USE_SWITCH
  bool send_switch_state(switch_::Switch *switch_, bool state);
#endif
#ifdef USE_TEXT_SENSOR
  bool send_text_sensor_state(text_sensor::TextSensor *text_sensor, std::string state);
#endif
  bool send_log_message(int level, const char *tag, const char *line);
  bool send_disconnect_request(const char *reason);
  bool send_ping_request();
  void send_service_call(ServiceCallResponse &call);

 protected:
  void on_error_(int8_t error);
  void on_disconnect_();
  void on_timeout_(uint32_t time);
  void on_data_(uint8_t *buf, size_t len);
  void fatal_error_();
  bool valid_rx_message_type_();
  void read_message_();

  // request types
  void on_hello_request_(const HelloRequest &req);
  void on_connect_request_(const ConnectRequest &req);
  void on_disconnect_request_(const DisconnectRequest &req);
  void on_disconnect_response_(const DisconnectResponse &req);
  void on_ping_request_(const PingRequest &req);
  void on_ping_response_(const PingResponse &req);
  void on_device_info_request_(const DeviceInfoRequest &req);
  void on_list_entities_request_(const ListEntitiesRequest &req);
  void on_subscribe_states_request_(const SubscribeStatesRequest &req);
  void on_subscribe_logs_request_(const SubscribeLogsRequest &req);
#ifdef USE_COVER
  void on_cover_command_request_(const CoverCommandRequest &req);
#endif
#ifdef USE_FAN
  void on_fan_command_request_(const FanCommandRequest &req);
#endif
#ifdef USE_LIGHT
  void on_light_command_request_(const LightCommandRequest &req);
#endif
#ifdef USE_SWITCH
  void on_switch_command_request_(const SwitchCommandRequest &req);
#endif
  void on_subscribe_service_calls_request(const SubscribeServiceCallsRequest &req);
  void on_subscribe_home_assistant_states_request(const SubscribeHomeAssistantStatesRequest &req);
  void on_home_assistant_state_response(const HomeAssistantStateResponse &req);

  void resize_buffer_();

  enum class ConnectionState {
    WAITING_FOR_HELLO,
    WAITING_FOR_CONNECT,
    CONNECTED,
  } connection_state_{ConnectionState::WAITING_FOR_HELLO};

  AsyncClient *client_;
  APIServer *parent_;

  uint8_t *buffer_;
  size_t buffer_size_;
  enum class ParseMessageState {
    PREAMBLE,
    LENGTH_FIELD,
    TYPE_FIELD,
    MESSAGE_FIELD,
    SKIP_MESSAGE_FIELD,
  } parse_state_{ParseMessageState::PREAMBLE};
  uint32_t rx_buffer_at_{0};

  APIMessageType message_type_;
  size_t message_length_;
  std::string client_info_;
  ListEntitiesIterator list_entities_iterator_;
  InitialStateIterator initial_state_iterator_;

  bool state_subscription_{false};
  int log_subscription_{ESPHOMELIB_LOG_LEVEL_NONE};
  uint32_t last_traffic_;
  bool sent_ping_{false};
  bool service_call_subscription_{false};
};

template<typename T>
class HomeAssistantServiceCallAction;

class APIServer : public Component, public StoringUpdateListenerController {
 public:
  APIServer();
  void setup() override;
  uint16_t get_port() const;
  float get_setup_priority() const override;
  void loop() override;
  void dump_config() override;
  bool check_password(const std::string &password) const;
  bool uses_password() const;
  void set_port(uint16_t port);
  void set_password(const std::string &password);
  void handle_disconnect(APIConnection *conn);
#ifdef USE_BINARY_SENSOR
  void on_binary_sensor_update(binary_sensor::BinarySensor *obj, bool state) override;
#endif
#ifdef USE_COVER
  void on_cover_update(cover::Cover *obj) override;
#endif
#ifdef USE_FAN
  void on_fan_update(fan::FanState *obj) override;
#endif
#ifdef USE_LIGHT
  void on_light_update(light::LightState *obj) override;
#endif
#ifdef USE_SENSOR
  void on_sensor_update(sensor::Sensor *obj, float state) override;
#endif
#ifdef USE_SWITCH
  void on_switch_update(switch_::Switch *obj, bool state) override;
#endif
#ifdef USE_TEXT_SENSOR
  void on_text_sensor_update(text_sensor::TextSensor *obj, std::string state) override;
#endif
  void send_service_call(ServiceCallResponse &call);
  template<typename T>
  HomeAssistantServiceCallAction<T> *make_home_assistant_service_call_action();

  struct HomeAssistantStateSubscription {
    std::string entity_id;
    std::function<void(std::string)> callback;
  };

  void subscribe_home_assistant_state(std::string entity_id, std::function<void(std::string)> f);
  const std::vector<HomeAssistantStateSubscription> &get_state_subs() const;

 protected:
  AsyncServer server_{0};
  uint16_t port_{6053};
  std::vector<APIConnection *> clients_;
  std::string password_;
  std::vector<HomeAssistantStateSubscription> state_subs_;
};

extern APIServer *global_api_server;

template<typename T>
class HomeAssistantServiceCallAction : public Action<T> {
 public:
  HomeAssistantServiceCallAction(APIServer *parent) : parent_(parent) {}
  void set_service(const std::string &service);
  void set_data(const std::vector<KeyValuePair> &data);
  void set_data_template(const std::vector<KeyValuePair> &data_template);
  void set_variables(const std::vector<TemplatableKeyValuePair> &variables);
  void play(T x) override;
 protected:
  APIServer *parent_;
  ServiceCallResponse resp_;
};

template<typename T>
HomeAssistantServiceCallAction<T> *APIServer::make_home_assistant_service_call_action() {
  return new HomeAssistantServiceCallAction<T>(this);
}

template<typename T>
void HomeAssistantServiceCallAction<T>::set_service(const std::string &service) {
  this->resp_.set_service(service);
}
template<typename T>
void HomeAssistantServiceCallAction<T>::set_data(const std::vector<KeyValuePair> &data) {
  this->resp_.set_data(data);
}
template<typename T>
void HomeAssistantServiceCallAction<T>::set_data_template(const std::vector<KeyValuePair> &data_template) {
  this->resp_.set_data_template(data_template);
}
template<typename T>
void HomeAssistantServiceCallAction<T>::set_variables(const std::vector<TemplatableKeyValuePair> &variables) {
  this->resp_.set_variables(variables);
}
template<typename T>
void HomeAssistantServiceCallAction<T>::play(T x) {
  this->parent_->send_service_call(this->resp_);
  this->play_next(x);
}

template<typename T>
bool APIConnection::send_buffer(T func, APIMessageType type) {
  APIBuffer buf(this->buffer_, this->buffer_size_);
  func(buf);
  return this->send_buffer(type, buf);
}

} // namespace api

ESPHOMELIB_NAMESPACE_END

#endif //USE_API

#endif //ESPHOMELIB_HOMEASSISTANT_API_SERVER_H
