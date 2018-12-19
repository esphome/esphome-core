#include "esphomelib/defines.h"

#ifdef USE_API

#include "esphomelib/api/api_server.h"
#include "esphomelib/api/basic_messages.h"
#include "esphomelib/log.h"
#include "esphomelib/application.h"
#include "esphomelib/deep_sleep_component.h"
#include "esphomelib/time/homeassistant_time.h"

#include <algorithm>

ESPHOMELIB_NAMESPACE_BEGIN

namespace api {

static const char *TAG = "api";

// APIServer
void APIServer::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Home Assistant API server...");
  this->server_ = AsyncServer(this->port_);
  this->server_.setNoDelay(false);
  this->server_.begin();
  this->server_.onClient([](void *s, AsyncClient *client) {
    if (client == nullptr)
      return;

    // can't print here because in lwIP thread
    // ESP_LOGD(TAG, "New client connected from %s", client->remoteIP().toString().c_str());
    auto *this_ = (APIServer *) s;
    this_->clients_.push_back(new APIConnection(client, this_));
  }, this);
  if (global_log_component != nullptr) {
    global_log_component->add_on_log_callback([this](int level, const char *tag, const char *message) {
      for (auto *c : this->clients_) {
        c->send_log_message(level, tag, message);
      }
    });
  }

  add_shutdown_hook([this] (const char *reason) {
    for (auto *c : this->clients_) {
      c->send_disconnect_request(reason);
    }

    delay(10);
  });
}
void APIServer::loop() {
  for (auto *client : this->clients_) {
    client->loop();
  }
}
void APIServer::dump_config() {

}
bool APIServer::uses_password() const {
  return !this->password_.empty();
}
bool APIServer::check_password(const std::string &password) const {
  // depend only on input password length
  const char *a = this->password_.c_str();
  uint32_t len_a = this->password_.length();
  const char *b = password.c_str();
  uint32_t len_b = password.length();

  // disable optimization with volatile
  volatile uint32_t length = len_b;
  volatile const char *left = nullptr;
  volatile const char *right = b;
  uint8_t result = 0;

  if (len_a == length) {
    left = *((volatile const char **) &a);
    result = 0;
  }
  if (len_a != length) {
    left = b;
    result = 1;
  }

  for (size_t i = 0; i < length; i++) {
    result |= *left++ ^ *right++;
  }

  return result == 0;
}
void APIServer::handle_disconnect(APIConnection *conn) {
  this->clients_.erase(
      std::remove_if(this->clients_.begin(), this->clients_.end(),
                     [conn](APIConnection *conn2) {
                       return conn2 == conn;
                     }),
      this->clients_.end()
  );

  delete conn;
}
#ifdef USE_BINARY_SENSOR
void APIServer::on_binary_sensor_update(binary_sensor::BinarySensor *obj, bool state) {
  if (obj->is_internal()) return;
  for (auto *c : this->clients_)
    c->send_binary_sensor_state(obj, state);
}
#endif

#ifdef USE_COVER
void APIServer::on_cover_update(cover::Cover *obj) {
  if (obj->is_internal()) return;
  for (auto *c : this->clients_)
    c->send_cover_state(obj);
}
#endif

#ifdef USE_FAN
void APIServer::on_fan_update(fan::FanState *obj) {
  if (obj->is_internal()) return;
  for (auto *c : this->clients_)
    c->send_fan_state(obj);
}
#endif

#ifdef USE_LIGHT
void APIServer::on_light_update(light::LightState *obj) {
  if (obj->is_internal()) return;
  for (auto *c : this->clients_)
    c->send_light_state(obj);
}
#endif

#ifdef USE_SENSOR
void APIServer::on_sensor_update(sensor::Sensor *obj, float state) {
  if (obj->is_internal()) return;
  for (auto *c : this->clients_)
    c->send_sensor_state(obj, state);
}
#endif

#ifdef USE_SWITCH
void APIServer::on_switch_update(switch_::Switch *obj, bool state) {
  if (obj->is_internal()) return;
  for (auto *c : this->clients_)
    c->send_switch_state(obj, state);
}
#endif

#ifdef USE_TEXT_SENSOR
void APIServer::on_text_sensor_update(text_sensor::TextSensor *obj, std::string state) {
  if (obj->is_internal()) return;
  for (auto *c : this->clients_)
    c->send_text_sensor_state(obj, state);
}
#endif
float APIServer::get_setup_priority() const {
  return setup_priority::WIFI - 1.0f;
}
void APIServer::set_port(uint16_t port) {
  this->port_ = port;
}
APIServer *global_api_server = nullptr;

void APIServer::set_password(const std::string &password) {
  this->password_ = password;
}
void APIServer::send_service_call(ServiceCallResponse &call) {
  for (auto *client : this->clients_) {
    client->send_service_call(call);
  }
}
APIServer::APIServer() {
  global_api_server = this;
}
void APIServer::subscribe_home_assistant_state(std::string entity_id, std::function<void(std::string)> f) {
  this->state_subs_.push_back(HomeAssistantStateSubscription{
      .entity_id = entity_id,
      .callback = f,
  });
}
const std::vector<APIServer::HomeAssistantStateSubscription> &APIServer::get_state_subs() const {
  return this->state_subs_;
}
uint16_t APIServer::get_port() const {
  return this->port_;
}

// APIConnection
APIConnection::APIConnection(AsyncClient *client, APIServer *parent)
    : client_(client), parent_(parent), initial_state_iterator_(parent, this),
      list_entities_iterator_(parent, this) {
  this->client_->onError([](void *s, AsyncClient *c, int8_t error) {
    ((APIConnection *) s)->on_error_(error);
  }, this);
  this->client_->onDisconnect([](void *s, AsyncClient *c) {
    ((APIConnection *) s)->on_disconnect_();
  }, this);
  this->client_->onTimeout([](void *s, AsyncClient *c, uint32_t time) {
    ((APIConnection *) s)->on_timeout_(time);
  }, this);
  this->client_->onData([](void *s, AsyncClient *c, void *buf, size_t len) {
    ((APIConnection *) s)->on_data_(reinterpret_cast<uint8_t *>(buf), len);
  }, this);

  this->buffer_size_ = 128;
  this->buffer_ = new uint8_t[this->buffer_size_];
  this->client_info_ = this->client_->remoteIP().toString().c_str();
  this->last_traffic_ = millis();
}
APIConnection::~APIConnection() {
  delete [] this->buffer_;
  delete this->client_;
}
void APIConnection::on_error_(int8_t error) {
  ESP_LOGD(TAG, "Error from client '%s': %d", this->client_info_.c_str(), error);
  // disconnect will also be called, nothing to do here
}
void APIConnection::on_disconnect_() {
  ESP_LOGD(TAG, "'%s' disconnected.", this->client_info_.c_str());
  // delete self, generally unsafe but not in this case.
  this->parent_->handle_disconnect(this);
}
void APIConnection::on_timeout_(uint32_t time) {
  ESP_LOGV(TAG, "Timeout from client");
  this->disconnect_client_();
}
void APIConnection::resize_buffer_() {
  delete [] this->buffer_;
  this->buffer_size_ *= 2;
  this->buffer_ = new uint8_t[this->buffer_size_];
}
void APIConnection::on_data_(uint8_t *buf, size_t len) {
  if (len == 0 || buf == nullptr)
    return;

  for (size_t i = 0; i < len; i++) {
    uint8_t val = buf[i];
    const uint32_t buffer_size = this->buffer_size_;
    const uint32_t buffer_at = this->rx_buffer_at_;

    if (buffer_at >= buffer_size && this->parse_state_ != ParseMessageState::SKIP_MESSAGE_FIELD) {
      this->fatal_error_();
      return;
    } else if (this->parse_state_ != ParseMessageState::SKIP_MESSAGE_FIELD) {
      this->buffer_[buffer_at] = val;
    }

    this->rx_buffer_at_ += 1;

    switch (this->parse_state_) {
      case ParseMessageState::PREAMBLE: {
        if (val != 0x00) {
          ESP_LOGW(TAG, "Invalid preamble (%02X)!", val);
          this->fatal_error_();
          return;
        }
        this->parse_state_ = ParseMessageState::LENGTH_FIELD;
        this->rx_buffer_at_ = 0;
        break;
      }
      case ParseMessageState::LENGTH_FIELD: {
        auto res = proto_decode_varuint32(this->buffer_, buffer_at + 1);
        if (res.has_value()) {
          this->message_length_ = *res;
          this->parse_state_ = ParseMessageState::TYPE_FIELD;
        }
        this->rx_buffer_at_ = 0;
        break;
      }
      case ParseMessageState::TYPE_FIELD: {
        auto res = proto_decode_varuint32(this->buffer_, buffer_at + 1);
        if (res.has_value()) {
          this->message_type_ = static_cast<APIMessageType>(*res);

          if (!this->valid_rx_message_type_()) {
            ESP_LOGE(TAG, "Not a valid message type: %u", *res);
            this->fatal_error_();
            return;
          }

          if (this->message_length_ == 0) {
            this->read_message_();
            this->parse_state_ = ParseMessageState::PREAMBLE;
          } else if (this->message_length_ >= buffer_size) {
            this->parse_state_ = ParseMessageState::SKIP_MESSAGE_FIELD;
            ESP_LOGW(TAG, "Message too long, can't parse: %u", *res);
          } else {
            this->parse_state_ = ParseMessageState::MESSAGE_FIELD;
          }
          this->rx_buffer_at_ = 0;
        }
        break;
      }
      case ParseMessageState::MESSAGE_FIELD: {
        if (buffer_at + 1 == this->message_length_) {
          this->read_message_();
          this->rx_buffer_at_ = 0;
          this->parse_state_ = ParseMessageState::PREAMBLE;
        }
        break;
      }
      case ParseMessageState::SKIP_MESSAGE_FIELD:
        if (buffer_at + 1 == this->message_length_) {
          this->rx_buffer_at_ = 0;
          this->parse_state_ = ParseMessageState::PREAMBLE;
        }
        break;
    }
  }
}
void APIConnection::read_message_() {
  this->last_traffic_ = millis();

  switch (this->message_type_) {
    case APIMessageType::HELLO_REQUEST: {
      HelloRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_hello_request_(req);
      break;
    }
    case APIMessageType::HELLO_RESPONSE: {
      // Invalid
      break;
    }
    case APIMessageType::CONNECT_REQUEST: {
      ConnectRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_connect_request_(req);
      break;
    }
    case APIMessageType::CONNECT_RESPONSE:
      // Invalid
      break;
    case APIMessageType::DISCONNECT_REQUEST: {
      DisconnectRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_disconnect_request_(req);
      break;
    }
    case APIMessageType::DISCONNECT_RESPONSE: {
      DisconnectResponse req;
      req.decode(this->buffer_, this->message_length_);
      this->on_disconnect_response_(req);
      break;
    }
    case APIMessageType::PING_REQUEST: {
      PingRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_ping_request_(req);
      break;
    }
    case APIMessageType::PING_RESPONSE: {
      PingResponse req;
      req.decode(this->buffer_, this->message_length_);
      this->on_ping_response_(req);
      break;
    }
    case APIMessageType::DEVICE_INFO_REQUEST: {
      DeviceInfoRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_device_info_request_(req);
      break;
    }
    case APIMessageType::DEVICE_INFO_RESPONSE: {
      // Invalid
      break;
    }
    case APIMessageType::LIST_ENTITIES_REQUEST: {
      ListEntitiesRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_list_entities_request_(req);
      break;
    }
    case APIMessageType::LIST_ENTITIES_BINARY_SENSOR_RESPONSE:
    case APIMessageType::LIST_ENTITIES_COVER_RESPONSE:
    case APIMessageType::LIST_ENTITIES_FAN_RESPONSE:
    case APIMessageType::LIST_ENTITIES_LIGHT_RESPONSE:
    case APIMessageType::LIST_ENTITIES_SENSOR_RESPONSE:
    case APIMessageType::LIST_ENTITIES_SWITCH_RESPONSE:
    case APIMessageType::LIST_ENTITIES_TEXT_SENSOR_RESPONSE:
    case APIMessageType::LIST_ENTITIES_DONE_RESPONSE:
      // Invalid
      break;
    case APIMessageType::SUBSCRIBE_STATES_REQUEST: {
      SubscribeStatesRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_subscribe_states_request_(req);
      break;
    }
    case APIMessageType::BINARY_SENSOR_STATE_RESPONSE:
    case APIMessageType::COVER_STATE_RESPONSE:
    case APIMessageType::FAN_STATE_RESPONSE:
    case APIMessageType::LIGHT_STATE_RESPONSE:
    case APIMessageType::SENSOR_STATE_RESPONSE:
    case APIMessageType::SWITCH_STATE_RESPONSE:
    case APIMessageType::TEXT_SENSOR_STATE_RESPONSE:
      // Invalid
      break;
    case APIMessageType::SUBSCRIBE_LOGS_REQUEST: {
      SubscribeLogsRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_subscribe_logs_request_(req);
      break;
    }
    case APIMessageType ::SUBSCRIBE_LOGS_RESPONSE:
      // Invalid
      break;
    case APIMessageType::COVER_COMMAND_REQUEST: {
#ifdef USE_COVER
      CoverCommandRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_cover_command_request_(req);
#endif
      break;
    }
    case APIMessageType::FAN_COMMAND_REQUEST: {
#ifdef USE_FAN
      FanCommandRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_fan_command_request_(req);
#endif
      break;
    }
    case APIMessageType::LIGHT_COMMAND_REQUEST: {
#ifdef USE_LIGHT
      LightCommandRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_light_command_request_(req);
#endif
      break;
    }
    case APIMessageType::SWITCH_COMMAND_REQUEST: {
#ifdef USE_SWITCH
      SwitchCommandRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_switch_command_request_(req);
#endif
      break;
    }
    case APIMessageType::SUBSCRIBE_SERVICE_CALLS_REQUEST: {
      SubscribeServiceCallsRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_subscribe_service_calls_request(req);
      break;
    }
    case APIMessageType::SERVICE_CALL_RESPONSE:
      // Invalid
      break;
    case APIMessageType::GET_TIME_REQUEST:
      // Invalid
      break;
    case APIMessageType::GET_TIME_RESPONSE: {
#ifdef USE_HOMEASSISTANT_TIME
      time::GetTimeResponse req;
      req.decode(this->buffer_, this->message_length_);
#endif
      break;
    }
    case APIMessageType::SUBSCRIBE_HOME_ASSISTANT_STATES_REQUEST: {
      SubscribeHomeAssistantStatesRequest req;
      req.decode(this->buffer_, this->message_length_);
      this->on_subscribe_home_assistant_states_request(req);
      break;
    }
    case APIMessageType::SUBSCRIBE_HOME_ASSISTANT_STATE_RESPONSE:
      // Invalid
      break;
    case APIMessageType::HOME_ASSISTANT_STATE_RESPONSE: {
      HomeAssistantStateResponse req;
      req.decode(this->buffer_, this->message_length_);
      this->on_home_assistant_state_response(req);
      break;
    }
  }
}
void APIConnection::on_hello_request_(const HelloRequest &req) {
  ESP_LOGVV(TAG, "on_hello_request_(client_info='%s')", req.get_client_info().c_str());
  this->client_info_ = req.get_client_info() + " (" + this->client_->remoteIP().toString().c_str();
  this->client_info_ += ")";
  ESP_LOGV(TAG, "Hello from client: '%s'", this->client_info_.c_str());

  bool success = this->send_buffer([](APIBuffer &buffer) {
    // uint32 api_version_major = 1; -> 1
    buffer.encode_uint32(1, 1);
    // uint32 api_version_minor = 2; -> 0
    buffer.encode_uint32(2, 0);

    // string server_info = 3;
    buffer.encode_string(3, App.get_name() + " (esphomelib v" ESPHOMELIB_VERSION ")");
  }, APIMessageType::HELLO_RESPONSE);
  if (!success) {
    this->fatal_error_();
    return;
  }

  this->connection_state_ = ConnectionState::WAITING_FOR_CONNECT;
}
void APIConnection::on_connect_request_(const ConnectRequest &req) {
  ESP_LOGVV(TAG, "on_connect_request_(password='%s')", req.get_password().c_str());
  bool correct = this->parent_->check_password(req.get_password());
  bool success = this->send_buffer([correct](APIBuffer &buffer) {
    // bool invalid_password = 1;
    buffer.encode_bool(1, !correct);
  }, APIMessageType::CONNECT_RESPONSE);
  if (!success) {
    this->fatal_error_();
    return;
  }

  if (correct) {
    ESP_LOGD(TAG, "Client '%s' connected successfully!", this->client_info_.c_str());
    this->connection_state_ = ConnectionState::CONNECTED;

#ifdef USE_HOMEASSISTANT_TIME
    if (time::global_homeassistant_time != nullptr) {
      this->send_empty_message(APIMessageType::GET_TIME_REQUEST);
    }
#endif
  }
}
void APIConnection::on_disconnect_request_(const DisconnectRequest &req) {
  ESP_LOGVV(TAG, "on_disconnect_request_");
  // remote initiated disconnect_client_
  if (!this->send_empty_message(APIMessageType::DISCONNECT_RESPONSE)) {
    this->fatal_error_();
    return;
  }
  this->disconnect_client_();
}
void APIConnection::on_disconnect_response_(const DisconnectResponse &req) {
  ESP_LOGVV(TAG, "on_disconnect_response_");
  // we initiated disconnect_client_
  this->disconnect_client_();
}
void APIConnection::on_ping_request_(const PingRequest &req) {
  ESP_LOGVV(TAG, "on_ping_request_");
  PingResponse resp;
  this->send_message(resp);
}
void APIConnection::on_ping_response_(const PingResponse &req) {
  ESP_LOGVV(TAG, "on_ping_response_");
  // we initiated ping
  this->sent_ping_ = false;
}
void APIConnection::on_device_info_request_(const DeviceInfoRequest &req) {
  ESP_LOGVV(TAG, "on_device_info_request_");
  this->send_buffer([this](APIBuffer &buffer) {
    // bool uses_password = 1;
    buffer.encode_bool(1, this->parent_->uses_password());
    // string name = 2;
    buffer.encode_string(2, App.get_name());
    // string mac_address = 3;
    buffer.encode_string(3, get_mac_address_pretty());
    // string esphomelib_version = 4;
    buffer.encode_string(4, ESPHOMELIB_VERSION);
    // string compilation_time = 5;
    buffer.encode_string(5, App.get_compilation_time());
#ifdef ARDUINO_BOARD
    // string model = 6;
    buffer.encode_string(6, ARDUINO_BOARD);
#endif
#ifdef USE_DEEP_SLEEP
    // bool has_deep_sleep = 8;
    buffer.encode_bool(8, global_has_deep_sleep);
#endif
  }, APIMessageType::DEVICE_INFO_RESPONSE);
}
void APIConnection::on_list_entities_request_(const ListEntitiesRequest &req) {
  ESP_LOGVV(TAG, "on_list_entities_request_");
  this->list_entities_iterator_.begin();
}
void APIConnection::on_subscribe_states_request_(const SubscribeStatesRequest &req) {
  ESP_LOGVV(TAG, "on_subscribe_states_request_");
  this->state_subscription_ = true;
  this->initial_state_iterator_.begin();
}
void APIConnection::on_subscribe_logs_request_(const SubscribeLogsRequest &req) {
  ESP_LOGVV(TAG, "on_subscribe_logs_request_");
  this->log_subscription_ = req.get_level();
  if (req.get_dump_config()) {
    App.schedule_dump_config();
  }
}


void APIConnection::fatal_error_() {
  this->client_->close();
  this->disconnect_client_();
}
bool APIConnection::valid_rx_message_type_() {
  switch (this->message_type_) {
    case APIMessageType::HELLO_RESPONSE:
    case APIMessageType::CONNECT_RESPONSE:
      return false;
    case APIMessageType::HELLO_REQUEST:
      return this->connection_state_ == ConnectionState::WAITING_FOR_HELLO;
    case APIMessageType::CONNECT_REQUEST:
      return this->connection_state_ == ConnectionState::WAITING_FOR_CONNECT;
    case APIMessageType::PING_REQUEST:
    case APIMessageType::PING_RESPONSE:
    case APIMessageType::DISCONNECT_REQUEST:
    case APIMessageType::DISCONNECT_RESPONSE:
    case APIMessageType::DEVICE_INFO_REQUEST:
      if (this->connection_state_ == ConnectionState::WAITING_FOR_CONNECT)
        return true;
    default:
      return this->connection_state_ == ConnectionState::CONNECTED;
  }
}
bool APIConnection::send_message(APIMessage &msg) {
  APIBuffer buf(this->buffer_, this->buffer_size_);
  msg.encode(buf);
  return this->send_buffer(msg.message_type(), buf);
}
bool APIConnection::send_message_resize(APIMessage &msg) {
  while (true) {
    APIBuffer buf(this->buffer_, this->buffer_size_);
    msg.encode(buf);
    if (buf.get_overflow()) {
      this->resize_buffer_();
    } else {
      return this->send_buffer(msg.message_type(), buf);
    }
  }
}
bool APIConnection::send_empty_message(APIMessageType type) {
  APIBuffer buf(this->buffer_, this->buffer_size_);
  return this->send_buffer(type, buf);
}

void APIConnection::disconnect_client_() {
  this->client_->close();
}

bool APIConnection::send_buffer(APIMessageType type, APIBuffer &buf) {
  if (buf.get_overflow()) {
    if (type != APIMessageType::SUBSCRIBE_LOGS_RESPONSE) {
      ESP_LOGV(TAG, "Cannot send message because of buffer overflow");
    }
    return false;
  }

  size_t needed_space = buf.get_length();
  uint8_t header_raw[15];
  APIBuffer header_buffer(header_raw, sizeof(header_raw));
  header_buffer.write(0x00);
  header_buffer.encode_varint_(buf.get_length());
  header_buffer.encode_varint_(static_cast<uint32_t>(type));
  needed_space += header_buffer.get_length();

  if (needed_space > this->client_->space()) {
    if (type != APIMessageType::SUBSCRIBE_LOGS_RESPONSE) {
      ESP_LOGV(TAG, "Cannot send message because of TCP buffer space");
    }
    return false;
  }

  this->client_->add(reinterpret_cast<char *>(header_raw), header_buffer.get_length());
  this->client_->add(reinterpret_cast<char *>(this->buffer_), buf.get_length());
  this->client_->send();
  return true;
}

void APIConnection::loop() {
  if (this->client_->disconnected()) {
    // failsave for disconnect logic
    this->on_disconnect_();
  }

  this->list_entities_iterator_.advance();
  this->initial_state_iterator_.advance();
  const uint32_t keepalive = 60000;
  if (this->sent_ping_) {
    if (millis() - this->last_traffic_ > (keepalive * 3) / 2) {
      ESP_LOGW(TAG, "'%s' didn't respond to ping request in time. Disconnecting...",
               this->client_info_.c_str());
      this->disconnect_client_();
    }
  } else if (millis() - this->last_traffic_ > keepalive) {
    this->sent_ping_ = true;
    this->send_ping_request();
  }
}

#ifdef USE_BINARY_SENSOR
bool APIConnection::send_binary_sensor_state(binary_sensor::BinarySensor *binary_sensor, bool state) {
  if (!this->state_subscription_)
    return false;

  return this->send_buffer([binary_sensor, state](APIBuffer &buffer) {
    // fixed32 key = 1;
    buffer.encode_fixed32(1, binary_sensor->get_object_id_hash());
    // bool state = 2;
    buffer.encode_bool(2, state);
  }, APIMessageType::BINARY_SENSOR_STATE_RESPONSE);
}
#endif

#ifdef USE_COVER
bool APIConnection::send_cover_state(cover::Cover *cover) {
  if (!this->state_subscription_)
    return false;

  return this->send_buffer([cover](APIBuffer &buffer) {
    // fixed32 key = 1;
    buffer.encode_fixed32(1, cover->get_object_id_hash());
    // enum CoverState {
    //   OPEN = 0;
    //   CLOSED = 1;
    // }
    // CoverState state = 2;
    uint32_t state = (cover->state == cover::COVER_OPEN) ? 0 : 1;
    buffer.encode_uint32(2, state);
  }, APIMessageType::COVER_STATE_RESPONSE);
}
#endif

#ifdef USE_FAN
bool APIConnection::send_fan_state(fan::FanState *fan) {
  if (!this->state_subscription_)
    return false;

  return this->send_buffer([fan](APIBuffer &buffer) {
    // fixed32 key = 1;
    buffer.encode_fixed32(1, fan->get_object_id_hash());
    // bool state = 2;
    buffer.encode_bool(2, fan->state);
    // bool oscillating = 3;
    if (fan->get_traits().supports_oscillation()) {
      buffer.encode_bool(3, fan->oscillating);
    }
    // enum FanSpeed {
    //   LOW = 0;
    //   MEDIUM = 1;
    //   HIGH = 2;
    // }
    // FanSpeed speed = 4;
    if (fan->get_traits().supports_speed()) {
      buffer.encode_uint32(4, fan->speed);
    }
  }, APIMessageType::FAN_STATE_RESPONSE);
}
#endif

#ifdef USE_LIGHT
bool APIConnection::send_light_state(light::LightState *light) {
  if (!this->state_subscription_)
    return false;

  return this->send_buffer([light](APIBuffer &buffer) {
    light::LightTraits traits = light->get_traits();
    light::LightColorValues values = light->get_remote_values();

    // fixed32 key = 1;
    buffer.encode_fixed32(1, light->get_object_id_hash());
    // bool state = 2;
    buffer.encode_bool(2, values.get_state() != 0.0f);
    // float brightness = 3;
    if (traits.has_brightness()) {
      buffer.encode_float(3, values.get_brightness());
    }
    if (traits.has_rgb()) {
      // float red = 4;
      buffer.encode_float(4, values.get_red());
      // float green = 5;
      buffer.encode_float(5, values.get_green());
      // float blue = 6;
      buffer.encode_float(6, values.get_blue());
    }
    // float white = 7;
    if (traits.has_rgb_white_value()) {
      buffer.encode_float(7, values.get_white());
    }
    // float color_temperature = 8;
    if (traits.has_color_temperature()) {
      buffer.encode_float(8, values.get_color_temperature());
    }
    // string effect = 9;
    if (light->supports_effects()) {
      buffer.encode_string(9, light->get_effect_name());
    }
  }, APIMessageType::LIGHT_STATE_RESPONSE);
}
#endif

#ifdef USE_SENSOR
bool APIConnection::send_sensor_state(sensor::Sensor *sensor, float state) {
  if (!this->state_subscription_)
    return false;

  return this->send_buffer([sensor, state](APIBuffer &buffer) {
    // fixed32 key = 1;
    buffer.encode_fixed32(1, sensor->get_object_id_hash());
    // float state = 2;
    buffer.encode_float(2, state);
  }, APIMessageType::SENSOR_STATE_RESPONSE);
}
#endif

#ifdef USE_SWITCH
bool APIConnection::send_switch_state(switch_::Switch *switch_, bool state) {
  if (!this->state_subscription_)
    return false;

  return this->send_buffer([switch_, state](APIBuffer &buffer) {
    // fixed32 key = 1;
    buffer.encode_fixed32(1, switch_->get_object_id_hash());
    // bool state = 2;
    buffer.encode_bool(2, state);
  }, APIMessageType::SWITCH_STATE_RESPONSE);
}
#endif

#ifdef USE_TEXT_SENSOR
bool APIConnection::send_text_sensor_state(text_sensor::TextSensor *text_sensor, std::string state) {
  if (!this->state_subscription_)
    return false;

  return this->send_buffer([text_sensor, state](APIBuffer &buffer) {
    // fixed32 key = 1;
    buffer.encode_fixed32(1, text_sensor->get_object_id_hash());
    // string state = 2;
    buffer.encode_string(2, state);
  }, APIMessageType::TEXT_SENSOR_STATE_RESPONSE);
}
#endif

bool APIConnection::send_log_message(int level,
                                     const char *tag,
                                     const char *line) {
  if (this->log_subscription_ < level)
    return false;

  return this->send_buffer([level, tag, line](APIBuffer &buffer) {
    // LogLevel level = 1;
    buffer.encode_uint32(1, static_cast<uint32_t>(level));
    // string tag = 2;
    buffer.encode_string(2, tag, strlen(tag));
    // string message = 3;
    buffer.encode_string(3, line, strlen(line));
  }, APIMessageType::SUBSCRIBE_LOGS_RESPONSE);
}
bool APIConnection::send_disconnect_request(const char *reason) {
  DisconnectRequest req;
  req.set_reason(reason);
  return this->send_message(req);
}
bool APIConnection::send_ping_request() {
  ESP_LOGVV(TAG, "Sending ping...");
  PingRequest req;
  return this->send_message(req);
}

#ifdef USE_COVER
void APIConnection::on_cover_command_request_(const CoverCommandRequest &req) {
  ESP_LOGVV(TAG, "on_cover_command_request_");
  cover::Cover *cover = this->parent_->get_cover_by_key(req.get_key());
  if (cover == nullptr)
    return;

  if (req.get_command().has_value()) {
    switch (*req.get_command()) {
      case cover::COVER_COMMAND_OPEN:
        cover->open();
        break;
      case cover::COVER_COMMAND_CLOSE:
        cover->close();
        break;
      case cover::COVER_COMMAND_STOP:
        cover->stop();
        break;
      default:
        break;
    }
  }
}
#endif

#ifdef USE_FAN
void APIConnection::on_fan_command_request_(const FanCommandRequest &req) {
  ESP_LOGVV(TAG, "on_fan_command_request_");
  fan::FanState *fan = this->parent_->get_fan_by_key(req.get_key());
  if (fan == nullptr)
    return;

  auto call = fan->make_call();
  call.set_state(req.get_state());
  call.set_oscillating(req.get_oscillating());
  call.set_speed(req.get_speed());
  call.perform();
}
#endif

#ifdef USE_LIGHT
void APIConnection::on_light_command_request_(const LightCommandRequest &req) {
  ESP_LOGVV(TAG, "on_light_command_request_");
  light::LightState *light = this->parent_->get_light_by_key(req.get_key());
  if (light == nullptr)
    return;

  auto call = light->make_call();
  call.set_state(req.get_state());
  call.set_brightness(req.get_brightness());
  call.set_red(req.get_red());
  call.set_green(req.get_green());
  call.set_blue(req.get_blue());
  call.set_white(req.get_white());
  call.set_color_temperature(req.get_color_temperature());
  call.set_transition_length(req.get_transition_length());
  call.set_flash_length(req.get_flash_length());
  call.set_effect(req.get_effect());
  call.perform();
}
#endif

#ifdef USE_SWITCH
void APIConnection::on_switch_command_request_(const SwitchCommandRequest &req) {
  ESP_LOGVV(TAG, "on_switch_command_request_");
  switch_::Switch *switch_ = this->parent_->get_switch_by_key(req.get_key());
  if (switch_ == nullptr)
    return;

  if (req.get_state()) {
    switch_->turn_on();
  } else {
    switch_->turn_off();
  }
}
#endif

void APIConnection::on_subscribe_service_calls_request(const SubscribeServiceCallsRequest &req) {
  this->service_call_subscription_ = true;
}
void APIConnection::send_service_call(ServiceCallResponse &call) {
  if (!this->service_call_subscription_)
    return;

  this->send_message_resize(call);
}
void APIConnection::on_subscribe_home_assistant_states_request(const SubscribeHomeAssistantStatesRequest &req) {
  for (auto &it : this->parent_->get_state_subs()) {
    this->send_buffer([it](APIBuffer &buffer) {
      // string entity_id = 1;
      buffer.encode_string(1, it.entity_id);
    }, APIMessageType::SUBSCRIBE_HOME_ASSISTANT_STATE_RESPONSE);
  }
}
void APIConnection::on_home_assistant_state_response(const HomeAssistantStateResponse &req) {
  for (auto &it : this->parent_->get_state_subs()) {
    if (it.entity_id == req.get_entity_id()) {
      it.callback(req.get_state());
    }
  }
}

} // namespace api

ESPHOMELIB_NAMESPACE_END

#endif //USE_API
