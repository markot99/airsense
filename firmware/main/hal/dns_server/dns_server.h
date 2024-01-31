/* Captive Portal Example

    This example code is in the Public Domain (or CC0 licensed, at your option.)

    Unless required by applicable law or agreed to in writing, this
    software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
    CONDITIONS OF ANY KIND, either express or implied.

    From: https://github.com/espressif/esp-idf/tree/master/examples/protocols/http_server/captive_portal
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set ups and starts a DNS server that will respond to all
 * queries with the soft AP's IP address
 *
 */
void start_dns_server();

/**
 * @brief Stops the DNS server
 *
 */
void stop_dns_server();

#ifdef __cplusplus
}
#endif
