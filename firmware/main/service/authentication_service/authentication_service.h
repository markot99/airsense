#pragma once

#include <string>

#include "main/hal/http_client/http_client.h"
#include "main/hal/non_volatile_storage/non_volatile_storage.h"

class AuthenticationService {
 public:
  //! @brief Constructor
  //! @param http_client The http client
  //! @param non_volatile_storage The non volatile storage
  AuthenticationService(HTTPClient* http_client,
                        NonVolatileStorage* non_volatile_storage);

  //! @brief Destructor
  ~AuthenticationService();

  //! @brief Retrieve the authentication token from the server, store it in the
  //! non volatile storage and update the authentication token
  //! @param temp_token The temporary token to authenticate with
  //! @return True if the authentication token was retrieved successfully, false
  //! otherwise
  bool authenticate(const std::string& temp_token);

  //! @brief Check if the device is authenticated
  //! @return True if the device is authenticated, false otherwise
  bool isAuthenticated();

  //! @brief Get the authentication token
  //! @return The authentication token, empty string if not authenticated
  std::string getAuthenticationToken();

  //! @brief Reset the authentication token and restart the device
  void reset();

 private:
  //! @brief Load the authentication token from the non volatile storage
  void loadAuthenticationToken();

  //! @brief Pointer to the http client
  HTTPClient* m_http_client;

  //! @brief Pointer to the non volatile storage
  NonVolatileStorage* m_non_volatile_storage;

  //! @brief The authentication token
  std::string m_authentication_token;
};
