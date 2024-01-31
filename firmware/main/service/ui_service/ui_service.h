#pragma once

#include <string>

#include "main/driver/eink/eink.h"
#include "main/service/data_download_service/data_download_service.h"
#include "main/ui/home_ui/home_ui.h"
#include "main/ui/image_ui/image_ui.h"

class UIService {
 public:
  //! @brief Constructor
  //! @param eink The eink driver
  //! @param data_download_service The data download service
  //! @param home_ui The home ui
  //! @param image_ui The image ui
  UIService(EInk* eink, DataDownloadService* data_download_service,
            HomeUI* home_ui, ImageUI* image_ui);

  //! @brief Destructor
  ~UIService();

  //! @brief Show the initial screen
  void show();

  //! @brief Move the screen to the left
  void moveLeft();

  //! @brief Move the screen to the right
  void moveRight();

 private:
  //! @brief Update the display with the new position
  void update();

  //! @brief Get the maximum x position
  //! @return The maximum x position
  int8_t getMaxXPos();

  //! @brief Get the minimum x position
  //! @return The minimum x position
  int8_t getMinXPos();

  //! @brief Pointer to the eink driver
  EInk* m_eink;

  //! @brief Pointer to the data download service
  DataDownloadService* m_data_download_service;

  //! @brief Pointer to the home ui
  HomeUI* m_home_ui;

  //! @brief Pointer to the image ui
  ImageUI* m_image_ui;

  //! @brief The x position of the current screen
  int8_t m_x_pos;

  //! @brief The y position of the current screen
  int8_t m_y_pos;
};
