#pragma once

#include <vector>

#include "driver/uart.h"

//! @brief Uart class to send and receive data over UART.
class Uart {
 public:
  //! @brief Constructor
  //! @param rx_pin The pin to use for RX.
  //! @param tx_pin The pin to use for TX.
  //! @param baud_rate The baud rate to use.
  Uart(int rx_pin, int tx_pin, int baud_rate);

  //! @brief Destructor
  ~Uart();

  //! @brief Initialize the UART.
  void init();

  //! @brief Send a string to the UART.
  //! @param data The data to send.
  void sendData(std::vector<uint8_t> data);

  //! @brief Read data from the UART.
  //! @param data The vector to store the data in.
  //! @param len The length of the data to read.
  void readData(std::vector<uint8_t>& data, int32_t len);

  //! @brief Set the baud rate of the UART.
  //! @param baud_rate The baud rate to set.
  void setBaudRate(uint32_t baud_rate);

  //! @brief Flush the UART.
  void flush();

 private:
  //! @brief The UART configuration.
  const uart_config_t m_conf;

  //! @brief The pin to use for RX.
  const int m_rx_pin;

  //! @brief The pin to use for TX.
  const int m_tx_pin;
};
