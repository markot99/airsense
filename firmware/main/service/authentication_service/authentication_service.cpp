#include "main/service/authentication_service/authentication_service.h"

#include "main/config.h"
#include "main/logger/logger.h"

AuthenticationService::AuthenticationService(
    HTTPClient *http_client, NonVolatileStorage *non_volatile_storage)
    : m_http_client(http_client), m_non_volatile_storage(non_volatile_storage) {
  Logger::info("Initializing authentication service...");
  loadAuthenticationToken();
  Logger::info("Finished initializing authentication service");
}

AuthenticationService::~AuthenticationService() {}

bool AuthenticationService::authenticate(const std::string &temp_token) {
  const std::string body = "{\"code\":\"" + temp_token + "\"}";

  auto response = m_http_client->postJSON(API_BASE_URL "/devices/login", body);
  Logger::info("Response: " + response.response_content);

  if (response.httpStatusCode != 200) {
    Logger::error("Failed to retrieve authentication token");
    return false;
  }

  Logger::debug("Storing authentication token...");
  m_non_volatile_storage->setValue(STORAGE_USERCONFIG, KEY_DEVICETOKEN,
                                   response.response_content);
  m_authentication_token = response.response_content;

  return true;
}

bool AuthenticationService::isAuthenticated() {
  return !m_authentication_token.empty();
}

std::string AuthenticationService::getAuthenticationToken() {
  return m_authentication_token;
}

void AuthenticationService::reset() {
  Logger::warn("Resetting authentication token");
  m_non_volatile_storage->setValue(STORAGE_USERCONFIG, KEY_DEVICETOKEN, "");
  Logger::warn("Restarting device");
  esp_restart();
}

void AuthenticationService::loadAuthenticationToken() {
  // load the authentication token, if no token is stored, the string will be
  // empty
  m_non_volatile_storage->getValue(STORAGE_USERCONFIG, KEY_DEVICETOKEN,
                                   &m_authentication_token);
}