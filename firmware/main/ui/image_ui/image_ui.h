#pragma once

#include <string>

#include "main/driver/eink/eink.h"

class ImageUI {
 public:
  //! @brief Constructor
  ImageUI(EInk* display_service);

  //! @brief Destructor
  ~ImageUI();

  //! @brief Show the setup screen
  void showSetupScreen();

  //! @brief Show the operation screen
  void showConnectingScreen();

  //! @brief Show the operation screen
  void showWrongWifiCredentialsScreen();

  //! @brief Show the operation screen
  void showWrongDeviceTokenScreen();

  //! @brief Show the operation screen
  void showConnectedScreen();

  //! @brief Show the startup screen
  void showStartupScreen();

 private:
  //! @brief Show the image with the given name and update the display
  //! @param image_name The name of the image to show
  void showImageOnly(const std::string& image_name);

  //! @brief Pointer to the eink driver
  EInk* m_eink;
};
