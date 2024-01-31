#pragma once

#include <esp_err.h>

#include "esp_http_server.h"

//! @brief HTTP server class
class HTTPServer {
 public:
  //! @brief Constructor
  HTTPServer();

  //! @brief Destructor
  ~HTTPServer();

  //! @brief Start the HTTP server.
  //! @return True if the server was started successfully, false otherwise.
  bool start();

  //! @brief Check if the HTTP server is running.
  //! @return True if the server is running, false otherwise.
  bool running();

  //! @brief Stop the HTTP server.
  //! @return True if the server was stopped successfully, false otherwise.
  bool stop();

  //! @brief Register a URI handler.
  //! @param uri_handler The URI handler to register.
  //! @return True if the handler was registered successfully, false otherwise.
  bool registerURIHandler(const httpd_uri_t* uri_handler);

  //! @brief Register an error handler.
  //! @param error The error code to register the handler for.
  //! @param handler_fn The handler function to register.
  //! @return True if the handler was registered successfully, false otherwise.
  bool registerErrHandler(const httpd_err_code_t error,
                          const httpd_err_handler_func_t handler_fn);

  //! @brief Unregister a URI handler.
  //! @param uri The URI to unregister the handler for.
  //! @param method The HTTP method to unregister the handler for.
  //! @return True if the handler was unregistered successfully, false
  //! otherwise.
  bool unregisterURIHandler(const char* uri, httpd_method_t method);

 private:
  //! @brief The HTTP server handle.
  httpd_handle_t m_server;
};