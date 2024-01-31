#include "main/hal/registration_portal/registration_portal.h"

#include <sys/param.h>

#include "main/config.h"
#include "main/hal/timer/timer.h"
#include "main/libs/cJson/cJSON.h"
#include "main/logger/logger.h"

RegistrationPortal::RegistrationPortal(Wifi *wifi, HTTPServer *http_server,
                                       AuthenticationService *auth_service,
                                       ImageUI *image_ui)
    : m_wifi(wifi),
      m_http_server(http_server),
      m_auth_service(auth_service),
      m_image_ui(image_ui),
      new_credentials(false) {}

RegistrationPortal::~RegistrationPortal() {}

extern const char registration_portal_start[] asm(
    "_binary_registration_html_start");
extern const char registration_portal_end[] asm(
    "_binary_registration_html_end");

bool RegistrationPortal::start() {
  if (m_http_server->running()) {
    Logger::error("HTTP server already started. Please stop it first.");
    return false;
  }

  do {
    // start the wifi access point
    m_wifi->startAccesspoint();
    m_http_server->start();
    registerHandlers();

    m_image_ui->showSetupScreen();
  } while (!waitForCredentials());

  return true;
}

void RegistrationPortal::registerHandlers() {
  const httpd_uri_t handleHomepage_uri{
      .uri = "/",
      .method = HTTP_GET,
      .handler =
          [](httpd_req_t *request) {
            return static_cast<RegistrationPortal *>(request->user_ctx)
                ->handleHomepage(request);
          },
      .user_ctx = this,
  };
  m_http_server->registerURIHandler(&handleHomepage_uri);

  const httpd_uri_t handleRegistration_uri{
      .uri = "/register",
      .method = HTTP_POST,
      .handler =
          [](httpd_req_t *request) {
            return static_cast<RegistrationPortal *>(request->user_ctx)
                ->handleRegistration(request);
          },
      .user_ctx = this,
  };
  m_http_server->registerURIHandler(&handleRegistration_uri);

  m_http_server->registerErrHandler(HTTPD_404_NOT_FOUND, handleError);
}

esp_err_t RegistrationPortal::handleHomepage(httpd_req_t *req) {
  // return the registration.html page
  const uint32_t registration_portal_len =
      registration_portal_end - registration_portal_start;
  httpd_resp_set_type(req, "text/html");
  httpd_resp_send(req, registration_portal_start, registration_portal_len);
  return ESP_OK;
}

esp_err_t RegistrationPortal::handleRegistration(httpd_req_t *req) {
  char content[req->content_len];

  /* Truncate if content length is larger than the buffer */
  size_t recv_size = MIN(req->content_len, sizeof(content));

  int ret = httpd_req_recv(req, content, recv_size);
  if (ret <= 0) {
    if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
      httpd_resp_send_408(req);
    }
    return ESP_FAIL;
  }

  cJSON *json = cJSON_Parse(content);
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Error before: %s\n", error_ptr);
    }
    return 1;
  }

  // Get the SSID value
  cJSON *wifi_ssid = cJSON_GetObjectItem(json, "wifi_ssid");
  if (!cJSON_IsString(wifi_ssid) || (wifi_ssid->valuestring == NULL)) {
    Logger::error("Error: Wifi SSID is not a string");
    cJSON_Delete(json);
    return 1;
  }

  // Get the Password value
  cJSON *wifi_password = cJSON_GetObjectItem(json, "wifi_password");
  if (!cJSON_IsString(wifi_password) || (wifi_password->valuestring == NULL)) {
    Logger::error("Error: Wifi Password is not a string");
    cJSON_Delete(json);
    return 1;
  }

  // Get the Device Token
  cJSON *device_token = cJSON_GetObjectItem(json, "device_token");
  if (!cJSON_IsString(device_token) || (device_token->valuestring == NULL)) {
    Logger::error("Error: Device Token is not a string");
    cJSON_Delete(json);
    return 1;
  }

  credentials.password = std::string(wifi_password->valuestring);
  credentials.ssid = std::string(wifi_ssid->valuestring);
  credentials.token = std::string(device_token->valuestring);

  Logger::debug("Registration values:");
  Logger::debug("SSID: " + credentials.ssid);
  Logger::debug("Password: " + credentials.password);
  Logger::debug("Device Token: " + credentials.token);

  Logger::debug("Cleaning up...");
  cJSON_Delete(json);

  const char resp[] = "Credentials received.";
  Logger::debug("Sending response...");
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

  m_image_ui->showConnectingScreen();

  new_credentials = true;
  return ESP_OK;
}

esp_err_t RegistrationPortal::handleError(httpd_req_t *req,
                                          httpd_err_code_t err) {
  // forward everything to the root handler, enable captive portal
  httpd_resp_set_status(req, "302 Temporary Redirect");
  httpd_resp_set_hdr(req, "Location", "/");
  httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

  return ESP_OK;
}

bool RegistrationPortal::waitForCredentials() {
  while (new_credentials == false) {
    Timer::sleepMS(1000);
    Logger::debug("Waiting for new credentials...");
  }
  new_credentials = false;

  Logger::debug("Credentials received. Stopping http server and wifi ap...");
  m_http_server->stop();
  m_wifi->stop();

  Logger::debug("Starting station and testing wifi credentials...");
  if (!m_wifi->startStation(credentials.ssid, credentials.password)) {
    Logger::error(
        "Failed to start station. Please check your wifi credentials.");

    m_image_ui->showWrongWifiCredentialsScreen();

    Timer::sleepMS(5000);
    return false;
  }

  bool authenticated = false;
  for (int i = 0; i < 3; i++) {
    if (m_auth_service->authenticate(credentials.token)) {
      authenticated = true;
      break;
    }
    Timer::sleepMS(1000);
    Logger::debug("Waiting for wifi connection...");
  }

  if (!authenticated) {
    Logger::error("Failed to authenticate. Please check your token.");

    m_image_ui->showWrongDeviceTokenScreen();

    Timer::sleepMS(5000);

    m_wifi->stop();
    return false;
  }

  m_wifi->storeWifiCredentials(credentials.ssid, credentials.password);

  m_image_ui->showConnectedScreen();

  Timer::sleepMS(5000);

  return true;
}