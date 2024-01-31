/*

Library for the APDS9960 digital proximity, ambient light, RGB and gesture

Adjusted from original library by Adafruit:
https://github.com/adafruit/Adafruit_APDS9960

*/

#pragma once

#include <stdint.h>

#include "main/hal/i2c/i2c.h"

#define APDS9960_ADDRESS (0x39)

//! @brief APDS9960 register addresses
enum {
  APDS9960_RAM = 0x00,
  APDS9960_ENABLE = 0x80,
  APDS9960_ATIME = 0x81,
  APDS9960_WTIME = 0x83,
  APDS9960_AILTIL = 0x84,
  APDS9960_AILTH = 0x85,
  APDS9960_AIHTL = 0x86,
  APDS9960_AIHTH = 0x87,
  APDS9960_PILT = 0x89,
  APDS9960_PIHT = 0x8B,
  APDS9960_PERS = 0x8C,
  APDS9960_CONFIG1 = 0x8D,
  APDS9960_PPULSE = 0x8E,
  APDS9960_CONTROL = 0x8F,
  APDS9960_CONFIG2 = 0x90,
  APDS9960_ID = 0x92,
  APDS9960_STATUS = 0x93,
  APDS9960_CDATAL = 0x94,
  APDS9960_CDATAH = 0x95,
  APDS9960_RDATAL = 0x96,
  APDS9960_RDATAH = 0x97,
  APDS9960_GDATAL = 0x98,
  APDS9960_GDATAH = 0x99,
  APDS9960_BDATAL = 0x9A,
  APDS9960_BDATAH = 0x9B,
  APDS9960_PDATA = 0x9C,
  APDS9960_POFFSET_UR = 0x9D,
  APDS9960_POFFSET_DL = 0x9E,
  APDS9960_CONFIG3 = 0x9F,
  APDS9960_GPENTH = 0xA0,
  APDS9960_GEXTH = 0xA1,
  APDS9960_GCONF1 = 0xA2,
  APDS9960_GCONF2 = 0xA3,
  APDS9960_GOFFSET_U = 0xA4,
  APDS9960_GOFFSET_D = 0xA5,
  APDS9960_GOFFSET_L = 0xA7,
  APDS9960_GOFFSET_R = 0xA9,
  APDS9960_GPULSE = 0xA6,
  APDS9960_GCONF3 = 0xAA,
  APDS9960_GCONF4 = 0xAB,
  APDS9960_GFLVL = 0xAE,
  APDS9960_GSTATUS = 0xAF,
  APDS9960_IFORCE = 0xE4,
  APDS9960_PICLEAR = 0xE5,
  APDS9960_CICLEAR = 0xE6,
  APDS9960_AICLEAR = 0xE7,
  APDS9960_GFIFO_U = 0xFC,
  APDS9960_GFIFO_D = 0xFD,
  APDS9960_GFIFO_L = 0xFE,
  APDS9960_GFIFO_R = 0xFF,
};

//! @brief APDS9906 ADC gain settings
typedef enum {
  APDS9960_AGAIN_1X = 0x00,  /**< No gain */
  APDS9960_AGAIN_4X = 0x01,  /**< 2x gain */
  APDS9960_AGAIN_16X = 0x02, /**< 16x gain */
  APDS9960_AGAIN_64X = 0x03  /**< 64x gain */
} apds9960AGain_t;

//! @brief APDS9906 proximity gain settings
typedef enum {
  APDS9960_PGAIN_1X = 0x00, /**< 1x gain */
  APDS9960_PGAIN_2X = 0x01, /**< 2x gain */
  APDS9960_PGAIN_4X = 0x02, /**< 4x gain */
  APDS9960_PGAIN_8X = 0x03  /**< 8x gain */
} apds9960PGain_t;

//! @brief APDS9906 pulse length settings
typedef enum {
  APDS9960_PPULSELEN_4US = 0x00,  /**< 4uS */
  APDS9960_PPULSELEN_8US = 0x01,  /**< 8uS */
  APDS9960_PPULSELEN_16US = 0x02, /**< 16uS */
  APDS9960_PPULSELEN_32US = 0x03  /**< 32uS */
} apds9960PPulseLen_t;

//! @brief APDS9906 LED drive settings
typedef enum {
  APDS9960_LEDDRIVE_100MA = 0x00, /**< 100mA */
  APDS9960_LEDDRIVE_50MA = 0x01,  /**< 50mA */
  APDS9960_LEDDRIVE_25MA = 0x02,  /**< 25mA */
  APDS9960_LEDDRIVE_12MA = 0x03   /**< 12.5mA */
} apds9960LedDrive_t;

//! @brief APDS9906 LED boost settings
typedef enum {
  APDS9960_LEDBOOST_100PCNT = 0x00, /**< 100% */
  APDS9960_LEDBOOST_150PCNT = 0x01, /**< 150% */
  APDS9960_LEDBOOST_200PCNT = 0x02, /**< 200% */
  APDS9960_LEDBOOST_300PCNT = 0x03  /**< 300% */
} apds9960LedBoost_t;

//! @brief APDS9906 gesture gain settings
enum {
  APDS9960_DIMENSIONS_ALL = 0x00,         // All dimensions
  APDS9960_DIMENSIONS_UP_DOWN = 0x01,     // Up/Down dimensions
  APGS9960_DIMENSIONS_LEFT_RIGHT = 0x02,  // Left/Right dimensions
};

//! @brief APDS9906 gesture FIFO threshold settings
enum {
  APDS9960_GFIFO_1 = 0x00,   // Generate interrupt after 1 dataset in FIFO
  APDS9960_GFIFO_4 = 0x01,   // Generate interrupt after 2 datasets in FIFO
  APDS9960_GFIFO_8 = 0x02,   // Generate interrupt after 3 datasets in FIFO
  APDS9960_GFIFO_16 = 0x03,  // Generate interrupt after 4 datasets in FIFO
};

//! @brief APDS9906 gesture gain settings
enum {
  APDS9960_GGAIN_1 = 0x00,  // Gain 1x
  APDS9960_GGAIN_2 = 0x01,  // Gain 2x
  APDS9960_GGAIN_4 = 0x02,  // Gain 4x
  APDS9960_GGAIN_8 = 0x03,  // Gain 8x
};

//! @brief APDS9906 gesture LED drive settings
enum {
  APDS9960_GPULSE_4US = 0x00,   // Pulse 4us
  APDS9960_GPULSE_8US = 0x01,   // Pulse 8us
  APDS9960_GPULSE_16US = 0x02,  // Pulse 16us
  APDS9960_GPULSE_32US = 0x03,  // Pulse 32us
};

#define APDS9960_UP 0x01     // Gesture Up
#define APDS9960_DOWN 0x02   // Gesture Down
#define APDS9960_LEFT 0x03   // Gesture Left
#define APDS9960_RIGHT 0x04  // Gesture Right

class APDS9960 {
 public:
  //! @brief Constructor
  //! @param[in] i2c Pointer to the I2C hal
  APDS9960(I2C *i2c);

  //! @brief Destructor
  ~APDS9960();

  //! @brief Check if the APDS9960 is connected.
  //! @return True if the APDS9960 is connected, false otherwise.
  bool isConnected();

  //! @brief Initialize the APDS9960.
  //! @param[in] iTimeMS The integration time in milliseconds.
  //! @param[in] gain The gain.
  //! @param[in] addr The address of the APDS9960.
  void init(uint16_t iTimeMS = 10, apds9960AGain_t = APDS9960_AGAIN_4X,
            uint8_t addr = APDS9960_ADDRESS);

  //! @brief Set the integration time of the analog to digital converter of the
  //! APDS9960.
  //! @param[in] milliseconds The integration time in milliseconds.
  void setADCIntegrationTime(uint16_t milliseconds);

  //! @brief Get the integration time of the analog to digital converter of the
  //! APDS9960.
  //! @return The integration time in milliseconds.
  float getADCIntegrationTime();

  //! @brief Set the ADC gain.
  //! @param[in] gain The ADC gain.
  void setADCGain(apds9960AGain_t gain);

  //! @brief Get the ADC gain.
  //! @return The ADC gain.
  apds9960AGain_t getADCGain();

  //! @brief Set LED brightness for proximity/gesture.
  //! @param[in] drive The LED drive strength.
  //! @param[in] boost The LED boost.
  void setLED(apds9960LedDrive_t drive, apds9960LedBoost_t boost);

  //! @brief Enable the APDS9960 proximity sensor.
  //! @param[in] enable True to enable the proximity sensor, false to disable.
  void enableProximity(bool enable = true);

  //! @brief Set the proximity gain.
  //! @param[in] gain The proximity gain.
  void setProximityGain(apds9960PGain_t gain);

  //! @brief Get the proximity gain.
  //! @return The proximity gain.
  apds9960PGain_t getProximityGain();

  //! @brief Set the proximity pulse length and number of pulses.
  //! @param[in] pLen The proximity pulse length.
  //! @param[in] pulses The number of pulses.
  void setProxPulse(apds9960PPulseLen_t pLen, uint8_t pulses);

  //! @brief Enable the proximity interrupt.
  void enableProximityInterrupt();

  //! @brief Disable the proximity interrupt.
  void disableProximityInterrupt();

  //! @brief Read the proximity data.
  //! @return The proximity data.
  uint8_t readProximity();

  //! @brief Set the proximity interrupt thresholds.
  //! @param[in] low The low threshold.
  //! @param[in] high The high threshold.
  //! @param[in] persistence The persistence.
  void setProximityInterruptThreshold(uint8_t low, uint8_t high,
                                      uint8_t persistence = 4);

  //! @brief Get the proximity interrupt status.
  //! @return True if the proximity interrupt is set, false otherwise.
  bool getProximityInterrupt();

  // gesture

  //! @brief Enable gesture detection.
  //! @param[in] enable True to enable gesture detection, false to disable.
  void enableGesture(bool enable = true);

  //! @brief Check if the gesture data is valid.
  //! @return True if the gesture data is valid, false otherwise.
  bool gestureValid();

  //! @brief Set the gesture dimensions.
  //! @param[in] dimensions The gesture dimensions.
  void setGestureDimensions(uint8_t dimensions);

  //! @brief Set the gesture FIFO threshold.
  //! @param[in] threshold The threshold.
  void setGestureFIFOThreshold(uint8_t threshold);

  //! @brief Set the gesture gain.
  //! @param[in] gain The gain.
  void setGestureGain(uint8_t gain);

  //! @brief Set the gesture proximity threshold.
  //! @param[in] thresh The proximity threshold.
  void setGestureProximityThreshold(uint8_t threshold);

  //! @brief Set the gesture offset.
  //! @param[in] offset_up The offset for up gestures.
  //! @param[in] offset_down The offset for down gestures.
  //! @param[in] offset_left The offset for left gestures.
  //! @param[in] offset_right The offset for right gestures.
  void setGestureOffset(uint8_t offset_up, uint8_t offset_down,
                        uint8_t offset_left, uint8_t offset_right);

  //! @brief Read the gesture data.
  //! @return The gesture data @see APDS9960_UP, @see APDS9960_DOWN, @see
  //! APDS9960_LEFT, @see APDS9960_RIGHT
  uint8_t readGesture();

  //! @brief Reset the gesture counts.
  void resetCounts();

  //! @brief Enable color reading
  //! @param[in] enable True to enable color reading, false to disable.
  void enableColor(bool enable = true);

  //! @brief Check if the color data is ready.
  //! @return True if the color data is ready, false otherwise.
  bool colorDataReady();

  //! @brief Get the color data.
  //! @param[out] r The red value.
  //! @param[out] g The green value.
  //! @param[out] b The blue value.
  //! @param[out] c The clear value.
  void getColorData(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c);

  //! @brief Calculate the color temperature from the given RGB values.
  //! @param[in] r The red value.
  //! @param[in] g The green value.
  //! @param[in] b The blue value.
  //! @return The color temperature in degrees Kelvin.
  uint16_t calculateColorTemperature(uint16_t r, uint16_t g, uint16_t b);

  //! @brief Calculate the lux value from the given RGB values.
  //! @param[in] r The red value.
  //! @param[in] g The green value.
  //! @param[in] b The blue value.
  //! @return The lux value.
  uint16_t calculateLux(uint16_t r, uint16_t g, uint16_t b);

  //! @brief Enable the color interrupt.
  void enableColorInterrupt();

  //! @brief Disable the color interrupt.
  void disableColorInterrupt();

  //! @brief Clear the interrupt.
  void clearInterrupt();

  //! @brief Set the interrupt limits.
  //! @param[in] l The low limit.
  //! @param[in] h The high limit.
  void setIntLimits(uint16_t l, uint16_t h);

  //! @brief Enable the APDS9960.
  //! @param[in] enable True to enable the APDS9960, false to disable.
  void enable(bool enable = true);

  //! @brief Function to power a number.
  //! @param[in] x The number to power.
  //! @param[in] y The power.
  float powf(const float x, const float y);

 private:
  I2C *m_i2c;

  uint8_t gestCnt;

  uint8_t UCount;
  uint8_t DCount;

  uint8_t LCount;
  uint8_t RCount;

  struct enable {
    // power on
    uint8_t PON : 1;

    // ALS enable
    uint8_t AEN : 1;

    // Proximity detect enable
    uint8_t PEN : 1;

    // wait timer enable
    uint8_t WEN : 1;

    // ALS interrupt enable
    uint8_t AIEN : 1;

    // proximity interrupt enable
    uint8_t PIEN : 1;

    // gesture enable
    uint8_t GEN : 1;

    uint8_t get() {
      return (GEN << 6) | (PIEN << 5) | (AIEN << 4) | (WEN << 3) | (PEN << 2) |
             (AEN << 1) | PON;
    };
  };
  struct enable _enable;

  struct pers {
    // ALS Interrupt Persistence. Controls rate of Clear channel interrupt to
    // the host processor
    uint8_t APERS : 4;

    // proximity interrupt persistence, controls rate of prox interrupt to host
    // processor
    uint8_t PPERS : 4;

    uint8_t get() { return (PPERS << 4) | APERS; };
  };
  pers _pers;

  struct config1 {
    uint8_t WLONG : 1;

    uint8_t get() { return WLONG << 1; };
  };
  config1 _config1;

  struct ppulse {
    /*Proximity Pulse Count. Specifies the number of proximity pulses to be
    generated on LDR. Number of pulses is set by PPULSE value plus 1.
    */
    uint8_t PPULSE : 6;

    // Proximity Pulse Length. Sets the LED-ON pulse width during a proximity
    // LDR pulse.
    uint8_t PPLEN : 2;

    uint8_t get() { return (PPLEN << 6) | PPULSE; }
  };
  ppulse _ppulse;

  struct control {
    // ALS and Color gain control
    uint8_t AGAIN : 2;

    // proximity gain control
    uint8_t PGAIN : 2;

    // led drive strength
    uint8_t LDRIVE : 2;

    uint8_t get() { return (LDRIVE << 6) | (PGAIN << 2) | AGAIN; }
  };
  control _control;

  struct config2 {
    /* Additional LDR current during proximity and gesture LED pulses. Current
    value, set by LDRIVE, is increased by the percentage of LED_BOOST.
    */
    uint8_t LED_BOOST : 2;

    // clear photodiode saturation int enable
    uint8_t CPSIEN : 1;

    // proximity saturation interrupt enable
    uint8_t PSIEN : 1;

    uint8_t get() {
      return (PSIEN << 7) | (CPSIEN << 6) | (LED_BOOST << 4) | 1;
    }
  };
  config2 _config2;

  struct status {
    /* ALS Valid. Indicates that an ALS cycle has completed since AEN was
    asserted or since a read from any of the ALS/Color data registers.
    */
    uint8_t AVALID : 1;

    /* Proximity Valid. Indicates that a proximity cycle has completed since PEN
    was asserted or since PDATA was last read. A read of PDATA automatically
    clears PVALID.
    */
    uint8_t PVALID : 1;

    /* Gesture Interrupt. GINT is asserted when GFVLV becomes greater than
    GFIFOTH or if GVALID has become asserted when GMODE transitioned to zero.
    The bit is reset when FIFO is completely emptied (read).
    */
    uint8_t GINT : 1;

    // ALS Interrupt. This bit triggers an interrupt if AIEN in ENABLE is set.
    uint8_t AINT : 1;

    // Proximity Interrupt. This bit triggers an interrupt if PIEN in ENABLE is
    // set.
    uint8_t PINT : 1;

    /* Indicates that an analog saturation event occurred during a previous
    proximity or gesture cycle. Once set, this bit remains set until cleared by
    clear proximity interrupt special function command (0xE5 PICLEAR) or by
    disabling Prox (PEN=0). This bit triggers an interrupt if PSIEN is set.
    */
    uint8_t PGSAT : 1;

    /* Clear Photodiode Saturation. When asserted, the analog sensor was at the
    upper end of its dynamic range. The bit can be de-asserted by sending a
    Clear channel interrupt command (0xE6 CICLEAR) or by disabling the ADC
    (AEN=0). This bit triggers an interrupt if CPSIEN is set.
    */
    uint8_t CPSAT : 1;

    void set(uint8_t data) {
      AVALID = data & 0x01;
      PVALID = (data >> 1) & 0x01;
      GINT = (data >> 2) & 0x01;
      AINT = (data >> 4) & 0x01;
      PINT = (data >> 5) & 0x01;
      PGSAT = (data >> 6) & 0x01;
      CPSAT = (data >> 7) & 0x01;
    }
  };
  status _status;

  struct config3 {
    // proximity mask
    uint8_t PMASK_R : 1;
    uint8_t PMASK_L : 1;
    uint8_t PMASK_D : 1;
    uint8_t PMASK_U : 1;

    /* Sleep After Interrupt. When enabled, the device will automatically enter
    low power mode when the INT pin is asserted and the state machine has
    progressed to the SAI decision block. Normal operation is resumed when INT
    pin is cleared over I2C.
    */
    uint8_t SAI : 1;

    /* Proximity Gain Compensation Enable. This bit provides gain compensation
    when proximity photodiode signal is reduced as a result of sensor masking.
    If only one diode of the diode pair is contributing, then only half of the
    signal is available at the ADC; this results in a maximum ADC value of 127.
    Enabling PCMP enables an additional gain of 2X, resulting in a maximum ADC
    value of 255.
    */
    uint8_t PCMP : 1;

    uint8_t get() {
      return (PCMP << 5) | (SAI << 4) | (PMASK_U << 3) | (PMASK_D << 2) |
             (PMASK_L << 1) | PMASK_R;
    }
  };
  config3 _config3;

  struct gconf1 {
    /* Gesture Exit Persistence. When a number of consecutive gesture end
    occurrences become equal or greater to the GEPERS value, the Gesture state
    machine is exited.
    */
    uint8_t GEXPERS : 2;

    /* Gesture Exit Mask. Controls which of the gesture detector photodiodes
    (UDLR) will be included to determine a gesture end and subsequent exit
    of the gesture state machine. Unmasked UDLR data will be compared with the
    value in GTHR_OUT. Field value bits correspond to UDLR detectors.
    */
    uint8_t GEXMSK : 4;

    /* Gesture FIFO Threshold. This value is compared with the FIFO Level (i.e.
    the number of UDLR datasets) to generate an interrupt (if enabled).
    */
    uint8_t GFIFOTH : 2;

    uint8_t get() { return (GFIFOTH << 6) | (GEXMSK << 2) | GEXPERS; }
  };
  gconf1 _gconf1;

  struct gconf2 {
    /* Gesture Wait Time. The GWTIME controls the amount of time in a low power
    mode between gesture detection cycles.
    */
    uint8_t GWTIME : 3;

    // Gesture LED Drive Strength. Sets LED Drive Strength in gesture mode.
    uint8_t GLDRIVE : 2;

    // Gesture Gain Control. Sets the gain of the proximity receiver in gesture
    // mode.
    uint8_t GGAIN : 2;

    uint8_t get() { return (GGAIN << 5) | (GLDRIVE << 3) | GWTIME; }
  };
  gconf2 _gconf2;

  struct gpulse {
    /* Number of Gesture Pulses. Specifies the number of pulses to be generated
    on LDR. Number of pulses is set by GPULSE value plus 1.
    */
    uint8_t GPULSE : 6;

    // Gesture Pulse Length. Sets the LED_ON pulse width during a Gesture LDR
    // Pulse.
    uint8_t GPLEN : 2;

    uint8_t get() { return (GPLEN << 6) | GPULSE; }
  };
  gpulse _gpulse;

  struct gconf3 {
    /* Gesture Dimension Select. Selects which gesture photodiode pairs are
    enabled to gather results during gesture.
    */
    uint8_t GDIMS : 2;

    uint8_t get() { return GDIMS; }
  };
  gconf3 _gconf3;

  struct gconf4 {
    /* Gesture Mode. Reading this bit reports if the gesture state machine is
    actively running, 1 = Gesture, 0= ALS, Proximity, Color. Writing a 1 to this
    bit causes immediate entry in to the gesture state machine (as if GPENTH had
    been exceeded). Writing a 0 to this bit causes exit of gesture when current
    analog conversion has finished (as if GEXTH had been exceeded).
    */
    uint8_t GMODE : 1;

    /* Gesture interrupt enable. Gesture Interrupt Enable. When asserted, all
    gesture related interrupts are unmasked.
    */
    uint8_t GIEN : 2;

    uint8_t get() { return (GIEN << 1) | GMODE; }
    void set(uint8_t data) {
      GIEN = (data >> 1) & 0x01;
      GMODE = data & 0x01;
    }
  };
  gconf4 _gconf4;

  struct gstatus {
    /* Gesture FIFO Data. GVALID bit is sent when GFLVL becomes greater than
    GFIFOTH (i.e. FIFO has enough data to set GINT). GFIFOD is reset when GMODE
    = 0 and the GFLVL=0 (i.e. All FIFO data has been read).
    */
    uint8_t GVALID : 1;

    /* Gesture FIFO Overflow. A setting of 1 indicates that the FIFO has filled
    to capacity and that new gesture detector data has been lost.
    */
    uint8_t GFOV : 1;

    void set(uint8_t data) {
      GFOV = (data >> 1) & 0x01;
      GVALID = data & 0x01;
    }
  };
  gstatus _gstatus;

  //! @brief Flag to indicate if the APDS9960 is connected.
  bool m_is_connected;
};
