#pragma once

#include "main/hal/http_server/http_server.h"
#include "main/hal/non_volatile_storage/non_volatile_storage.h"
#include "main/hal/wifi/wifi.h"
#include "main/service/authentication_service/authentication_service.h"
#include "main/ui/image_ui/image_ui.h"

class RegistrationPortal {
 public:
  //! @brief Constructor
  //! @param wifi The wifi driver.
  //! @param http_server The HTTP server.
  //! @param auth_service The authentication service.
  //! @param image_ui The ImageUI service.
  RegistrationPortal(Wifi *wifi, HTTPServer *http_server,
                     AuthenticationService *auth_service, ImageUI *image_ui);

  //! @brief Destructor
  ~RegistrationPortal();

  //! @brief Start the registration portal.
  //! @return True if the registration portal was started successfully, false
  //! otherwise.
  bool start();

 private:
  //! @brief Register the uri handlers.
  void registerHandlers();

  //! @brief Wait for the http request with the credentials.
  //! @return True if valid credentials were received, false otherwise.
  bool waitForCredentials();

  //! @brief The homepage handler.
  //! @param req The HTTP request.
  //! @return ESP_OK if the request was handled successfully, an error
  //! otherwise.
  esp_err_t handleHomepage(httpd_req_t *req);

  //! @brief The registration handler.
  //! @param req The HTTP request.
  //! @return ESP_OK if the request was handled successfully, an error
  //! otherwise.
  esp_err_t handleRegistration(httpd_req_t *req);

  //! @brief The error handler.
  //! @param req The HTTP request.
  //! @param err The error code.
  //! @return ESP_OK if the request was handled successfully, an error
  //! otherwise.
  static esp_err_t handleError(httpd_req_t *req, httpd_err_code_t err);

  //! @brief The wifi driver.
  Wifi *m_wifi;

  //! @brief The HTTP server.
  HTTPServer *m_http_server;

  //! @brief The authentication service.
  AuthenticationService *m_auth_service;

  //! @brief The ImageUI service.
  ImageUI *m_image_ui;

  //! @brief Flag to indicate if new credentials were received (true if new
  //! credentials were received, false otherwise).
  bool new_credentials;

  //! @brief The passed credentials.
  struct Credentials {
    //! @brief The wifi SSID.
    std::string ssid = "";
    //! @brief The wifi password.
    std::string password = "";
    //! @brief The temporary authentication token.
    std::string token = "";
  } credentials;
};
