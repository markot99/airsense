#include "main/runtime/runtime.h"

#include "esp_timer.h"
#include "main/config.h"
#include "main/hal/timer/timer.h"
#include "main/logger/logger.h"

Runtime::Runtime()
    : m_i2c(nullptr),
      m_uart(nullptr),
      m_non_volatile_storage(nullptr),
      m_wifi(nullptr),
      m_http_server(nullptr),
      m_upload_data_http_client(nullptr),
      m_download_data_http_client(nullptr),
      m_registration_portal(nullptr),
      m_eink(nullptr),
      m_apds9960(nullptr),
      m_bme680(nullptr),
      m_ui_service(nullptr),
      m_authentication_service(nullptr),
      m_data_service(nullptr),
      m_data_download_service(nullptr),
      m_home_ui(nullptr),
      m_image_ui(nullptr) {}

Runtime::~Runtime() {
  delete m_home_ui;
  delete m_data_download_service;
  delete m_data_service;
  delete m_authentication_service;
  delete m_ui_service;
  delete m_bme680;
  delete m_apds9960;
  delete m_eink;
  delete m_registration_portal;
  delete m_download_data_http_client;
  delete m_upload_data_http_client;
  delete m_http_server;
  delete m_wifi;
  delete m_non_volatile_storage;
  delete m_uart;
  delete m_i2c;
  delete m_display_wakeup_pin;
  delete m_led_pin;
}

void Runtime::initialize() {
  m_led_pin = new DigitalOutputPin(2);
  m_led_pin->setHigh();

  m_display_wakeup_pin = new DigitalOutputPin(25);

  m_uart = new Uart(16, 17, 115200);

  m_eink = new EInk(m_uart, m_display_wakeup_pin);

  m_image_ui = new ImageUI(m_eink);
  m_image_ui->showStartupScreen();
  Timer::sleepMS(2000);

  m_i2c = new I2C(21, 22, 1000);

  m_non_volatile_storage = new NonVolatileStorage();

  m_wifi = new Wifi(m_non_volatile_storage);

  m_http_server = new HTTPServer();

  m_upload_data_http_client = new HTTPClient();

  m_download_data_http_client = new HTTPClient();

  m_authentication_service = new AuthenticationService(
      m_upload_data_http_client, m_non_volatile_storage);

  m_registration_portal = new RegistrationPortal(
      m_wifi, m_http_server, m_authentication_service, m_image_ui);

  m_bme680 = new BME680(m_i2c);

  m_apds9960 = new APDS9960(m_i2c);

  m_data_service = new DataService(m_upload_data_http_client,
                                   m_authentication_service, m_bme680);

  m_data_download_service = new DataDownloadService(m_download_data_http_client,
                                                    m_authentication_service);

  m_home_ui = new HomeUI(m_eink, m_data_download_service);

  m_ui_service =
      new UIService(m_eink, m_data_download_service, m_home_ui, m_image_ui);

  Logger::debug("Runtime initialized");
  // check if the device is authenticated and try to connect to the wifi
  if (!m_authentication_service->isAuthenticated()) {
    Logger::debug("Device is not authenticated");
    m_registration_portal->start();
  } else {
    if (!m_wifi->connectUsingStoredCredentials()) {
      Logger::debug("Wifi credentials are not valid");
      m_registration_portal->start();
    }
  }

  m_data_service->startDataUploadTask();

  Logger::debug("Device is authenticated");

  // download data is only needed if device is a base station is connected
  if (m_apds9960->isConnected()) {
    m_data_download_service->startDataDownloadTask();

    // wait for the first data to be downloaded
    while (m_data_download_service->getAirQualityData().size() == 0) {
      Timer::sleepMS(1000);
      Logger::debug("Waiting for first data to be downloaded");
    }
  }
}

void Runtime::run() {
  if (!m_apds9960->isConnected()) {
    Logger::debug("No gesture sensor connected, skipping gesture detection");
    vTaskDelay(60000 / portTICK_PERIOD_MS);
  }

  // initial clear to remove previous image artifacts
  // m_eink->clearDisplay();
  // m_eink->updateDisplay();

  m_ui_service->show();

  auto start_time = esp_timer_get_time();
  while (1) {
    auto gesture = m_apds9960->readGesture();

    if (gesture == APDS9960_LEFT) {
      Logger::debug("Left gesture detected");
      m_ui_service->moveLeft();
      start_time = esp_timer_get_time();
    } else if (gesture == APDS9960_RIGHT) {
      Logger::debug("Right gesture detected");
      m_ui_service->moveRight();
      start_time = esp_timer_get_time();
    }
    Timer::sleepMS(1);

    // refresh after 30 seconds
    if (esp_timer_get_time() - start_time > 30000000) {
      m_ui_service->show();
      start_time = esp_timer_get_time();
    }
  }
}
