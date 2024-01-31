#include "main/ui/image_ui/image_ui.h"

#include "main/logger/logger.h"

ImageUI::ImageUI(EInk* eink) : m_eink(eink) {}

ImageUI::~ImageUI() {}

void ImageUI::showSetupScreen() { showImageOnly("setup.bmp"); }

void ImageUI::showConnectingScreen() { showImageOnly("conn.bmp"); }

void ImageUI::showWrongWifiCredentialsScreen() { showImageOnly("fwifi.bmp"); }

void ImageUI::showWrongDeviceTokenScreen() { showImageOnly("ftoken.bmp"); }

void ImageUI::showConnectedScreen() { showImageOnly("connf.bmp"); }

void ImageUI::showStartupScreen() { showImageOnly("load.bmp"); }

void ImageUI::showImageOnly(const std::string& image_name) {
  Logger::debug("Show Image: " + image_name);
  m_eink->clearDisplay();
  m_eink->drawBitmap(0, 0, image_name);
  m_eink->updateDisplay();
}