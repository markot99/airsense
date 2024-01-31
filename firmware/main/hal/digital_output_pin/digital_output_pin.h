#pragma once

#include "driver/gpio.h"

//! @brief A class for controlling a digital output pin.
class DigitalOutputPin {
 public:
  //! @brief Constructor
  DigitalOutputPin(int pin_num);

  //! @brief Destructor
  ~DigitalOutputPin();

  //! @brief Set the pin level to high.
  void setHigh();

  //! @brief Set the pin level to low.
  void setLow();

  //! @brief Toggle the pin level.
  void toggle();

  //! @brief Create a rising edge.
  void createRisingEdge();

  //! @brief Create a falling edge.
  void createFallingEdge();

 private:
  //! @brief Initialize the pin.
  void init();

  //! @brief The pin number.
  const gpio_num_t m_gpio_num;

  //! @brief The current pin level.
  bool m_is_high;
};
