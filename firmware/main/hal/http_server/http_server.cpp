#include "main/hal/http_server/http_server.h"

#include "main/logger/logger.h"

HTTPServer::HTTPServer() : m_server(nullptr) {}

HTTPServer::~HTTPServer() {
  if (m_server == nullptr) {
    return;
  }
  stop();
}

bool HTTPServer::start() {
  if (m_server != nullptr) {
    Logger::error("HTTP server already started. Please stop it first.");
    return false;
  }
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  const esp_err_t err = httpd_start(&m_server, &config);
  if (err != ESP_OK) {
    Logger::error("Failed to start HTTP server: " +
                  std::string(esp_err_to_name(err)));
    return false;
  }
  Logger::debug("HTTP server started.");
  return true;
}

bool HTTPServer::running() { return m_server != nullptr; }

bool HTTPServer::stop() {
  if (m_server == nullptr) {
    return true;
  }
  const esp_err_t err = httpd_stop(m_server);
  if (err != ESP_OK) {
    Logger::error("Failed to stop HTTP server: " +
                  std::string(esp_err_to_name(err)));
    return false;
  }
  m_server = nullptr;
  Logger::debug("HTTP server stopped.");
  return true;
}

bool HTTPServer::registerURIHandler(const httpd_uri_t* uri_handler) {
  if (m_server == nullptr) {
    Logger::error("HTTP server not started. Please start it first.");
    return false;
  }
  const esp_err_t err = httpd_register_uri_handler(m_server, uri_handler);
  if (err != ESP_OK) {
    Logger::error("Failed to register URI handler: " +
                  std::string(esp_err_to_name(err)));
    return false;
  }
  Logger::debug("URI handler registered.");
  return true;
}

bool HTTPServer::registerErrHandler(const httpd_err_code_t error,
                                    const httpd_err_handler_func_t handler_fn) {
  if (m_server == nullptr) {
    Logger::error("HTTP server not started. Please start it first.");
    return false;
  }
  const esp_err_t err = httpd_register_err_handler(m_server, error, handler_fn);
  if (err != ESP_OK) {
    Logger::error("Failed to register error handler: " +
                  std::string(esp_err_to_name(err)));
    return false;
  }
  Logger::debug("Error handler registered.");
  return true;
}

bool HTTPServer::unregisterURIHandler(const char* uri, httpd_method_t method) {
  if (m_server == nullptr) {
    Logger::error("HTTP server not started. Please start it first.");
    return false;
  }
  const esp_err_t err = httpd_unregister_uri_handler(m_server, uri, method);
  if (err != ESP_OK) {
    Logger::error("Failed to unregister URI handler: " +
                  std::string(esp_err_to_name(err)));
    return false;
  }
  Logger::debug("URI handler unregistered.");
  return true;
}