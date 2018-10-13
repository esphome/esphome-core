#include <utility>

//
//  http_client_component.h
//  esphomelib
//
//  Created by Otto Winter on 2018/10/12.
//  Copyright © 2018 Otto Winter. All rights reserved.
//

#ifndef ESPHOMELIB_HTTP_CLIENT_COMPONENT_H
#define ESPHOMELIB_HTTP_CLIENT_COMPONENT_H

#include "esphomelib/defines.h"

#ifdef USE_HTTP_CLIENT

#include "esphomelib/component.h"
#include "esphomelib/automation.h"

#include <WiFiClient.h>

ESPHOMELIB_NAMESPACE_BEGIN

class HTTPClientHeader {
 public:
  HTTPClientHeader(std::string key, std::string value) : key_(std::move(key)), value_(std::move(value)) {}

  const std::string &get_key() const;
  const std::string &get_value() const;

 protected:
  std::string key_;
  std::string value_;
};

enum HTTPClientMethod {
  HTTP_CLIENT_METHOD_GET = 0,
  HTTP_CLIENT_METHOD_POST,
  HTTP_CLIENT_METHOD_PUT,
  HTTP_CLIENT_METHOD_PATCH,
  HTTP_CLIENT_METHOD_DELETE,
  HTTP_CLIENT_METHOD_HEAD,
};

template<typename T>
class HTTPRequestAction;

class HTTPClientComponent : public Component {
 public:
  HTTPClientComponent();

  void setup() override;

  void parse_url(const std::string &url_);

  void send();

  void disconnect(bool force = false);

  void clear();

  void header(const std::string &name, std::string value);

  void set_method(HTTPClientMethod method);

  void set_payload(const std::string &payload);

  template<typename T>
  HTTPRequestAction<T> *make_http_request_action(HTTPClientMethod method);

  float get_setup_priority() const override;

 protected:
  void send_data_();

  const char *method_as_string();

  WiFiClient client_{};
  std::string host_;
  uint16_t port_;
  HTTPClientMethod method_;
  std::string uri_;
  std::string auth_;
  std::string payload_;
  std::vector<HTTPClientHeader> headers_;
  bool has_user_agent_;
  bool has_accept_encoding_;
};

template<typename T>
class HTTPRequestAction : public Action<T> {
 public:
  HTTPRequestAction(HTTPClientComponent *parent, HTTPClientMethod method);

  void play(T x) override;

  void set_url(std::string url);
  void set_url(std::function<std::string(T)> url);
  void set_payload(std::string payload);
  void set_payload(std::function<std::string(T)> payload);
  void set_headers(const std::vector<HTTPClientHeader> &headers);

 protected:
  HTTPClientComponent *parent_;
  HTTPClientMethod method_;
  TemplatableValue<std::string, T> url_{};
  TemplatableValue<std::string, T> payload_{};
  std::vector<HTTPClientHeader> headers_{};
};

template<typename T>
void HTTPRequestAction<T>::play(T x) {
  this->parent_->clear();
  this->parent_->set_method(this->method_);
  this->parent_->parse_url(this->url_.value(x));
  this->parent_->set_payload(this->payload_.value(x));
  for (const auto &header : this->headers_) {
    this->parent_->header(header.get_key(), header.get_value());
  }
  this->parent_->send();
  this->play_next(x);
}

template<typename T>
HTTPRequestAction<T>::HTTPRequestAction(HTTPClientComponent *parent, HTTPClientMethod method)
    : parent_(parent), method_(method) {

}
template<typename T>
void HTTPRequestAction<T>::set_headers(const std::vector<HTTPClientHeader> &headers) {
  this->headers_ = headers;
}
template<typename T>
void HTTPRequestAction<T>::set_url(std::string url) {
  this->url_ = std::move(url);
}
template<typename T>
void HTTPRequestAction<T>::set_url(std::function<std::string(T)> url) {
  this->url_ = std::move(url);
}
template<typename T>
void HTTPRequestAction<T>::set_payload(std::string payload) {
  this->payload_ = std::move(payload);
}
template<typename T>
void HTTPRequestAction<T>::set_payload(std::function<std::string(T)> payload) {
  this->payload_ = std::move(payload);
}

template<typename T>
HTTPRequestAction<T> *HTTPClientComponent::make_http_request_action(HTTPClientMethod method) {
  return new HTTPRequestAction<T>(this, method);
}

ESPHOMELIB_NAMESPACE_END

#endif //USE_HTTP_CLIENT

#endif //ESPHOMELIB_HTTP_CLIENT_COMPONENT_H
