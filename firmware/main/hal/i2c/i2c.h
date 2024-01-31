#pragma once

#include "driver/i2c.h"

class I2C {
 public:
  //! @brief Constructor
  //! @param sda_pin The pin to use for SDA.
  //! @param scl_pin The pin to use for SCL.
  //! @param master_timeout_ms The timeout for the master.
  I2C(int sda_pin, int scl_pin, int master_timeout_ms);

  //! @brief Destructor
  ~I2C();

  //! @brief Initialize the I2C.
  void init();

  //! @brief Write data to the I2C.
  //! @param device_addr The address of the device to write to.
  //! @param reg_addr The address of the register to write to.
  //! @param data The data to write.
  //! @param len The length of the data to write.
  void write(uint8_t device_addr, uint8_t reg_addr, const uint8_t *data,
             size_t len);

  //! @brief Write one byte to the I2C.
  //! @param device_addr The address of the device to write to.
  //! @param reg_addr The address of the register to write to.
  //! @param data The byte to write.
  void write(uint8_t device_addr, uint8_t reg_addr, uint8_t data);

  //! @brief Read data from the I2C.
  //! @param device_addr The address of the device to read from.
  //! @param reg_addr The address of the register to read from.
  //! @param data The pointer to store the data in.
  //! @param len The length of the data to read.
  void read(uint8_t device_addr, uint8_t reg_addr, uint8_t *data, size_t len);

  //! @brief Read one byte from the I2C.
  //! @param device_addr The address of the device to read from.
  //! @param reg_addr The address of the register to read from.
  //! @param data The pointer to store the data in.
  void read(uint8_t device_addr, uint8_t reg_addr, uint8_t *data);

  //! @brief Read two bytes from the I2C in little endian.
  //! @param device_addr The address of the device to read from.
  //! @param reg_addr The address of the register to read from.
  //! @param data The pointer to store the data in.
  void readLittleEndian(uint8_t device_addr, uint8_t reg_addr, uint16_t *data);

  //! @brief Read two bytes from the I2C in big endian.
  //! @param device_addr The address of the device to read from.
  //! @param reg_addr The address of the register to read from.
  //! @param data The pointer to store the data in.
  void readBigEndian(uint8_t device_addr, uint8_t reg_addr, uint16_t *data);

  //! @brief Read four bytes from the I2C in little endian.
  //! @param device_addr The address of the device to read from.
  //! @param reg_addr The address of the register to read from.
  //! @param data The pointer to store the data in.
  void readLittleEndian(uint8_t device_addr, uint8_t reg_addr, uint32_t *data);

  //! @brief Read four bytes from the I2C in big endian.
  //! @param device_addr The address of the device to read from.
  //! @param reg_addr The address of the register to read from.
  void readBigEndian(uint8_t device_addr, uint8_t reg_addr, uint32_t *data);

 private:
  //! @brief The I2C configuration.
  const i2c_config_t m_conf;

  //! @brief The pin to use for SDA.
  const int m_sda_pin;

  //! @brief The pin to use for SCL.
  const int m_scl_pin;

  //! @brief The timeout in ms for the i2c master.
  int m_master_timeout_ms;
};
