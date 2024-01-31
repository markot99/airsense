#include "main/service/data_download_service/data_download_service.h"

#include "main/config.h"
#include "main/hal/timer/timer.h"
#include "main/libs/cJson/cJSON.h"

DataDownloadService::DataDownloadService(HTTPClient *http_client,
                                         AuthenticationService *auth_service)
    : m_http_client(http_client),
      m_auth_service(auth_service),
      m_data_download_task_handle(NULL),
      m_mutex(xSemaphoreCreateMutex()) {}

DataDownloadService::~DataDownloadService() {}

bool DataDownloadService::startDataDownloadTask() {
  Logger::info("Starting data download task...");
  if (m_data_download_task_handle != NULL) {
    Logger::error("Data upload task already running");
    return false;
  }

  // create task for sendAirQualityData
  xTaskCreate(
      [](void *data_download_service_ptr) {
        DataDownloadService *data_download_service =
            (DataDownloadService *)data_download_service_ptr;
        while (true) {
          if (!data_download_service->downloadAirQualityData()) {
            Logger::error("Failed to download air quality data");
          }
          Timer::sleepMS(30000);
        }
      },
      "data_download_task", 8192, this, 5, &m_data_download_task_handle);
  Logger::info("Finished starting data download task");
  return true;
}

bool DataDownloadService::stopDataDownloadTask() {
  if (m_data_download_task_handle == NULL) {
    Logger::info("Data download task not running");
    return true;
  }

  vTaskDelete(m_data_download_task_handle);
  m_data_download_task_handle = NULL;
  return true;
}

std::map<uint32_t, AirQualityData> DataDownloadService::getAirQualityData() {
  std::map<uint32_t, AirQualityData> air_quality_data;
  if (xSemaphoreTake(m_mutex, (TickType_t)10) == pdTRUE) {
    air_quality_data = m_air_quality_data;
    xSemaphoreGive(m_mutex);
  } else {
    Logger::error("Failed to take mutex lock for getting data");
  }
  return air_quality_data;
}

bool DataDownloadService::downloadAirQualityData() {
  std::map<uint32_t, AirQualityData> air_quality_data;
  if (!m_auth_service->isAuthenticated()) {
    Logger::error("Not authenticated");
    return false;
  }

  auto response = m_http_client->getJSON(
      API_BASE_URL "/sensors/latest", m_auth_service->getAuthenticationToken());

  if (response.httpStatusCode != 200) {
    Logger::error("Failed to send air quality data, status code: " +
                  std::to_string(response.httpStatusCode));

    if (response.httpStatusCode == 401) {
      m_auth_service->reset();
    }

    return false;
  }

  cJSON *json = cJSON_Parse(response.response_content.c_str());
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      Logger::error("Error before: " + std::string(error_ptr));
    }
    return false;
  }

  if (!cJSON_IsArray(json)) {
    Logger::debug("Error: Expected an array\n");
    cJSON_Delete(json);
    return false;
  }

  cJSON *element;
  uint32_t index = 0;

  cJSON_ArrayForEach(element, json) {
    cJSON *device = cJSON_GetObjectItem(element, "device");
    if ((device == NULL) || (!cJSON_IsString(device))) {
      Logger::error("Wrong data format");
      cJSON_Delete(json);
      return false;
    }

    cJSON *humidity = cJSON_GetObjectItem(element, "humidity");
    if ((humidity == NULL) || (!cJSON_IsNumber(humidity))) {
      Logger::error("Wrong data format");
      cJSON_Delete(json);
      return false;
    }

    cJSON *pressure = cJSON_GetObjectItem(element, "pressure");
    if ((pressure == NULL) || (!cJSON_IsNumber(pressure))) {
      Logger::error("Wrong data format");
      cJSON_Delete(json);
      return false;
    }

    cJSON *temperature = cJSON_GetObjectItem(element, "temperature");
    if ((temperature == NULL) || (!cJSON_IsNumber(temperature))) {
      Logger::error("Wrong data format");
      cJSON_Delete(json);
      return false;
    }

    cJSON *gasResistance = cJSON_GetObjectItem(element, "gasResistance");
    if ((gasResistance == NULL) || (!cJSON_IsNumber(gasResistance))) {
      Logger::error("Wrong data format");
      cJSON_Delete(json);
      return false;
    }

    air_quality_data[index] = AirQualityData{
        .device_name = std::string(device->valuestring),
        .temperature = static_cast<float>(temperature->valuedouble),
        .humidity = static_cast<float>(humidity->valuedouble),
        .pressure = static_cast<uint32_t>(pressure->valueint),
        .gas_resistance = static_cast<uint32_t>(gasResistance->valueint),
    };
    index++;
  }

  cJSON_Delete(json);

  if (xSemaphoreTake(m_mutex, (TickType_t)10) == pdTRUE) {
    m_air_quality_data = air_quality_data;
    xSemaphoreGive(m_mutex);
  } else {
    Logger::error("Failed to take mutex lock for setting data");
    return false;
  }

  return true;
}