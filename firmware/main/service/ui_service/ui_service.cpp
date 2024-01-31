#include "main/service/ui_service/ui_service.h"

#include "main/logger/logger.h"

UIService::UIService(EInk* eink, DataDownloadService* data_download_service,
                     HomeUI* home_ui, ImageUI* image_ui)
    : m_eink(eink),
      m_data_download_service(data_download_service),
      m_home_ui(home_ui),
      m_image_ui(image_ui),
      m_x_pos(0),
      m_y_pos(0) {}

UIService::~UIService() {}

void UIService::show() {
  Logger::debug("Min: " + std::to_string(getMinXPos()) +
                " Max: " + std::to_string(getMaxXPos()) +
                " Current: " + std::to_string(m_x_pos));
  update();
}

void UIService::moveLeft() {
  if (m_x_pos == getMinXPos()) {
    m_x_pos = getMaxXPos();
  } else {
    m_x_pos--;
  }
  Logger::debug("Move left to " + std::to_string(m_x_pos) + "/" +
                std::to_string(getMinXPos()));
  show();
}

void UIService::moveRight() {
  if (m_x_pos == getMaxXPos()) {
    m_x_pos = getMinXPos();
  } else {
    m_x_pos++;
  }
  Logger::debug("Move right to " + std::to_string(m_x_pos) + "/" +
                std::to_string(getMaxXPos()));
  show();
}

void UIService::update() {
  if (m_x_pos == 0) {
    m_home_ui->show();
    return;
  }
  m_home_ui->showSensorHome(m_x_pos - 1);
};

int8_t UIService::getMaxXPos() {
  // count of sensors + 1 for the home screen
  return m_data_download_service->getAirQualityData().size();
}

int8_t UIService::getMinXPos() { return 0; }
