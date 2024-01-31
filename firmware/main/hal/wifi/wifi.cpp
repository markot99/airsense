#include "main/hal/wifi/wifi.h"

#include <algorithm>
#include <string>

#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "lwip/dns.h"
#include "lwip/inet.h"
#include "main/config.h"
#include "main/hal/dns_server/dns_server.h"
#include "main/hal/timer/timer.h"
#include "main/logger/logger.h"

Wifi::Wifi(NonVolatileStorage* non_volatile_storage)
    : m_non_volatile_storage(non_volatile_storage),
      m_current_mode(WIFIMode::MODE_OFF),
      m_connected(false),
      m_retries(0) {
  init();
}

Wifi::~Wifi() {}

void Wifi::startAccesspoint() {
  wifi_init_softap();
  start_dns();
}

void Wifi::start_dns() { start_dns_server(); }

void Wifi::stop_dns() { stop_dns_server(); }

void Wifi::wifi_event_handler(void* arg, esp_event_base_t event_base,
                              int32_t event_id, void* event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    esp_wifi_connect();
    Logger::debug("Versuche, erneut zu verbinden...");
    m_retries++;
    Logger::debug("Retries 2: " + std::to_string(m_retries) + "/50");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
    Logger::debug("IP-Adresse erhalten: " +
                  std::string(inet_ntoa(event->ip_info.ip)));
    m_connected = true;
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
    m_connected = true;
    Logger::debug("connected");
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    esp_wifi_connect();
  }
  Logger::debug("Event Base: " + std::string(event_base) +
                " Event ID: " + std::to_string(event_id));
}

bool Wifi::startStation(const std::string& ssid, const std::string& password) {
  Logger::debug("SSID: +" + ssid + "+");
  Logger::debug("Password: +" + password + "+");

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  esp_wifi_disable_pmf_config(WIFI_IF_STA);
  wifi_config_t wifi_config{};

  std::copy(ssid.begin(), ssid.end(), wifi_config.sta.ssid);
  std::copy(password.begin(), password.end(), wifi_config.sta.password);

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  // wait until connected
  while (m_connected == false) {
    Timer::sleepMS(1000);
    Logger::debug("Waiting for connection...");
    Logger::debug("Retries 1: " + std::to_string(m_retries) + "/50");

    if (m_retries > WIFI_CONNECT_MAX_RETRIES) {
      Logger::debug("Too many retries, stopping...");
      m_retries = 0;
      stop();
      return false;
    }
  }

  ip_addr_t dns_server;
  IP_ADDR4(&dns_server, 8, 8, 8, 8);  // Google DNS
  dns_setserver(0, &dns_server);

  m_current_mode = WIFIMode::MODE_STA;
  return true;
}

bool Wifi::connectUsingStoredCredentials() {
  std::string ssid = get_wifi_ssid();
  std::string password = get_wifi_password();

  if (ssid.empty() || password.empty()) {
    Logger::debug("No stored credentials found.");
    return false;
  }

  Logger::debug("SSID: +" + ssid + "+");
  Logger::debug("Password: +" + password + "+");

  Logger::debug("Connecting using stored credentials...");
  return startStation(ssid, password);
}

void Wifi::stop() {
  Logger::debug("Stopping wifi...");
  if (m_current_mode == WIFIMode::MODE_STA) {
    Logger::debug("Stopping wifi in station mode...");
    m_connected = false;
    ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
    m_current_mode = WIFIMode::MODE_OFF;
  } else if (m_current_mode == WIFIMode::MODE_AP) {
    Logger::debug("Stopping wifi in access point mode...");
    stop_dns();
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
    m_current_mode = WIFIMode::MODE_OFF;
  } else {
    Logger::debug("Stopping wifi in unknown mode...");
  }
  Logger::debug("Wifi stopped.");
}

void Wifi::init() {
  Logger::info("Initializing wifi...");
  if (!m_non_volatile_storage->isInitialized()) {
    Logger::error("Non volatile storage not initialized!");
  }
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_ap();
  esp_netif_create_default_wifi_sta();

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID,
      [](void* arg, esp_event_base_t event_base, int32_t event_id,
         void* event_data) {
        static_cast<Wifi*>(arg)->wifi_event_handler(arg, event_base, event_id,
                                                    event_data);
      },
      this, &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP,
      [](void* arg, esp_event_base_t event_base, int32_t event_id,
         void* event_data) {
        static_cast<Wifi*>(arg)->wifi_event_handler(arg, event_base, event_id,
                                                    event_data);
      },
      this, &instance_got_ip));
  Logger::info("Finished initializing wifi.");
}

void Wifi::wifi_init_softap() {
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  esp_wifi_disable_pmf_config(WIFI_IF_AP);

  wifi_config_t wifi_config = {
      .ap = {.ssid = DEFAULT_WIFI_AP_SSID,
             .password = DEFAULT_WIFI_AP_PASS,
             .ssid_len = strlen(DEFAULT_WIFI_AP_SSID),
             .authmode = WIFI_AUTH_WPA_WPA2_PSK,
             .max_connection = 5},
  };
  if (strlen(DEFAULT_WIFI_AP_PASS) == 0) {
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  }

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  esp_netif_ip_info_t ip_info;
  esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"),
                        &ip_info);

  Logger::debug("Started Accesspoint.");
  m_current_mode = WIFIMode::MODE_AP;
}

std::string Wifi::get_wifi_ssid() {
  std::string wifi_ssid;
  m_non_volatile_storage->getValue(STORAGE_USERCONFIG, KEY_WIFISSID,
                                   &wifi_ssid);
  return wifi_ssid;
}

std::string Wifi::get_wifi_password() {
  std::string wifi_password;
  m_non_volatile_storage->getValue(STORAGE_USERCONFIG, KEY_WIFIPASSWORD,
                                   &wifi_password);
  return wifi_password;
}

bool Wifi::storeWifiCredentials(const std::string& ssid,
                                const std::string& password) {
  bool all_ok = true;
  all_ok &=
      m_non_volatile_storage->setValue(STORAGE_USERCONFIG, KEY_WIFISSID, ssid);
  all_ok &= m_non_volatile_storage->setValue(STORAGE_USERCONFIG,
                                             KEY_WIFIPASSWORD, password);
  return all_ok;
}