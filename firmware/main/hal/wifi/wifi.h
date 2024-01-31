#pragma once

#include <string>

#include "esp_http_server.h"
#include "main/hal/non_volatile_storage/non_volatile_storage.h"

enum WIFIMode { MODE_OFF, MODE_AP, MODE_STA, MODE_APSTA };

//! @brief The wifi driver
class Wifi {
 public:
  //! @brief Constructor
  //! @param non_volatile_storage The non volatile storage.
  Wifi(NonVolatileStorage* non_volatile_storage);

  //! @brief Destructor
  ~Wifi();

  //! @brief Start wifi in access point mode.
  void startAccesspoint();

  //! @brief Start wifi in station mode.
  bool startStation(const std::string& ssid, const std::string& password);

  //! @brief Connect to the wifi using the stored credentials.
  bool connectUsingStoredCredentials();

  //! @brief Stop wifi
  //! @note This will stop the wifi in both modes.
  void stop();

  //! @brief Store the wifi credentials in the non volatile storage.
  //! @param ssid The ssid.
  //! @param password The password.
  //! @return True if the credentials were stored successfully, false otherwise.
  bool storeWifiCredentials(const std::string& ssid,
                            const std::string& password);
  //! @brief Start default initialization for all wifi modes.
  void init();

 private:
  //! @brief Start the dns server.
  //! @note used to provide the captive portal.
  void start_dns();

  //! @brief Stop the dns server.
  void stop_dns();

  //! @brief Initialize the wifi in access point mode.
  void wifi_init_softap();

  //! @brief Get the wifi ssid
  std::string get_wifi_ssid();

  //! @brief Get the wifi password
  std::string get_wifi_password();

  //! @brief The wifi event handler.
  //! @param arg The argument.
  //! @param event_base The event base.
  //! @param event_id The event id.
  //! @param event_data The event data.
  void wifi_event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data);

  //! @brief The non volatile storage.
  NonVolatileStorage* m_non_volatile_storage;

  //! @brief The current wifi mode.
  WIFIMode m_current_mode;

  //! @brief Flag if wifi is connected. True if connected, false otherwise.
  bool m_connected;

  //! @brief The number of retries made to connect to the wifi.
  uint8_t m_retries;
};
