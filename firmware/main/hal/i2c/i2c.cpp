#include "main/hal/i2c/i2c.h"

#include <cstring>
#include <string>

#include "freertos/FreeRTOS.h"
#include "main/logger/logger.h"

I2C::I2C(int sda_pin, int scl_pin, int master_timeout_ms)
    : m_conf({.mode = I2C_MODE_MASTER,
              .sda_io_num = sda_pin,
              .scl_io_num = scl_pin,
              .sda_pullup_en = GPIO_PULLUP_ENABLE,
              .scl_pullup_en = GPIO_PULLUP_ENABLE,
              .master = {.clk_speed = 100000},
              .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL}),
      m_sda_pin(sda_pin),
      m_scl_pin(scl_pin),
      m_master_timeout_ms(master_timeout_ms) {
  init();
}

I2C::~I2C() {}

void I2C::init() {
  Logger::info("Initializing I2C.");
  i2c_param_config(I2C_NUM_0, &m_conf);
  i2c_driver_install(I2C_NUM_0, m_conf.mode, 0, 0, 0);
  Logger::info("Finished initializing I2C.");
}

void I2C::read(uint8_t device_addr, uint8_t reg_addr, uint8_t *data,
               size_t len) {
  i2c_master_write_read_device(I2C_NUM_0, device_addr, &reg_addr, 1, data, len,
                               m_master_timeout_ms / portTICK_PERIOD_MS);
}

void I2C::read(uint8_t device_addr, uint8_t reg_addr, uint8_t *data) {
  read(device_addr, reg_addr, data, 1);
}

void I2C::readLittleEndian(uint8_t device_addr, uint8_t reg_addr,
                           uint16_t *data) {
  uint8_t buf[2];
  read(device_addr, reg_addr, buf, 2);
  *data = buf[0] | (buf[1] << 8);
}

void I2C::readBigEndian(uint8_t device_addr, uint8_t reg_addr, uint16_t *data) {
  uint8_t buf[2];
  read(device_addr, reg_addr, buf, 2);
  *data = buf[1] | (buf[0] << 8);
}

void I2C::readLittleEndian(uint8_t device_addr, uint8_t reg_addr,
                           uint32_t *data) {
  uint8_t buf[4];
  read(device_addr, reg_addr, buf, 4);
  *data = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
}

void I2C::readBigEndian(uint8_t device_addr, uint8_t reg_addr, uint32_t *data) {
  uint8_t buf[4];
  read(device_addr, reg_addr, buf, 4);
  *data = buf[3] | (buf[2] << 8) | (buf[1] << 16) | (buf[0] << 24);
}

void I2C::write(uint8_t device_addr, uint8_t reg_addr, const uint8_t *data,
                size_t len) {
  uint8_t write_buf[len + 1];
  write_buf[0] = reg_addr;
  memcpy(write_buf + 1, data, len);

  i2c_master_write_to_device(I2C_NUM_0, device_addr, write_buf,
                             sizeof(write_buf),
                             m_master_timeout_ms / portTICK_PERIOD_MS);
}

void I2C::write(uint8_t device_addr, uint8_t reg_addr, uint8_t data) {
  write(device_addr, reg_addr, &data, 1);
}
