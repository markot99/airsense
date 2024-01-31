/*

Library for the BME680 sensor

Adjusted from original library by Adafruit:
https://github.com/adafruit/Adafruit_BME680

*/

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "main/driver/bme680/libs/bme68x.h"
#include "main/hal/i2c/i2c.h"

#define BME68X_DEFAULT_ADDRESS (0x77)

class BME680 {
 public:
  //! Constructor
  BME680(I2C *i2c);

  //! Destructor
  ~BME680();

  //! Flag to indicate that the sensor reading has been started
  static constexpr int reading_not_started = -1;

  //! Flag to indicate that the sensor reading is completed
  static constexpr int reading_complete = 0;

  //! @brief Get the last temperature reading
  //! @return Temperature in degrees C
  float getTemperature();

  //! @brief Get the last pressure reading
  //! @return Pressure in Pascals
  float getPressure();

  //! @brief Get the last humidity reading
  //! @return Humidity in RH %
  float getHumidity();

  //! @brief Get the last volatile organic compound reading
  //! @return VOC in ohms
  uint32_t getGas();

  //! @brief Get the last altitude reading
  //! @param seaLevel Sea level pressure in Pascals
  //! @return Altitude in meters
  float getAltitude(float seaLevel);

  //! @brief Perform a reading and store the results in class variables
  //! @return True if successful, false if there was an error
  bool readData();

 private:
  //! @brief Initialize the sensor
  bool init();

  //! @brief set the temperature oversampling
  //! @param os Oversampling setting
  //! @return True if successful, false if there was an error
  bool setTemperatureOversampling(uint8_t os);

  //! @brief set the pressure oversampling
  //! @param os Oversampling setting
  //! @return True if successful, false if there was an error
  bool setPressureOversampling(uint8_t os);

  //! @brief set the humidity oversampling
  //! @param os Oversampling setting
  //! @return True if successful, false if there was an error
  bool setHumidityOversampling(uint8_t os);

  //! @brief set the IIR filter size
  //! @param fs Filter size
  //! @return True if successful, false if there was an error
  bool setIIRFilterSize(uint8_t fs);

  //! @brief set the gas measurement interval
  //! @param interval Interval in milliseconds
  //! @return True if successful, false if there was an error
  bool setGasHeater(uint16_t heaterTemp, uint16_t heaterTime);

  //! @brief set the gas measurement interval
  //! @param interval Interval in milliseconds
  //! @return True if successful, false if there was an error
  bool setODR(uint8_t odr);

  //! @brief begin reading data from the sensor
  //! @return milliseconds to wait until the data is ready
  uint32_t beginReading();

  //! @brief end reading data from the sensor
  //! @return True if successful, false if there was an error
  bool endReading();

  //! @brief get the remaining time in milliseconds until the measurement is
  //! complete
  //! @return milliseconds to wait until the data is ready
  int remainingReadingMillis();

  //! Pointer to the I2C hal
  I2C *m_i2c;

  //! Sensor ID
  int32_t _sensorID;

  //! Timestamp of the start of the measurement
  uint32_t m_meas_start;

  //! Measurement period in milliseconds
  uint16_t m_meas_period;

  //! I2C address
  uint8_t m_i2c_addr;

  //! Sensor configuration
  struct bme68x_dev gas_sensor;

  //! Sensor configuration
  struct bme68x_conf gas_conf;

  //! Heater configuration
  struct bme68x_heatr_conf gas_heatr_conf;

  //! Function to read from the I2C bus
  //! @note function is passed to the BME68X library
  //! @param reg_addr Register address
  //! @param reg_data Pointer to the data to be read
  //! @param len Number of bytes to read
  //! @param intf_ptr Pointer to the I2C interface
  //! @return 0 if successful, non-zero otherwise
  static int8_t readI2C(uint8_t reg_addr, uint8_t *reg_data, uint32_t len,
                        void *interface);

  //! Function to write to the I2C bus
  //! @note function is passed to the BME68X library
  //! @param reg_addr Register address
  //! @param reg_data Pointer to the data to be written
  //! @param len Number of bytes to write
  //! @param intf_ptr Pointer to the I2C interface
  //! @return 0 if successful, non-zero otherwise
  static int8_t writeI2C(uint8_t reg_addr, const uint8_t *reg_data,
                         uint32_t len, void *interface);

  //! Function to delay execution
  //! @note function is passed to the BME68X library
  //! @param period Time in microseconds to delay
  //! @param intf_ptr Pointer to the I2C interface
  //! @return 0 if successful, non-zero otherwise
  static void delayMicroseconds(uint32_t us, void *intf_ptr);

  //! cached temperature in degrees C
  float temperature;

  //! cached pressure in Pascals
  uint32_t pressure;

  //! cached humidity in RH %
  float humidity;

  //! cached gas in ohms
  uint32_t gas_resistance;

  // mutex
  SemaphoreHandle_t m_mutex;
};