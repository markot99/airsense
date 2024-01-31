/*

Library for the APDS9960 digital proximity, ambient light, RGB and gesture

Adjusted from original library by Adafruit:
https://github.com/adafruit/Adafruit_APDS9960

*/

#include "main/driver/apds9960/apds9960.h"

#include <math.h>

#include <string>

#include "esp_timer.h"
#include "main/hal/timer/timer.h"
#include "main/logger/logger.h"

APDS9960::APDS9960(I2C *i2c) : m_i2c(i2c), m_is_connected(false) { init(); }

APDS9960::~APDS9960() {}

float powf(const float x, const float y) {
  return (float)(pow((double)x, (double)y));
}

void APDS9960::init(uint16_t iTimeMS, apds9960AGain_t aGain, uint8_t addr) {
  Logger::info("Initializing APDS9960");
  // check if apds9960 is available by reading the ID register
  uint8_t id = 0;
  m_i2c->read(APDS9960_ADDRESS, APDS9960_ID, &id);
  if (id != 0xAB) {
    Logger::error("APDS9960 ID not found: " + std::to_string(id));
    return;
  } else {
    Logger::info("APDS9960 found.");
    m_is_connected = true;
  }

  setADCIntegrationTime(iTimeMS);
  setADCGain(aGain);

  // disable everything to start
  enableGesture(false);
  enableProximity(false);
  enableColor(false);

  disableColorInterrupt();
  disableProximityInterrupt();
  clearInterrupt();

  /* Note: by default, the device is in power down mode on bootup */
  enable(false);
  Timer::sleepMS(10);
  enable(true);
  Timer::sleepMS(10);

  // default to all gesture dimensions
  setGestureDimensions(APDS9960_DIMENSIONS_ALL);
  setGestureFIFOThreshold(APDS9960_GFIFO_4);
  setGestureGain(APDS9960_GGAIN_4);
  setGestureProximityThreshold(50);
  resetCounts();

  _gpulse.GPLEN = APDS9960_GPULSE_32US;
  _gpulse.GPULSE = 9;  // 10 pulses
  m_i2c->write(APDS9960_ADDRESS, APDS9960_GPULSE, _gpulse.get());

  enableProximity(true);
  enableGesture(true);
  Logger::info("Finished initializing APDS9960");
}

bool APDS9960::isConnected() { return m_is_connected; }

void APDS9960::enable(bool enable) {
  _enable.PON = enable;
  m_i2c->write(APDS9960_ADDRESS, APDS9960_ENABLE, _enable.get());
}

void APDS9960::setADCIntegrationTime(uint16_t milliseconds) {
  float temp;

  // convert ms into 2.78ms increments
  temp = milliseconds;
  temp /= 2.78;
  temp = 256 - temp;
  if (temp > 255) temp = 255;
  if (temp < 0) temp = 0;

  /* Update the timing register */
  m_i2c->write(APDS9960_ADDRESS, APDS9960_ATIME, (uint8_t)temp);
}

float APDS9960::getADCIntegrationTime() {
  float temp;

  m_i2c->read(APDS9960_ADDRESS, APDS9960_ATIME, (uint8_t *)&temp);

  // convert to units of 2.78 ms
  temp = 256 - temp;
  temp *= 2.78;
  return temp;
}

void APDS9960::setADCGain(apds9960AGain_t aGain) {
  _control.AGAIN = aGain;

  /* Update the timing register */
  m_i2c->write(APDS9960_ADDRESS, APDS9960_CONTROL, _control.get());
}

apds9960AGain_t APDS9960::getADCGain() {
  uint8_t ret;
  m_i2c->read(APDS9960_ADDRESS, APDS9960_CONTROL, &ret);
  return (apds9960AGain_t)(ret & 0x03);
}

void APDS9960::setProximityGain(apds9960PGain_t pGain) {
  _control.PGAIN = pGain;

  /* Update the timing register */
  m_i2c->write(APDS9960_ADDRESS, APDS9960_CONTROL, _control.get());
}

apds9960PGain_t APDS9960::getProximityGain() {
  uint8_t ret;
  m_i2c->read(APDS9960_ADDRESS, APDS9960_CONTROL, &ret);
  return (apds9960PGain_t)((ret & 0x0C) >> 2);
}

void APDS9960::setProxPulse(apds9960PPulseLen_t pLen, uint8_t pulses) {
  if (pulses < 1) pulses = 1;
  if (pulses > 64) pulses = 64;
  pulses--;

  _ppulse.PPLEN = pLen;
  _ppulse.PPULSE = pulses;

  m_i2c->write(APDS9960_ADDRESS, APDS9960_PPULSE, _ppulse.get());
}

void APDS9960::enableProximity(bool enable) {
  _enable.PEN = enable;

  m_i2c->write(APDS9960_ADDRESS, APDS9960_ENABLE, _enable.get());
}

void APDS9960::enableProximityInterrupt() {
  _enable.PIEN = 1;
  m_i2c->write(APDS9960_ADDRESS, APDS9960_ENABLE, _enable.get());
  clearInterrupt();
}

void APDS9960::disableProximityInterrupt() {
  _enable.PIEN = 0;
  m_i2c->write(APDS9960_ADDRESS, APDS9960_ENABLE, _enable.get());
}

void APDS9960::setProximityInterruptThreshold(uint8_t low, uint8_t high,
                                              uint8_t persistence) {
  m_i2c->write(APDS9960_ADDRESS, APDS9960_PILT, low);
  m_i2c->write(APDS9960_ADDRESS, APDS9960_PIHT, high);

  if (persistence > 7) persistence = 7;
  _pers.PPERS = persistence;

  m_i2c->write(APDS9960_ADDRESS, APDS9960_PERS, _pers.get());
}

bool APDS9960::getProximityInterrupt() {
  uint8_t ret;
  m_i2c->read(APDS9960_ADDRESS, APDS9960_STATUS, &ret);
  _status.set(ret);
  return _status.PINT;
};

uint8_t APDS9960::readProximity() {
  uint8_t ret;
  m_i2c->read(APDS9960_ADDRESS, APDS9960_PDATA, &ret);
  return ret;
}

bool APDS9960::gestureValid() {
  uint8_t ret;
  m_i2c->read(APDS9960_ADDRESS, APDS9960_GSTATUS, &ret);
  _gstatus.set(ret);
  return _gstatus.GVALID;
}

void APDS9960::setGestureDimensions(uint8_t dimensions) {
  _gconf3.GDIMS = dimensions;
  m_i2c->write(APDS9960_ADDRESS, APDS9960_GCONF3, _gconf3.get());
}

void APDS9960::setGestureFIFOThreshold(uint8_t thresh) {
  _gconf1.GFIFOTH = thresh;
  m_i2c->write(APDS9960_ADDRESS, APDS9960_GCONF1, _gconf1.get());
}

void APDS9960::setGestureGain(uint8_t gain) {
  _gconf2.GGAIN = gain;
  m_i2c->write(APDS9960_ADDRESS, APDS9960_GCONF2, _gconf2.get());
}

void APDS9960::setGestureProximityThreshold(uint8_t threshold) {
  m_i2c->write(APDS9960_ADDRESS, APDS9960_GPENTH, threshold);
}

void APDS9960::setGestureOffset(uint8_t offset_up, uint8_t offset_down,
                                uint8_t offset_left, uint8_t offset_right) {
  m_i2c->write(APDS9960_ADDRESS, APDS9960_GOFFSET_U, offset_up);
  m_i2c->write(APDS9960_ADDRESS, APDS9960_GOFFSET_D, offset_down);
  m_i2c->write(APDS9960_ADDRESS, APDS9960_GOFFSET_L, offset_left);
  m_i2c->write(APDS9960_ADDRESS, APDS9960_GOFFSET_R, offset_right);
}

void APDS9960::enableGesture(bool enable) {
  if (!enable) {
    _gconf4.GMODE = 0;
    m_i2c->write(APDS9960_ADDRESS, APDS9960_GCONF4, _gconf4.get());
  }
  _enable.GEN = enable;
  m_i2c->write(APDS9960_ADDRESS, APDS9960_ENABLE, _enable.get());
  resetCounts();
}

void APDS9960::resetCounts() {
  gestCnt = 0;
  UCount = 0;
  DCount = 0;
  LCount = 0;
  RCount = 0;
}

uint8_t APDS9960::readGesture() {
  uint8_t toRead;
  uint8_t buf[256];
  unsigned long t = 0;
  uint8_t gestureReceived;
  while (1) {
    int up_down_diff = 0;
    int left_right_diff = 0;
    gestureReceived = 0;
    if (!gestureValid()) {
      return 0;
    }
    Timer::sleepMS(30);

    m_i2c->read(APDS9960_ADDRESS, APDS9960_GFLVL, &toRead);

    // produces sideffects needed for readGesture to work
    m_i2c->read(APDS9960_ADDRESS, APDS9960_GFIFO_U, buf, toRead);

    if (abs((int)buf[0] - (int)buf[1]) > 13)
      up_down_diff += (int)buf[0] - (int)buf[1];

    if (abs((int)buf[2] - (int)buf[3]) > 13)
      left_right_diff += (int)buf[2] - (int)buf[3];

    if (up_down_diff != 0) {
      if (up_down_diff < 0) {
        if (DCount > 0) {
          gestureReceived = APDS9960_UP;
        } else {
          UCount++;
        }

      } else if (up_down_diff > 0) {
        if (UCount > 0) {
          gestureReceived = APDS9960_DOWN;
        } else {
          DCount++;
        }
      }
    }

    if (left_right_diff != 0) {
      if (left_right_diff < 0) {
        if (RCount > 0) {
          gestureReceived = APDS9960_LEFT;
        } else {
          LCount++;
        }
      } else if (left_right_diff > 0) {
        if (LCount > 0) {
          gestureReceived = APDS9960_RIGHT;
        } else {
          RCount++;
        }
      }
    }

    if (up_down_diff != 0 || left_right_diff != 0) t = xTaskGetTickCount();

    if (gestureReceived || xTaskGetTickCount() - t > 300) {
      resetCounts();
      return gestureReceived;
    }
  }
}

void APDS9960::setLED(apds9960LedDrive_t drive, apds9960LedBoost_t boost) {
  // set BOOST
  _config2.LED_BOOST = boost;
  m_i2c->write(APDS9960_ADDRESS, APDS9960_CONFIG2, _config2.get());

  _control.LDRIVE = drive;
  m_i2c->write(APDS9960_ADDRESS, APDS9960_CONTROL, _control.get());
}

void APDS9960::enableColor(bool enable) {
  _enable.AEN = enable;
  m_i2c->write(APDS9960_ADDRESS, APDS9960_ENABLE, _enable.get());
}

bool APDS9960::colorDataReady() {
  uint8_t ret;
  m_i2c->read(APDS9960_ADDRESS, APDS9960_STATUS, &ret);
  _status.set(ret);
  return _status.AVALID;
}

void APDS9960::getColorData(uint16_t *r, uint16_t *g, uint16_t *b,
                            uint16_t *c) {
  m_i2c->readLittleEndian(APDS9960_ADDRESS, APDS9960_CDATAL, c);
  m_i2c->readLittleEndian(APDS9960_ADDRESS, APDS9960_RDATAL, r);
  m_i2c->readLittleEndian(APDS9960_ADDRESS, APDS9960_GDATAL, g);
  m_i2c->readLittleEndian(APDS9960_ADDRESS, APDS9960_BDATAL, b);
}

uint16_t APDS9960::calculateColorTemperature(uint16_t r, uint16_t g,
                                             uint16_t b) {
  float X, Y, Z; /* RGB to XYZ correlation      */
  float xc, yc;  /* Chromaticity co-ordinates   */
  float n;       /* McCamy's formula            */
  float cct;

  /* 1. Map RGB values to their XYZ counterparts.    */
  /* Based on 6500K fluorescent, 3000K fluorescent   */
  /* and 60W incandescent values for a wide range.   */
  /* Note: Y = Illuminance or lux                    */
  X = (-0.14282F * r) + (1.54924F * g) + (-0.95641F * b);
  Y = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
  Z = (-0.68202F * r) + (0.77073F * g) + (0.56332F * b);

  /* 2. Calculate the chromaticity co-ordinates      */
  xc = (X) / (X + Y + Z);
  yc = (Y) / (X + Y + Z);

  /* 3. Use McCamy's formula to determine the CCT    */
  n = (xc - 0.3320F) / (0.1858F - yc);

  /* Calculate the final CCT */
  cct =
      (449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) + 5520.33F;

  /* Return the results in degrees Kelvin */
  return (uint16_t)cct;
}

uint16_t APDS9960::calculateLux(uint16_t r, uint16_t g, uint16_t b) {
  float illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
  return (uint16_t)illuminance;
}

void APDS9960::enableColorInterrupt() {
  _enable.AIEN = 1;
  m_i2c->write(APDS9960_ADDRESS, APDS9960_ENABLE, _enable.get());
}

void APDS9960::disableColorInterrupt() {
  _enable.AIEN = 0;
  m_i2c->write(APDS9960_ADDRESS, APDS9960_ENABLE, _enable.get());
}

void APDS9960::clearInterrupt() {
  m_i2c->write(APDS9960_ADDRESS, APDS9960_AICLEAR, NULL, 0);
}

void APDS9960::setIntLimits(uint16_t low, uint16_t high) {
  m_i2c->write(APDS9960_ADDRESS, APDS9960_AILTIL, (uint8_t)(low & 0xFF));
  m_i2c->write(APDS9960_ADDRESS, APDS9960_AILTH, (uint8_t)(low >> 8));
  m_i2c->write(APDS9960_ADDRESS, APDS9960_AIHTL, (uint8_t)(high & 0xFF));
  m_i2c->write(APDS9960_ADDRESS, APDS9960_AIHTH, (uint8_t)(high >> 8));
}
