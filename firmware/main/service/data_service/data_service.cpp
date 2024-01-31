#include "main/service/data_service/data_service.h"

#include "main/config.h"
#include "main/hal/http_client/http_client.h"
#include "main/logger/logger.h"

DataService::DataService(HTTPClient* http_client,
                         AuthenticationService* auth_service, BME680* bme680)
    : m_http_client(http_client),
      m_auth_service(auth_service),
      m_bme680(bme680),
      m_data_upload_task_handle(NULL) {}

DataService::~DataService() {}

bool DataService::startDataUploadTask() {
  Logger::info("Starting data upload task...");
  if (m_data_upload_task_handle != NULL) {
    Logger::error("Data upload task already running");
    return false;
  }

  // create task for sendAirQualityData
  xTaskCreate(
      [](void* data_service_ptr) {
        DataService* data_service = (DataService*)data_service_ptr;

        while (true) {
          if (!data_service->sendAirQualityData()) {
            Logger::error("Failed to send air quality data");
          }

          vTaskDelay(10000 / portTICK_PERIOD_MS);
        }
      },
      "data_upload_task", 4096, this, 5, &m_data_upload_task_handle);
  Logger::info("Finished starting data upload task");
  return true;
}

bool DataService::stopDataUploadTask() {
  if (m_data_upload_task_handle == NULL) {
    Logger::info("Data upload task not running");
    return true;
  }

  vTaskDelete(m_data_upload_task_handle);
  m_data_upload_task_handle = NULL;
  return true;
}

bool DataService::sendAirQualityData() {
  if (!m_auth_service->isAuthenticated()) {
    Logger::error("Not authenticated");
    return false;
  }

  m_bme680->readData();

  const std::string body =
      "{\"temp\":" + std::to_string(m_bme680->getTemperature()) +
      ",\"humidity\":" + std::to_string(m_bme680->getHumidity()) +
      ",\"pressure\":" + std::to_string(m_bme680->getPressure()) +
      ",\"gasResistance\":" + std::to_string(m_bme680->getGas()) + "}";

  auto response = m_http_client->postJSON(
      API_BASE_URL "/data", body, m_auth_service->getAuthenticationToken());

  if (response.httpStatusCode != 200) {
    Logger::error("Failed to send air quality data, status code: " +
                  std::to_string(response.httpStatusCode));

    if (response.httpStatusCode == 401) {
      m_auth_service->reset();
    }

    return false;
  }
  return true;
}