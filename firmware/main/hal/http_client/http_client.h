#pragma once

#include <esp_err.h>
#include <esp_http_client.h>

#include <string>

//! @brief HTTP response struct
struct HTTPResponse {
  //! http status code
  int httpStatusCode;

  //! http response content
  std::string response_content;
};

//! @brief HTTP client class
class HTTPClient {
 public:
  //! @brief Constructor
  HTTPClient();

  //! @brief Destructor
  ~HTTPClient();

  //! @brief Get the data from the given URL.
  //! @param url The URL to get the data from.
  //! @param token The token to use for authentication, empty string if no token
  //! is needed.
  HTTPResponse getJSON(const std::string& url, const std::string& token = "");

  //! @brief Post the given data to the given URL.
  //! @param url The URL to post the data to.
  //! @param data The json data to post.
  //! @param token The token to use for authentication, empty string if no token
  //! is needed.
  HTTPResponse postJSON(const std::string& url, const std::string& data,
                        const std::string& token = "");

 private:
  //! @brief The http event handler.
  //! @param event The http event.
  //! @return ESP_OK if the event was handled successfully, an error otherwise.
  esp_err_t httpEventHandler(esp_http_client_event_t* event);

  //! @brief Flag to indicate if a request is ongoing. (True if a request is
  //! ongoin, false otherwise).
  bool request_ongoing;

  //! @brief The response content.
  std::string response_content;

  //! @brief The http client handle.
  esp_http_client_handle_t m_client;
};
