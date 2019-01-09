#ifndef ESPHOMELIB_API_SERVICE_CALL_MESSAGE_H
#define ESPHOMELIB_API_SERVICE_CALL_MESSAGE_H

#include "esphomelib/defines.h"

#ifdef USE_API

#include "esphomelib/helpers.h"
#include "esphomelib/automation.h"
#include "esphomelib/api/api_message.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace api {

class SubscribeServiceCallsRequest : public APIMessage {
 public:
  APIMessageType message_type() const override;
};

class KeyValuePair {
 public:
  KeyValuePair(const std::string &key, const std::string &value);

  std::string key;
  std::string value;
};

class TemplatableKeyValuePair {
 public:
  template<typename T>
  TemplatableKeyValuePair(std::string key, T func);

  std::string key;
  std::function<std::string()> value;
};
template<typename T>
TemplatableKeyValuePair::TemplatableKeyValuePair(std::string key, T func) : key(key) {
  this->value = [func]() -> std::string {
    return to_string(func());
  };
}

class ServiceCallResponse : public APIMessage {
 public:
  APIMessageType message_type() const override;

  void encode(APIBuffer &buffer) override;

  void set_service(const std::string &service);
  void set_data(const std::vector<KeyValuePair> &data);
  void set_data_template(const std::vector<KeyValuePair> &data_template);
  void set_variables(const std::vector<TemplatableKeyValuePair> &variables);

 protected:
  std::string service_;
  std::vector<KeyValuePair> data_;
  std::vector<KeyValuePair> data_template_;
  std::vector<TemplatableKeyValuePair> variables_;
};

} // namespace api

ESPHOMELIB_NAMESPACE_END

#endif //USE_API

#endif //ESPHOMELIB_API_SERVICE_CALL_MESSAGE_H
