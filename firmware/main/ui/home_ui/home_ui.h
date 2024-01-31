#pragma once

#include <map>
#include <string>

#include "main/driver/eink/eink.h"
#include "main/service/data_download_service/data_download_service.h"

class HomeUI {
 public:
  //! @brief Constructor
  //! @param eink The eink driver
  //! @param data_download_service The data download service
  HomeUI(EInk* eink, DataDownloadService* data_download_service);

  //! @brief Destructor
  ~HomeUI();

  //! @brief Show the home screen.
  void show();

  //! @brief Show the home screen with the given sensor.
  //! @param sensor_name The sensor id
  void showSensorHome(const uint8_t sensor_id);

 private:
  //! @brief Show the home screen with zero sensors.
  void showZeroSensorScreen();

  //! @brief Show the home screen with one sensor.
  //! @param air_data The air quality data
  void showOneSensorScreen(const AirQualityData& air_data);

  //! @brief Show the home screen with two sensors.
  //! @param air_data1 The first air quality data
  //! @param air_data2 The second air quality data
  void showTwoSensorScreen(const AirQualityData& air_data1,
                           const AirQualityData& air_data2);

  //! @brief Show the home screen with three sensors.
  //! @param air_data1 The first air quality data
  //! @param air_data2 The second air quality data
  //! @param air_data3 The third air quality data
  void showThreeSensorScreen(const AirQualityData& air_data1,
                             const AirQualityData& air_data2,
                             const AirQualityData& air_data3);

  //! @brief Convert a float to a string with the given precision.
  //! @param value The float value
  //! @param precision The precision
  //! @return The string representation of the float
  std::string convertFloatToString(float value, uint8_t precision);

  //! @brief Pointer to the eink driver
  EInk* m_eink;

  //! @brief Pointer to the data download service
  DataDownloadService* m_data_download_service;
};
