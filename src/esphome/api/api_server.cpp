#include "esphome/defines.h"

#ifdef USE_API

#include "esphome/api/api_server.h"
#include "esphome/api/basic_messages.h"
#include "esphome/log.h"
#include "esphome/application.h"
#include "esphome/util.h"
#include "esphome/deep_sleep_component.h"
#include "esphome/time/homeassistant_time.h"

#include <algorithm>

ESPHOME_NAMESPACE_BEGIN

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
        if (!c->remove_)
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

  this->last_connected_ = millis();
}
void APIServer::loop() {
  // Partition clients into remove and active
  auto new_end = std::partition(this->clients_.begin(), this->clients_.end(),
                                [](APIConnection *conn) {
                                  return !conn->remove_;
                                });
  // print disconnection messages
  for (auto it = new_end; it != this->clients_.end(); ++it) {
    ESP_LOGD(TAG, "Disconnecting %s", (*it)->client_info_.c_str());
  }
  // only then delete the pointers, otherwise log routine
  // would access freed memory
  for (auto it = new_end; it != this->clients_.end(); ++it)
    delete *it;
  // resize vector
  this->clients_.erase(new_end, this->clients_.end());

  for (auto *client : this->clients_) {
    client->loop();
  }

  if (this->reboot_timeout_ != 0) {
    const uint32_t now = millis();
    if (this->clients_.empty()) {
      if (now - this->last_connected_ > this->reboot_timeout_) {
        ESP_LOGE(TAG, "No client connected to API. Rebooting...");
        reboot("api");
      }
    } else {
      this->last_connected_ = now;
    }
  }
}
void APIServer::dump_config() {
  ESP_LOGCONFIG(TAG, "API Server:");
  ESP_LOGCONFIG(TAG, "  Address: %s:%u", network_get_address().c_str(), this->port_);
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
void APIServer::set_reboot_timeout(uint32_t reboot_timeout) {
  this->reboot_timeout_ = reboot_timeout;
}
#ifdef USE_HOMEASSISTANT_TIME
void APIServer::request_time() {
  for (auto *client : this->clients_) {
    if (!client->remove_ && client->connection_state_ == APIConnection::ConnectionState::CONNECTED)
      client->send_time_request();
  }
}
#endif

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

  this->send_buffer_.reserve(64);
  this->recv_buffer_.reserve(32);
  this->client_info_ = this->client_->remoteIP().toString().c_str();
  this->last_traffic_ = millis();
}
APIConnection::~APIConnection() {
  delete this->client_;
}
void APIConnection::on_error_(int8_t error) {
  ESP_LOGD(TAG, "Error from client '%s': %d", this->client_info_.c_str(), error);
  // disconnect will also be called, nothing to do here
  this->remove_ = true;
}
void APIConnection::on_disconnect_() {
  // delete self, generally unsafe but not in this case.
  this->remove_ = true;
}
void APIConnection::on_timeout_(uint32_t time) {
  this->disconnect_client_();
}
void APIConnection::on_data_(uint8_t *buf, size_t len) {
  if (len == 0 || buf == nullptr)
    return;

  this->recv_buffer_.insert(this->recv_buffer_.end(), buf, buf + len);
  // TODO: On ESP32, use queue to notify main thread of new data
}
void APIConnection::parse_recv_buffer_() {
  if (this->recv_buffer_.empty() || this->remove_)
    return;

  while (!this->recv_buffer_.empty()) {
    if (this->recv_buffer_[0] != 0x00) {
      ESP_LOGW(TAG, "Invalid preamble from %s", this->client_info_.c_str());
      this->fatal_error_();
      return;
    }
    uint32_t i = 1;
    const uint32_t size = this->recv_buffer_.size();
    uint32_t msg_size = 0;
    while (i < size) {
      const uint8_t dat = this->recv_buffer_[i];
      msg_size |= (dat & 0x7F);
      // consume
      i += 1;
      if ((dat & 0x80) == 0x00) {
        break;
      } else {
        msg_size <<= 7;
      }
    }
    if (i == size)
      // not enough data there yet
      return;

    uint32_t msg_type = 0;
    bool msg_type_done = false;
    while (i < size) {
      const uint8_t dat = this->recv_buffer_[i];
      msg_type |= (dat & 0x7F);
      // consume
      i += 1;
      if ((dat & 0x80) == 0x00) {
        msg_type_done = true;
        break;
      } else {
        msg_type <<= 7;
      }
    }
    if (!msg_type_done)
      // not enough data there yet
      return;

    if (size - i < msg_size)
      // message body not fully received
      return;

    // ESP_LOGVV(TAG, "RECV Message: Size=%u Type=%u", msg_size, msg_type);

    if (!this->valid_rx_message_type_(msg_type)) {
      ESP_LOGE(TAG, "Not a valid message type: %u", msg_type);
      this->fatal_error_();
      return;
    }

    uint8_t *msg = &this->recv_buffer_[i];
    this->read_message_(msg_size, msg_type, msg);
    if (this->remove_)
      return;
    // pop front
    uint32_t total = i + msg_size;
    this->recv_buffer_.erase(this->recv_buffer_.begin(), this->recv_buffer_.begin() + total);
  }
}
void APIConnection::read_message_(uint32_t size, uint32_t type, uint8_t *msg) {
  this->last_traffic_ = millis();

  switch (static_cast<APIMessageType>(type)) {
    case APIMessageType::HELLO_REQUEST: {
      HelloRequest req;
      req.decode(msg, size);
      this->on_hello_request_(req);
      break;
    }
    case APIMessageType::HELLO_RESPONSE: {
      // Invalid
      break;
    }
    case APIMessageType::CONNECT_REQUEST: {
      ConnectRequest req;
      req.decode(msg, size);
      this->on_connect_request_(req);
      break;
    }
    case APIMessageType::CONNECT_RESPONSE:
      // Invalid
      break;
    case APIMessageType::DISCONNECT_REQUEST: {
      DisconnectRequest req;
      req.decode(msg, size);
      this->on_disconnect_request_(req);
      break;
    }
    case APIMessageType::DISCONNECT_RESPONSE: {
      DisconnectResponse req;
      req.decode(msg, size);
      this->on_disconnect_response_(req);
      break;
    }
    case APIMessageType::PING_REQUEST: {
      PingRequest req;
      req.decode(msg, size);
      this->on_ping_request_(req);
      break;
    }
    case APIMessageType::PING_RESPONSE: {
      PingResponse req;
      req.decode(msg, size);
      this->on_ping_response_(req);
      break;
    }
    case APIMessageType::DEVICE_INFO_REQUEST: {
      DeviceInfoRequest req;
      req.decode(msg, size);
      this->on_device_info_request_(req);
      break;
    }
    case APIMessageType::DEVICE_INFO_RESPONSE: {
      // Invalid
      break;
    }
    case APIMessageType::LIST_ENTITIES_REQUEST: {
      ListEntitiesRequest req;
      req.decode(msg, size);
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
      req.decode(msg, size);
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
      req.decode(msg, size);
      this->on_subscribe_logs_request_(req);
      break;
    }
    case APIMessageType ::SUBSCRIBE_LOGS_RESPONSE:
      // Invalid
      break;
    case APIMessageType::COVER_COMMAND_REQUEST: {
#ifdef USE_COVER
      CoverCommandRequest req;
      req.decode(msg, size);
      this->on_cover_command_request_(req);
#endif
      break;
    }
    case APIMessageType::FAN_COMMAND_REQUEST: {
#ifdef USE_FAN
      FanCommandRequest req;
      req.decode(msg, size);
      this->on_fan_command_request_(req);
#endif
      break;
    }
    case APIMessageType::LIGHT_COMMAND_REQUEST: {
#ifdef USE_LIGHT
      LightCommandRequest req;
      req.decode(msg, size);
      this->on_light_command_request_(req);
#endif
      break;
    }
    case APIMessageType::SWITCH_COMMAND_REQUEST: {
#ifdef USE_SWITCH
      SwitchCommandRequest req;
      req.decode(msg, size);
      this->on_switch_command_request_(req);
#endif
      break;
    }
    case APIMessageType::SUBSCRIBE_SERVICE_CALLS_REQUEST: {
      SubscribeServiceCallsRequest req;
      req.decode(msg, size);
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
      req.decode(msg, size);
#endif
      break;
    }
    case APIMessageType::SUBSCRIBE_HOME_ASSISTANT_STATES_REQUEST: {
      SubscribeHomeAssistantStatesRequest req;
      req.decode(msg, size);
      this->on_subscribe_home_assistant_states_request(req);
      break;
    }
    case APIMessageType::SUBSCRIBE_HOME_ASSISTANT_STATE_RESPONSE:
      // Invalid
      break;
    case APIMessageType::HOME_ASSISTANT_STATE_RESPONSE: {
      HomeAssistantStateResponse req;
      req.decode(msg, size);
      this->on_home_assistant_state_response(req);
      break;
    }
    case APIMessageType::EXECUTE_SERVICE_REQUEST: {
      ExecuteServiceRequest req;
      req.decode(msg, size);
      this->on_execute_service(req);
      break;
    }
  }
}
void APIConnection::on_hello_request_(const HelloRequest &req) {
  ESP_LOGVV(TAG, "on_hello_request_(client_info='%s')", req.get_client_info().c_str());
  this->client_info_ = req.get_client_info() + " (" + this->client_->remoteIP().toString().c_str();
  this->client_info_ += ")";
  ESP_LOGV(TAG, "Hello from client: '%s'", this->client_info_.c_str());

  auto buffer = this->get_buffer();
  // uint32 api_version_major = 1; -> 1
  buffer.encode_uint32(1, 1);
  // uint32 api_version_minor = 2; -> 0
  buffer.encode_uint32(2, 0);

  // string server_info = 3;
  buffer.encode_string(3, get_app_name() + " (esphome v" ESPHOME_VERSION ")");
  bool success = this->send_buffer(APIMessageType::HELLO_RESPONSE);
  if (!success) {
    this->fatal_error_();
    return;
  }

  this->connection_state_ = ConnectionState::WAITING_FOR_CONNECT;
}
void APIConnection::on_connect_request_(const ConnectRequest &req) {
  ESP_LOGVV(TAG, "on_connect_request_(password='%s')", req.get_password().c_str());
  bool correct = this->parent_->check_password(req.get_password());
  auto buffer = this->get_buffer();
  // bool invalid_password = 1;
  buffer.encode_bool(1, !correct);
  bool success = this->send_buffer(APIMessageType::CONNECT_RESPONSE);
  if (!success) {
    this->fatal_error_();
    return;
  }

  if (correct) {
    ESP_LOGD(TAG, "Client '%s' connected successfully!", this->client_info_.c_str());
    this->connection_state_ = ConnectionState::CONNECTED;

#ifdef USE_HOMEASSISTANT_TIME
    if (time::global_homeassistant_time != nullptr) {
      this->send_time_request();
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
  auto buffer = this->get_buffer();
  // bool uses_password = 1;
  buffer.encode_bool(1, this->parent_->uses_password());
  // string name = 2;
  buffer.encode_string(2, get_app_name());
  // string mac_address = 3;
  buffer.encode_string(3, get_mac_address_pretty());
  // string esphome_version = 4;
  buffer.encode_string(4, ESPHOME_VERSION);
  // string compilation_time = 5;
  buffer.encode_string(5, get_app_compilation_time());
#ifdef ARDUINO_BOARD
  // string model = 6;
  buffer.encode_string(6, ARDUINO_BOARD);
#endif
#ifdef USE_DEEP_SLEEP
  // bool has_deep_sleep = 7;
  buffer.encode_bool(7, global_has_deep_sleep);
#endif
  this->send_buffer(APIMessageType::DEVICE_INFO_RESPONSE);
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
  this->remove_ = true;
}
bool APIConnection::valid_rx_message_type_(uint32_t type) {
  switch (static_cast<APIMessageType>(type)) {
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
  this->send_buffer_.clear();
  APIBuffer buf(&this->send_buffer_);
  msg.encode(buf);
  return this->send_buffer(msg.message_type());
}
bool APIConnection::send_empty_message(APIMessageType type) {
  this->send_buffer_.clear();
  return this->send_buffer(type);
}

void APIConnection::disconnect_client_() {
  this->client_->close();
  this->remove_ = true;
}
void encode_varint(uint8_t *dat, uint8_t *len, uint32_t value) {
  if (value <= 0x7F) {
    *dat = value;
    (*len)++;
    return;
  }

  while (value) {
    uint8_t temp = value & 0x7F;
    value >>= 7;
    if (value) {
      *dat = temp | 0x80;
    } else {
      *dat = temp;
    }
    dat++;
    (*len)++;
  }
}

bool APIConnection::send_buffer(APIMessageType type) {
  uint8_t header[20];
  header[0] = 0x00;
  uint8_t header_len = 1;
  encode_varint(header + header_len, &header_len, this->send_buffer_.size());
  encode_varint(header + header_len, &header_len, static_cast<uint32_t>(type));

  size_t needed_space = this->send_buffer_.size() + header_len;

  if (needed_space > this->client_->space()) {
    delay(5);
    if (needed_space > this->client_->space()) {
      if (type != APIMessageType::SUBSCRIBE_LOGS_RESPONSE) {
        ESP_LOGV(TAG, "Cannot send message because of TCP buffer space");
      }
      delay(5);
      return false;
    }
  }

//  char buffer[512];
//  uint32_t offset = 0;
//  for (int j = 0; j < header_len; j++) {
//    offset += snprintf(buffer + offset, 512 - offset, "0x%02X ", header[j]);
//  }
//  offset += snprintf(buffer + offset, 512 - offset, "| ");
//  for (auto &it : this->send_buffer_) {
//    int i = snprintf(buffer + offset, 512 - offset, "0x%02X ", it);
//    if (i <= 0)
//      break;
//    offset += i;
//  }
//  ESP_LOGVV(TAG, "SEND %s", buffer);

  this->client_->add(reinterpret_cast<char *>(header), header_len);
  this->client_->add(reinterpret_cast<char *>(this->send_buffer_.data()), this->send_buffer_.size());
  return this->client_->send();
}

void APIConnection::loop() {
  if (this->client_->disconnected()) {
    // failsafe for disconnect logic
    this->on_disconnect_();
    return;
  }
  this->parse_recv_buffer_();

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

  auto buffer = this->get_buffer();
  // fixed32 key = 1;
  buffer.encode_fixed32(1, binary_sensor->get_object_id_hash());
  // bool state = 2;
  buffer.encode_bool(2, state);
  return this->send_buffer(APIMessageType::BINARY_SENSOR_STATE_RESPONSE);
}
#endif

#ifdef USE_COVER
bool APIConnection::send_cover_state(cover::Cover *cover) {
  if (!this->state_subscription_)
    return false;

  auto buffer = this->get_buffer();
  // fixed32 key = 1;
  buffer.encode_fixed32(1, cover->get_object_id_hash());
  // enum CoverState {
  //   OPEN = 0;
  //   CLOSED = 1;
  // }
  // CoverState state = 2;
  uint32_t state = (cover->state == cover::COVER_OPEN) ? 0 : 1;
  buffer.encode_uint32(2, state);
  return this->send_buffer(APIMessageType::COVER_STATE_RESPONSE);
}
#endif

#ifdef USE_FAN
bool APIConnection::send_fan_state(fan::FanState *fan) {
  if (!this->state_subscription_)
    return false;

  auto buffer = this->get_buffer();
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
  return this->send_buffer(APIMessageType::FAN_STATE_RESPONSE);
}
#endif

#ifdef USE_LIGHT
bool APIConnection::send_light_state(light::LightState *light) {
  if (!this->state_subscription_)
    return false;

  auto buffer = this->get_buffer();
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
  return this->send_buffer(APIMessageType::LIGHT_STATE_RESPONSE);
}
#endif

#ifdef USE_SENSOR
bool APIConnection::send_sensor_state(sensor::Sensor *sensor, float state) {
  if (!this->state_subscription_)
    return false;

  auto buffer = this->get_buffer();
  // fixed32 key = 1;
  buffer.encode_fixed32(1, sensor->get_object_id_hash());
  // float state = 2;
  buffer.encode_float(2, state);
  return this->send_buffer(APIMessageType::SENSOR_STATE_RESPONSE);
}
#endif

#ifdef USE_SWITCH
bool APIConnection::send_switch_state(switch_::Switch *switch_, bool state) {
  if (!this->state_subscription_)
    return false;

  auto buffer = this->get_buffer();
  // fixed32 key = 1;
  buffer.encode_fixed32(1, switch_->get_object_id_hash());
  // bool state = 2;
  buffer.encode_bool(2, state);
  return this->send_buffer(APIMessageType::SWITCH_STATE_RESPONSE);
}
#endif

#ifdef USE_TEXT_SENSOR
bool APIConnection::send_text_sensor_state(text_sensor::TextSensor *text_sensor, std::string state) {
  if (!this->state_subscription_)
    return false;

  auto buffer = this->get_buffer();
  // fixed32 key = 1;
  buffer.encode_fixed32(1, text_sensor->get_object_id_hash());
  // string state = 2;
  buffer.encode_string(2, state);
  return this->send_buffer(APIMessageType::TEXT_SENSOR_STATE_RESPONSE);
}
#endif

bool APIConnection::send_log_message(int level,
                                     const char *tag,
                                     const char *line) {
  if (this->log_subscription_ < level)
    return false;

  auto buffer = this->get_buffer();
  // LogLevel level = 1;
  buffer.encode_uint32(1, static_cast<uint32_t>(level));
  // string tag = 2;
  // buffer.encode_string(2, tag, strlen(tag));
  // string message = 3;
  buffer.encode_string(3, line, strlen(line));
  bool success = this->send_buffer(APIMessageType::SUBSCRIBE_LOGS_RESPONSE);

  if (!success) {
    auto buffer = this->get_buffer();
    // bool send_failed = 4;
    buffer.encode_bool(4, true);
    return this->send_buffer(APIMessageType::SUBSCRIBE_LOGS_RESPONSE);
  } else {
    return true;
  }
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

  this->send_message(call);
}
void APIConnection::on_subscribe_home_assistant_states_request(const SubscribeHomeAssistantStatesRequest &req) {
  for (auto &it : this->parent_->get_state_subs()) {
    auto buffer = this->get_buffer();
    // string entity_id = 1;
    buffer.encode_string(1, it.entity_id);
    this->send_buffer(APIMessageType::SUBSCRIBE_HOME_ASSISTANT_STATE_RESPONSE);
  }
}
void APIConnection::on_home_assistant_state_response(const HomeAssistantStateResponse &req) {
  for (auto &it : this->parent_->get_state_subs()) {
    if (it.entity_id == req.get_entity_id()) {
      it.callback(req.get_state());
    }
  }
}
void APIConnection::on_execute_service(const ExecuteServiceRequest &req) {
  ESP_LOGVV(TAG, "on_execute_service");
  bool found = false;
  for (auto *service : this->parent_->get_user_services()) {
    if (service->execute_service(req)) {
      found = true;
    }
  }
  if (!found) {
    ESP_LOGV(TAG, "Could not find matching service!");
  }
}

APIBuffer APIConnection::get_buffer() {
  this->send_buffer_.clear();
  return APIBuffer(&this->send_buffer_);
}
#ifdef USE_HOMEASSISTANT_TIME
void APIConnection::send_time_request() {
  this->send_empty_message(APIMessageType::GET_TIME_REQUEST);
}
#endif

} // namespace api

ESPHOME_NAMESPACE_END

#endif //USE_API
