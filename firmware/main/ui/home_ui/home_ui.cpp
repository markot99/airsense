#include "main/ui/home_ui/home_ui.h"

#include <iomanip>
#include <sstream>

#include "main/logger/logger.h"

HomeUI::HomeUI(EInk* eink, DataDownloadService* data_download_service)
    : m_eink(eink), m_data_download_service(data_download_service) {}

HomeUI::~HomeUI() {}

void HomeUI::show() {
  Logger::debug("Showing home screen");

  auto data = m_data_download_service->getAirQualityData();
  Logger::debug("Showing home screen with " + std::to_string(data.size()) +
                " sensors");
  // switch data size to show the different screens
  switch (data.size()) {
    case 0:
      showZeroSensorScreen();
      break;
    case 1:
      showOneSensorScreen(data[0]);
      break;
    case 2:
      showTwoSensorScreen(data[0], data[1]);
      break;
    case 3:
      showThreeSensorScreen(data[0], data[1], data[2]);
      break;
    default:
      showThreeSensorScreen(data[0], data[1], data[2]);
      break;
  }
}

void HomeUI::showSensorHome(const uint8_t sensor_id) {
  Logger::debug("Showing home screen with sensor " + std::to_string(sensor_id));
  auto data = m_data_download_service->getAirQualityData()[sensor_id];
  showOneSensorScreen(data);
}

void HomeUI::showZeroSensorScreen() {
  m_eink->clearDisplay();
  m_eink->drawBitmap(0, 0, "serror.bmp");
  m_eink->updateDisplay();
}

void HomeUI::showOneSensorScreen(const AirQualityData& air_data) {
  m_eink->clearDisplay();

  m_eink->drawBitmap(0, 0, "homeone.bmp");

  // draw the sensor name
  m_eink->setFontSize(FontSize::LARGE);

  // calucate size of text, large means 30 px per character
  uint16_t text_width = air_data.device_name.size() * 30;

  // place text in the middle of the display
  uint16_t x = (800 - text_width) / 2;
  uint16_t y = 0;

  m_eink->drawText(x, y, air_data.device_name);

  // draw the temperature
  m_eink->setFontSize(FontSize::LARGE);

  m_eink->drawText(250, 140, convertFloatToString(air_data.temperature, 2));
  m_eink->drawText(250, 265, convertFloatToString(air_data.humidity, 2));
  m_eink->drawText(250, 390, std::to_string(air_data.pressure));
  m_eink->drawText(250, 517, std::to_string(air_data.gas_resistance));

  m_eink->updateDisplay();
}

void HomeUI::showTwoSensorScreen(const AirQualityData& air_data1,
                                 const AirQualityData& air_data2) {
  m_eink->clearDisplay();

  m_eink->drawBitmap(0, 0, "hometwo.bmp");

  // draw first sensor name
  m_eink->setFontSize(FontSize::LARGE);

  // calucate size of text, large means 30 px per character
  uint16_t text_width = air_data1.device_name.size() * 30;

  // place text in the middle of the display
  uint16_t x = (400 - text_width) / 2;
  uint16_t y = 0;

  m_eink->drawText(x, y, air_data1.device_name);

  // draw second sensor name
  text_width = air_data2.device_name.size() * 30;
  x = 410 + (400 - text_width) / 2;
  m_eink->drawText(x, y, air_data2.device_name);

  // draw the temperature of the first sensor
  m_eink->drawText(170, 140, convertFloatToString(air_data1.temperature, 2));
  m_eink->drawText(170, 265, convertFloatToString(air_data1.humidity, 2));
  m_eink->drawText(170, 390, std::to_string(air_data1.pressure));

  if (air_data1.gas_resistance > 99999) {
    m_eink->drawText(170, 517, std::to_string(air_data1.gas_resistance / 1000));
    m_eink->drawText(280, 517, "k");
  } else {
    m_eink->drawText(170, 517, std::to_string(air_data1.gas_resistance));
  }

  // draw the temperature of the second sensor
  m_eink->drawText(570, 140, convertFloatToString(air_data2.temperature, 2));
  m_eink->drawText(570, 265, convertFloatToString(air_data2.humidity, 2));
  m_eink->drawText(570, 390, std::to_string(air_data2.pressure));

  if (air_data2.gas_resistance > 99999) {
    m_eink->drawText(570, 517, std::to_string(air_data2.gas_resistance / 1000));
    m_eink->drawText(680, 517, "k");
  } else {
    m_eink->drawText(570, 517, std::to_string(air_data2.gas_resistance));
  }

  m_eink->updateDisplay();
}

void HomeUI::showThreeSensorScreen(const AirQualityData& air_data1,
                                   const AirQualityData& air_data2,
                                   const AirQualityData& air_data3) {
  m_eink->clearDisplay();

  m_eink->drawBitmap(0, 0, "hometh.bmp");

  // draw first sensor name
  m_eink->setFontSize(FontSize::MEDIUM);

  // calucate size of text, medium means 15 px per character
  uint16_t text_width = air_data1.device_name.size() * 15;

  // place text in the middle of the display
  uint16_t x = ((250 - text_width) / 2) - 30;
  uint16_t y = 0;

  m_eink->drawText(x, y, air_data1.device_name);

  // draw second sensor name
  text_width = air_data2.device_name.size() * 15;
  x = 240 + (250 - text_width) / 2;
  m_eink->drawText(x, y, air_data2.device_name);

  // draw third sensor name
  text_width = air_data3.device_name.size() * 15;
  x = 500 + (250 - text_width) / 2;
  m_eink->drawText(x, y, air_data3.device_name);

  m_eink->setFontSize(FontSize::LARGE);

  // draw the temperature of the first sensor
  m_eink->drawText(20, 140, convertFloatToString(air_data1.temperature, 2));
  m_eink->drawText(20, 265, convertFloatToString(air_data1.humidity, 2));
  m_eink->drawText(20, 390, std::to_string(air_data1.pressure));

  if (air_data1.gas_resistance > 99999) {
    m_eink->drawText(20, 517, std::to_string(air_data1.gas_resistance / 1000));
    m_eink->drawText(140, 517, "k");
  } else {
    m_eink->drawText(20, 517, std::to_string(air_data1.gas_resistance));
  }

  // draw the temperature of the second sensor
  m_eink->drawText(285, 140, convertFloatToString(air_data2.temperature, 2));
  m_eink->drawText(285, 265, convertFloatToString(air_data2.humidity, 2));
  m_eink->drawText(285, 390, std::to_string(air_data2.pressure));

  if (air_data2.gas_resistance > 99999) {
    m_eink->drawText(285, 517, std::to_string(air_data2.gas_resistance / 1000));
    m_eink->drawText(405, 517, "k");
  } else {
    m_eink->drawText(285, 517, std::to_string(air_data2.gas_resistance));
  }

  // draw the temperature of the third sensor
  m_eink->drawText(555, 140, convertFloatToString(air_data3.temperature, 2));
  m_eink->drawText(555, 265, convertFloatToString(air_data3.humidity, 2));
  m_eink->drawText(555, 390, std::to_string(air_data3.pressure));

  if (air_data3.gas_resistance > 99999) {
    m_eink->drawText(555, 517, std::to_string(air_data3.gas_resistance / 1000));
    m_eink->drawText(675, 517, "k");
  } else {
    m_eink->drawText(555, 517, std::to_string(air_data3.gas_resistance));
  }

  m_eink->updateDisplay();
}

std::string HomeUI::convertFloatToString(float value, uint8_t precision) {
  std::stringstream float_str;
  float_str << std::fixed << std::setprecision(precision) << value;
  return float_str.str();
}