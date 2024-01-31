#pragma once

#include <map>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "main/driver/bme680/bme680.h"
#include "main/hal/http_client/http_client.h"
#include "main/service/authentication_service/authentication_service.h"

//! @brief The air quality data of a device
struct AirQualityData {
  //! @brief The device name
  std::string device_name;
  //! @brief The temperature in degrees celsius
  float temperature;
  //! @brief The humidity in percent
  float humidity;
  //! @brief The pressure in pascal
  uint32_t pressure;
  //! @brief The gas resistance in ohm
  uint32_t gas_resistance;
};

class DataDownloadService {
 public:
  //! @brief Constructor
  //! @param http_client The http client
  //! @param auth_service The authentication service
  DataDownloadService(HTTPClient* http_client,
                      AuthenticationService* auth_service);

  //! @brief Destructor
  ~DataDownloadService();

  //! @brief Start the air quality data download task
  bool startDataDownloadTask();

  //! @brief Stop the air quality data download task
  bool stopDataDownloadTask();

  //! @brief retrieve the cached air quality data
  //! @note using cached data is faster than downloading the data from the
  //! server
  //! @return a map of the air quality data
  //! @note the map key is the device id
  std::map<uint32_t, AirQualityData> getAirQualityData();

 private:
  //! @brief retrieve the air quality data from the server and cache it
  //! @return True if the air quality data was downloaded successfully, false
  //! otherwise
  bool downloadAirQualityData();

  //! @brief Pointer to the http client
  HTTPClient* m_http_client;

  //! @brief Pointer to the authentication service
  AuthenticationService* m_auth_service;

  //! @brief The air quality data download task handle
  TaskHandle_t m_data_download_task_handle;

  //! @brief The cached air quality data
  //! @note the map key is the device id
  std::map<uint32_t, AirQualityData> m_air_quality_data;

  //! mutex to protect the cached air quality data
  SemaphoreHandle_t m_mutex;
};