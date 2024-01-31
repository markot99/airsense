/*

Library for the BME680 sensor

Adjusted from original library by Adafruit:
https://github.com/adafruit/Adafruit_BME680

*/

#include "main/driver/bme680/bme680.h"

#include <math.h>

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "main/hal/timer/timer.h"
#include "main/logger/logger.h"

BME680::BME680(I2C *i2c)
    : m_i2c(i2c),
      m_meas_start(0),
      m_meas_period(0),
      m_i2c_addr(0x77),
      m_mutex(xSemaphoreCreateMutex()) {
  init();
}

BME680::~BME680() {}

bool BME680::init() {
  Logger::info("Initializing BME680");
  gas_sensor.chip_id = m_i2c_addr;
  gas_sensor.intf = BME68X_I2C_INTF;
  gas_sensor.intf_ptr = (void *)m_i2c;
  gas_sensor.read = &readI2C;
  gas_sensor.write = &writeI2C;
  gas_sensor.amb_temp = 25;
  gas_sensor.delay_us = delayMicroseconds;

  int8_t rslt = bme68x_init(&gas_sensor);

  if (rslt != BME68X_OK) {
    Logger::error("BME680: Failed to initialize");
    return false;
  }

  // default settings
  setTemperatureOversampling(BME68X_OS_8X);
  setHumidityOversampling(BME68X_OS_2X);
  setPressureOversampling(BME68X_OS_4X);
  setIIRFilterSize(BME68X_FILTER_SIZE_3);
  setGasHeater(320, 150);

  rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &gas_sensor);

  if (rslt != BME68X_OK) {
    Logger::error("BME680: Failed to set operation mode");
    return false;
  }
  Logger::info("Finished initializing BME680");
  return true;
}

float BME680::getTemperature(void) {
  if (xSemaphoreTake(m_mutex, (TickType_t)10) == pdTRUE) {
    float temp = temperature;
    xSemaphoreGive(m_mutex);
    return temp;
  }
  Logger::error("BME680: Failed to take mutex lock for reading temperature");
  return 0;
}

float BME680::getPressure(void) {
  if (xSemaphoreTake(m_mutex, (TickType_t)10) == pdTRUE) {
    float pres = pressure;
    xSemaphoreGive(m_mutex);
    return pres;
  }

  Logger::error("BME680: Failed to take mutex lock for reading pressure");
  return 0;
}

float BME680::getHumidity(void) {
  if (xSemaphoreTake(m_mutex, (TickType_t)10) == pdTRUE) {
    float hum = humidity;
    xSemaphoreGive(m_mutex);
    return hum;
  }

  Logger::error("BME680: Failed to take mutex lock for reading humidity");
  return 0;
}

uint32_t BME680::getGas(void) {
  if (xSemaphoreTake(m_mutex, (TickType_t)10) == pdTRUE) {
    uint32_t gas = gas_resistance;
    xSemaphoreGive(m_mutex);
    return gas;
  }

  Logger::error("BME680: Failed to take mutex lock for reading gas");
  return 0;
}

float BME680::getAltitude(float seaLevel) {
  float atmospheric = getPressure() / 100.0F;
  return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}

bool BME680::readData() { return endReading(); }

uint32_t BME680::beginReading(void) {
  if (m_meas_start != 0) {
    /* A measurement is already in progress */
    return m_meas_start + m_meas_period;
  }

  int8_t rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &gas_sensor);
  if (rslt != BME68X_OK) return false;

  /* Calculate delay period in microseconds */
  uint32_t delayus_period = (uint32_t)bme68x_get_meas_dur(
                                BME68X_FORCED_MODE, &gas_conf, &gas_sensor) +
                            ((uint32_t)gas_heatr_conf.heatr_dur * 1000);

  m_meas_start = esp_timer_get_time();
  m_meas_period = delayus_period / 1000;

  return m_meas_start + m_meas_period;
}

bool BME680::endReading(void) {
  uint32_t meas_end = beginReading();

  if (meas_end == 0) {
    return false;
  }

  int remaining_millis = remainingReadingMillis();

  if (remaining_millis > 0) {
    Timer::sleepMS(remaining_millis * 2);
    /* Delay till the measurement is ready */
  }
  m_meas_start = 0; /* Allow new measurement to begin */
  m_meas_period = 0;

  struct bme68x_data data;
  uint8_t n_fields;

  int8_t rslt =
      bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &gas_sensor);
  if (rslt != BME68X_OK) return false;

  if (n_fields) {
    if (xSemaphoreTake(m_mutex, (TickType_t)10) == pdTRUE) {
      temperature = data.temperature;
      humidity = data.humidity;
      pressure = data.pressure;

      if (data.status & (BME68X_HEAT_STAB_MSK | BME68X_GASM_VALID_MSK)) {
        gas_resistance = data.gas_resistance;
      } else {
        gas_resistance = 0;
      }
      xSemaphoreGive(m_mutex);
    } else {
      Logger::error("BME680: Failed to take mutex lock for setting data");
      return false;
    }
  }

  return true;
}

int BME680::remainingReadingMillis(void) {
  if (m_meas_start != 0) {
    /* A measurement is already in progress */
    int remaining_time =
        (int)m_meas_period - (esp_timer_get_time() - m_meas_start);
    return remaining_time < 0 ? reading_complete : remaining_time;
  }
  return reading_not_started;
}

bool BME680::setGasHeater(uint16_t heaterTemp, uint16_t heaterTime) {
  if ((heaterTemp == 0) || (heaterTime == 0)) {
    gas_heatr_conf.enable = BME68X_DISABLE;
  } else {
    gas_heatr_conf.enable = BME68X_ENABLE;
    gas_heatr_conf.heatr_temp = heaterTemp;
    gas_heatr_conf.heatr_dur = heaterTime;
  }

  int8_t rslt =
      bme68x_set_heatr_conf(BME68X_FORCED_MODE, &gas_heatr_conf, &gas_sensor);
  return rslt == 0;
}

bool BME680::setODR(uint8_t odr) {
  if (odr > BME68X_ODR_NONE) return false;

  gas_conf.odr = odr;

  int8_t rslt = bme68x_set_conf(&gas_conf, &gas_sensor);
  return rslt == 0;
}

bool BME680::setTemperatureOversampling(uint8_t oversample) {
  if (oversample > BME68X_OS_16X) return false;

  gas_conf.os_temp = oversample;

  int8_t rslt = bme68x_set_conf(&gas_conf, &gas_sensor);
  return rslt == 0;
}

bool BME680::setHumidityOversampling(uint8_t oversample) {
  if (oversample > BME68X_OS_16X) return false;

  gas_conf.os_hum = oversample;

  int8_t rslt = bme68x_set_conf(&gas_conf, &gas_sensor);
  return rslt == 0;
}

bool BME680::setPressureOversampling(uint8_t oversample) {
  if (oversample > BME68X_OS_16X) return false;

  gas_conf.os_pres = oversample;

  int8_t rslt = bme68x_set_conf(&gas_conf, &gas_sensor);
  return rslt == 0;
}

bool BME680::setIIRFilterSize(uint8_t filtersize) {
  if (filtersize > BME68X_FILTER_SIZE_127) return false;
  gas_conf.filter = filtersize;

  int8_t rslt = bme68x_set_conf(&gas_conf, &gas_sensor);
  return rslt == 0;
}

int8_t BME680::readI2C(uint8_t reg_addr, uint8_t *reg_data, uint32_t len,
                       void *intf_ptr) {
  I2C *m_i2c = static_cast<I2C *>(intf_ptr);

  m_i2c->read(0x77, reg_addr, reg_data, len);

  return 0;
}

int8_t BME680::writeI2C(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len,
                        void *intf_ptr) {
  I2C *m_i2c = static_cast<I2C *>(intf_ptr);
  m_i2c->write(0x77, reg_addr, reg_data, len);
  return 0;
}

void BME680::delayMicroseconds(uint32_t us, void *intf_ptr) {
  // I2C *m_i2c = static_cast<I2C *>(intf_ptr);
  auto t_ms = us / 1000;
  Timer::sleepMS(t_ms);
}