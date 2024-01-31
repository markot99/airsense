#include "main/hal/http_client/http_client.h"

#include <esp_crt_bundle.h>
#include <esp_log.h>
#include <esp_tls.h>

#include "main/config.h"
#include "main/hal/timer/timer.h"
#include "main/logger/logger.h"

HTTPClient::HTTPClient() : request_ongoing(false), response_content("") {}

HTTPClient::~HTTPClient() {}

esp_err_t HTTPClient::httpEventHandler(esp_http_client_event_t *event) {
  switch (event->event_id) {
    case HTTP_EVENT_ERROR:
      break;

    case HTTP_EVENT_ON_CONNECTED:
      break;

    case HTTP_EVENT_HEADER_SENT:
      break;

    case HTTP_EVENT_ON_HEADER:
      break;

    case HTTP_EVENT_ON_DATA: {
      response_content.append((char *)event->data, event->data_len);
      request_ongoing = false;
      break;
    }

    case HTTP_EVENT_ON_FINISH:
      break;

    case HTTP_EVENT_DISCONNECTED: {
      int mbedtls_err = 0;
      esp_err_t err = esp_tls_get_and_clear_last_error(
          (esp_tls_error_handle_t)event->data, &mbedtls_err, NULL);
      if (err != 0) {
        Logger::error("Last esp error code: " + std::to_string(err));
        Logger::error("Last mbedtls failure: " + std::to_string(mbedtls_err));
      }
      break;
    }

    case HTTP_EVENT_REDIRECT:
      break;
  }

  return ESP_OK;
}

HTTPResponse HTTPClient::getJSON(const std::string &url,
                                 const std::string &token) {
  esp_http_client_config_t config = {
      .url = API_BASE_URL,
      .event_handler = [](esp_http_client_event_t *event) -> esp_err_t {
        return static_cast<HTTPClient *>(event->user_data)
            ->httpEventHandler(event);
      },
      .buffer_size = 1024,
      .user_data = this,
      .crt_bundle_attach = esp_crt_bundle_attach};

  m_client = esp_http_client_init(&config);

  Logger::debug("GET " + url);
  esp_http_client_set_url(m_client, url.c_str());
  esp_http_client_set_method(m_client, HTTP_METHOD_GET);
  esp_http_client_set_header(m_client, "Content-Type", "application/json");
  if (!token.empty()) {
    esp_http_client_set_header(m_client, "Authorization",
                               ("Bearer " + token).c_str());
  }

  request_ongoing = true;
  esp_err_t err = esp_http_client_perform(m_client);

  if (err == ESP_OK) {
    auto httpStatusCode = esp_http_client_get_status_code(m_client);

    while (request_ongoing) {
      Timer::sleepMS(100);
    }

    esp_http_client_cleanup(m_client);

    const std::string response_content_tmp = response_content;
    response_content.clear();

    return {httpStatusCode, response_content_tmp};
  }
  esp_http_client_cleanup(m_client);
  response_content.clear();
  Logger::error("HTTP request failed: " + std::string(esp_err_to_name(err)));
  return {0, ""};
}

HTTPResponse HTTPClient::postJSON(const std::string &url,
                                  const std::string &data,
                                  const std::string &token) {
  esp_http_client_config_t config = {
      .url = API_BASE_URL,
      .event_handler = [](esp_http_client_event_t *event) -> esp_err_t {
        return static_cast<HTTPClient *>(event->user_data)
            ->httpEventHandler(event);
      },
      .buffer_size = 1024,
      .user_data = this,
      .crt_bundle_attach = esp_crt_bundle_attach};

  m_client = esp_http_client_init(&config);

  Logger::debug("POST " + url + " " + data);
  esp_http_client_set_url(m_client, url.c_str());
  esp_http_client_set_method(m_client, HTTP_METHOD_POST);
  esp_http_client_set_header(m_client, "Content-Type", "application/json");
  esp_http_client_set_post_field(m_client, data.c_str(), data.length());

  if (!token.empty()) {
    esp_http_client_set_header(m_client, "Authorization",
                               ("Bearer " + token).c_str());
  }

  request_ongoing = true;
  esp_err_t err = esp_http_client_perform(m_client);

  if (err == ESP_OK) {
    auto httpStatusCode = esp_http_client_get_status_code(m_client);

    while (request_ongoing) {
      Timer::sleepMS(100);
    }
    esp_http_client_cleanup(m_client);

    const std::string response_content_tmp = response_content;
    response_content.clear();

    return {httpStatusCode, response_content_tmp};
  }
  esp_http_client_cleanup(m_client);
  response_content.clear();
  Logger::error("HTTP request failed: " + std::string(esp_err_to_name(err)));
  return {0, ""};
}