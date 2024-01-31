#include "main/hal/digital_output_pin/digital_output_pin.h"

#include "main/logger/logger.h"

DigitalOutputPin::DigitalOutputPin(int pin_num)
    : m_gpio_num(static_cast<gpio_num_t>(pin_num)), m_is_high(false) {
  init();
}

DigitalOutputPin::~DigitalOutputPin() {}

void DigitalOutputPin::init() {
  Logger::debug("Initializing digital output pin: " + std::to_string(m_gpio_num));
  gpio_reset_pin(m_gpio_num);
  gpio_set_direction(m_gpio_num, GPIO_MODE_OUTPUT);
  gpio_set_level(m_gpio_num, 0);
  Logger::debug("Finished initializing digital output pin: " +
                std::to_string(m_gpio_num));
}

void DigitalOutputPin::setHigh() {
  gpio_set_level(m_gpio_num, 1);
  m_is_high = true;
}

void DigitalOutputPin::setLow() {
  gpio_set_level(m_gpio_num, 0);
  m_is_high = false;
}

void DigitalOutputPin::toggle() {
  gpio_set_level(m_gpio_num, !m_is_high);
  m_is_high = !m_is_high;
}

void DigitalOutputPin::createRisingEdge() {
  setLow();
  setHigh();
}

void DigitalOutputPin::createFallingEdge() {
  setHigh();
  setLow();
}