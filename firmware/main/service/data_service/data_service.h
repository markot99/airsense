#pragma once

#include "main/driver/bme680/bme680.h"
#include "main/hal/http_client/http_client.h"
#include "main/service/authentication_service/authentication_service.h"

class DataService {
 public:
  //! @brief Constructor
  //! @param http_client The http client
  //! @param auth_service The authentication service
  //! @param bme680 The BME680 driver
  DataService(HTTPClient* http_client, AuthenticationService* auth_service,
              BME680* bme680);

  //! @brief Destructor
  ~DataService();

  //! @brief Start the air quality data upload task
  bool startDataUploadTask();

  //! @brief Stop the air quality data upload task
  bool stopDataUploadTask();

 private:
  //! @brief Send the air quality data to the server
  //! @return True if the air quality data was sent successfully, false
  //! otherwise
  bool sendAirQualityData();

  //! @brief Pointer to the http client
  HTTPClient* m_http_client;

  //! @brief Pointer to the authentication service
  AuthenticationService* m_auth_service;

  //! @brief Pointer to the BME680 driver
  BME680* m_bme680;

  //! @brief The air quality data upload task handle
  TaskHandle_t m_data_upload_task_handle;
};