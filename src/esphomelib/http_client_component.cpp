//
//  http_client_component.cpp
//  esphomelib
//
//  Created by Otto Winter on 2018/10/12.
//  Copyright © 2018 Otto Winter. All rights reserved.
//


#include "esphomelib/defines.h"

#ifdef USE_HTTP_CLIENT

#include "esphomelib/http_client_component.h"
#include "esphomelib/mqtt/mqtt_client_component.h"
#include "esphomelib/log.h"
#include <base64.h>
#include <cstdio>

ESPHOMELIB_NAMESPACE_BEGIN

static const char *TAG = "http_client";

void HTTPClientComponent::setup() {

}
void HTTPClientComponent::parse_url(const std::string &url_) {
  std::string url;
  if (url_.compare(0, 7, "http://") == 0) {
    url = url_.substr(7);
  } else {
    url = url_;
  }

  size_t path_begin = url.find('/');
  std::string host = url.substr(0, path_begin);
  if (path_begin == std::string::npos) {
    this->uri_ = "/";
  } else {
    this->uri_ = url.substr(path_begin);
  }

  size_t auth_begin = host.find('@');
  if (auth_begin != std::string::npos) {
    this->auth_ = base64::encode(host.substr(0, auth_begin).c_str()).c_str();
    host = host.substr(auth_begin + 1);
  } else {
    this->auth_ = "";
  }

  size_t port_begin = host.find(':');
  uint16_t port = 80;
  if (port_begin != std::string::npos) {
    std::string port_s = host.substr(0, port_begin);
    char *end;
    port = ::strtoul(port_s.c_str(), &end, 10);
    if (end == nullptr) {
      ESP_LOGW(TAG, "Cannot convert '%s' to port number!", port_s.c_str());
      port = 80;
    }
    host = host.substr(port_begin + 1);
  }

  if (this->host_ != host || this->port_ != port) {
    this->disconnect();
  }
  this->host_ = host;
  this->port_ = port;
  ESP_LOGV(TAG, "Host: '%s', Port: %u, URI: '%s', Auth: '%s'",
      this->host_.c_str(), this->port_, this->uri_.c_str(), this->auth_.c_str());
}
void HTTPClientComponent::send() {
  if (!this->client_.connected()) {
    if (!this->client_.connect(this->host_.c_str(), this->port_) || !this->client_.connected()) {
      ESP_LOGW(TAG, "Error connecting to %s:%u!", this->host_.c_str(), this->port_);
      return;
    }
  } else {
    while (this->client_.available()) {
      this->client_.read();
    }
  }
  this->send_data_();
}
void HTTPClientComponent::disconnect(bool force) {
  if (!this->client_.connected())
    return;

  this->client_.stop();
}
void HTTPClientComponent::clear() {
  this->method_ = HTTP_CLIENT_METHOD_GET;
  this->uri_ = "/";
  this->auth_.clear();
  this->payload_.clear();
  this->headers_.clear();
  this->has_accept_encoding_ = false;
  this->has_user_agent_ = false;
}
void HTTPClientComponent::header(const std::string &name, std::string value) {
  this->headers_.emplace_back(name, std::move(value));
  this->has_accept_encoding_ = this->has_accept_encoding_ || name == "Accept-Encoding";
  this->has_user_agent_ = this->has_user_agent_ || name == "User-Agent";
}
void HTTPClientComponent::send_data_() {
  char buffer[100];
  if (!this->payload_.empty()) {
    snprintf(buffer, sizeof(buffer), "%u", this->payload_.size());
    this->header("Content-Length", buffer);
  }

  const char *method_s = this->method_as_string();
  ESP_LOGD(TAG, "%s http://%s:%u%s", method_s, this->host_.c_str(), this->port_, this->uri_.c_str());

  this->client_.print(method_s);
  this->client_.print(" ");
  this->client_.print(this->uri_.c_str());
  this->client_.print(" HTTP/1.1\r\n");
  ESP_LOGV(TAG, "%s %s HTTP/1.1\\r\\n", method_s, this->uri_.c_str());

  this->client_.print("Host: ");
  this->client_.print(this->host_.c_str());
  if (this->port_ != 80) {
    snprintf(buffer, sizeof(buffer), "%u", this->port_);
    this->client_.print(":");
    this->client_.print(buffer);
    ESP_LOGV(TAG, "Host: %s:%s\\r\\n", this->host_.c_str(), buffer);
  } else {
    ESP_LOGV(TAG, "Host: %s\\r\\n", this->host_.c_str());
  }
  this->client_.print("\r\n");

  this->client_.print("Connection: close\r\n");
  ESP_LOGV(TAG, "Connection: close\\r\\n");

  if (!this->auth_.empty()) {
    this->client_.print("Authorization: Basic ");
    this->client_.print(this->auth_.c_str());
    this->client_.print("\r\n");
    ESP_LOGV(TAG, "Authorization: Basic %s\\r\\n", this->auth_.c_str());
  }

  if (!this->has_user_agent_) {
    this->header("User-Agent", "esphomelib HTTP client");
  }
  if (!this->has_accept_encoding_) {
    this->header("Accept-Encoding", "identity;q=1,chunked;q=0.1,*;q=0");
  }

  for (const auto &header : this->headers_) {
    this->client_.print(header.get_key().c_str());
    this->client_.print(": ");
    this->client_.print(header.get_value().c_str());
    this->client_.print("\r\n");
    ESP_LOGV(TAG, "%s: %s\\r\\n", header.get_key().c_str(), header.get_value().c_str());
  }

  this->client_.print("\r\n");
  ESP_LOGV(TAG, "\\r\\n");

  if (!this->payload_.empty()) {
    this->client_.print(this->payload_.c_str());
    ESP_LOGV(TAG, "%s", this->payload_.c_str());
  }
#ifdef ESPHOMEliB_LOG_HAS_VERBOSE
  ESP_LOGV(TAG, "%s", this->client_.readString().c_str());
#endif
  this->client_.flush();
  this->client_.stop();
}
const char *HTTPClientComponent::method_as_string() {
  switch (this->method_) {
    case HTTP_CLIENT_METHOD_GET: return "GET";
    case HTTP_CLIENT_METHOD_POST: return "POST";
    case HTTP_CLIENT_METHOD_PUT: return "PUT";
    case HTTP_CLIENT_METHOD_PATCH: return "PATCH";
    case HTTP_CLIENT_METHOD_DELETE: return "DELETE";
    case HTTP_CLIENT_METHOD_HEAD: return "HEAD";
  }
}
void HTTPClientComponent::set_method(HTTPClientMethod method) {
  this->method_ = method;
}
void HTTPClientComponent::set_payload(const std::string &payload) {
  this->payload_ = payload;
}
float HTTPClientComponent::get_setup_priority() const {
  return setup_priority::HARDWARE_LATE;
}
HTTPClientComponent::HTTPClientComponent() = default;

const std::string &HTTPClientHeader::get_key() const {
  return this->key_;
}
const std::string &HTTPClientHeader::get_value() const {
  return this->value_;
}

ESPHOMELIB_NAMESPACE_END

#endif //USE_HTTP_CLIENT
