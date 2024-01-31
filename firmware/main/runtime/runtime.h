#pragma once

#include <cstdint>

#include "main/driver/apds9960/apds9960.h"
#include "main/driver/bme680/bme680.h"
#include "main/driver/eink/eink.h"
#include "main/hal/digital_output_pin/digital_output_pin.h"
#include "main/hal/http_client/http_client.h"
#include "main/hal/http_server/http_server.h"
#include "main/hal/i2c/i2c.h"
#include "main/hal/non_volatile_storage/non_volatile_storage.h"
#include "main/hal/registration_portal/registration_portal.h"
#include "main/hal/uart/uart.h"
#include "main/hal/wifi/wifi.h"
#include "main/service/authentication_service/authentication_service.h"
#include "main/service/data_download_service/data_download_service.h"
#include "main/service/data_service/data_service.h"
#include "main/service/ui_service/ui_service.h"
#include "main/ui/home_ui/home_ui.h"
#include "main/ui/image_ui/image_ui.h"

//! @brief Runtime class to run the application.
class Runtime {
 public:
  //! @brief Constructor
  Runtime();

  //! @brief Destructor
  ~Runtime();

  //! @brief Initialize the runtime.
  void initialize();

  //! @brief Run the application.
  void run();

 private:
  //! @brief The current runtime state.
  uint8_t m_runtime_state;

  //! @brief The LED pin.
  DigitalOutputPin* m_led_pin;

  //! @brief The display wakeup pin.
  DigitalOutputPin* m_display_wakeup_pin;

  //! @brief The I2C hal.
  I2C* m_i2c;

  //! @brief The UART hal.
  Uart* m_uart;

  //! @brief The non volatile storage.
  NonVolatileStorage* m_non_volatile_storage;

  //! @brief The wifi driver.
  Wifi* m_wifi;

  //! @brief The HTTP server.
  HTTPServer* m_http_server;

  //! @brief The HTTP client for data upload.
  HTTPClient* m_upload_data_http_client;

  //! @brief The HTTP client for data download.
  HTTPClient* m_download_data_http_client;

  //! @brief The registration portal.
  RegistrationPortal* m_registration_portal;

  //! @brief The eink driver.
  EInk* m_eink;

  //! @brief The APDS9960 driver.
  APDS9960* m_apds9960;

  //! @brief The BME680 driver.
  BME680* m_bme680;

  //! @brief The UI service.
  UIService* m_ui_service;

  //! @brief The authentication service.
  AuthenticationService* m_authentication_service;

  //! @brief The data service.
  DataService* m_data_service;

  //! @brief The data download service
  DataDownloadService* m_data_download_service;

  //! @brief The home UI.
  HomeUI* m_home_ui;

  //! @brief The image UI.
  ImageUI* m_image_ui;
};
