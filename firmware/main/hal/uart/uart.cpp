#include "main/hal/uart/uart.h"

#include "driver/uart.h"
#include "esp_timer.h"
#include "main/logger/logger.h"

static const int RX_BUF_SIZE = 1024;

Uart::Uart(int rx_pin, int tx_pin, int baud_rate)
    : m_conf({
          .baud_rate = baud_rate,
          .data_bits = UART_DATA_8_BITS,
          .parity = UART_PARITY_DISABLE,
          .stop_bits = UART_STOP_BITS_1,
          .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
          .rx_flow_ctrl_thresh = 0,
          .source_clk = UART_SCLK_DEFAULT,
      }),
      m_rx_pin(rx_pin),
      m_tx_pin(tx_pin) {
  init();
}

Uart::~Uart() {}

void Uart::init() {
  Logger::info("Initializing UART.");
  uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
  uart_param_config(UART_NUM_1, &m_conf);
  uart_set_pin(UART_NUM_1, m_tx_pin, m_rx_pin, UART_PIN_NO_CHANGE,
               UART_PIN_NO_CHANGE);
  Logger::info("Finished initializing UART.");
}

void Uart::sendData(std::vector<uint8_t> data) {
  uart_write_bytes(UART_NUM_1, data.data(), data.size());
}

void Uart::readData(std::vector<uint8_t>& data, int32_t len) {
  data.clear();
  uint8_t* dataPtr = (uint8_t*)malloc(len + 1);
  while (data.size() < len) {
    const int rxBytes = uart_read_bytes(UART_NUM_1, dataPtr, RX_BUF_SIZE,
                                        1000 / portTICK_PERIOD_MS);
    if (rxBytes > 0) {
      dataPtr[rxBytes] = 0;
      Logger::info("Received " + std::to_string(rxBytes) + " bytes from UART.");
      // data integers to chars
      for (int i = 0; i < rxBytes; i++) {
        data.push_back(dataPtr[i]);
      }
    }
  }
  free(dataPtr);
}

void Uart::setBaudRate(uint32_t baud_rate) {
  esp_err_t err = uart_set_baudrate(UART_NUM_1, baud_rate);
  if (err == ESP_OK) {
    Logger::info("Successfully set baud rate to " + std::to_string(baud_rate));
  } else {
    Logger::error("Failed to set baud rate to " + std::to_string(baud_rate));
  }
}

void Uart::flush() { uart_flush(UART_NUM_1); }