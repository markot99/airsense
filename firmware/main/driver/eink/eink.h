#pragma once

#include <string>

#include "main/driver/eink/eink_command.h"
#include "main/hal/digital_output_pin/digital_output_pin.h"
#include "main/hal/uart/uart.h"

// enum with 32/48/64 dots font size
enum FontSize { SMALL = 1, MEDIUM = 2, LARGE = 3 };

// enum with 0/90/180/270 degree direction
enum DisplayDirection { DIR0 = 0, DIR90 = 1, DIR180 = 2, DIR270 = 3 };

// enum with NAND/TF memory mode
enum MemoryMode { MEM_NAND = 0, MEM_TF = 1 };

class EInk {
 public:
  //! Constructor
  //! @param[in] uart Pointer to the UART hal
  //! @param[in] display_wakeup_pin Pointer to the display wakeup pin
  EInk(Uart* uart, DigitalOutputPin* display_wakeup_pin);

  //! Destructor
  ~EInk();

  //! @brief Initialize the display.
  void init();

  //! @brief Set the memory mode of the display.
  //! @param[in] mode The memory mode to set.
  void setMemory(MemoryMode mode);

  //! @brief Set the baud rate of the display.
  //! @param[in] baud_rate The baud rate to set.
  void setBaudRate(uint32_t baud_rate);

  //! @brief Update and referesh the display.
  void updateDisplay();

  //! @brief Clear the display content in the buffer.
  void clearDisplay();

  //! @brief Set the font size for the following texts in the buffer.
  //! @param[in] font_size The font size to set.
  void setFontSize(FontSize font_size);

  //! @brief Set the display direction.
  //! @param[in] direction The direction to set.
  void setDirection(DisplayDirection direction);

  //! @brief Draw a point in the buffer.
  //! @param[in] x The x coordinate of the point.
  //! @param[in] y The y coordinate of the point.
  void drawPoint(uint16_t x, uint16_t y);

  //! @brief Draw a line in the buffer.
  //! @param[in] x1 The x coordinate of the first point.
  //! @param[in] y1 The y coordinate of the first point.
  //! @param[in] x2 The x coordinate of the second point.
  //! @param[in] y2 The y coordinate of the second point.
  void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

  //! @brief Draw a filled rectangle in the buffer.
  //! @param[in] x1 The x coordinate of the first point.
  //! @param[in] y1 The y coordinate of the first point.
  //! @param[in] x2 The x coordinate of the second point.
  //! @param[in] y2 The y coordinate of the second point.
  void drawFilledRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

  //! @brief Draw a rectangle in the buffer.
  //! @param[in] x1 The x coordinate of the first point.
  //! @param[in] y1 The y coordinate of the first point.
  //! @param[in] x2 The x coordinate of the second point.
  //! @param[in] y2 The y coordinate of the second point.
  void drawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

  //! @brief Draw a circle in the buffer.
  //! @param[in] x The x coordinate of the center.
  //! @param[in] y The y coordinate of the center.
  //! @param[in] r The radius of the circle.
  void drawCircle(uint16_t x, uint16_t y, uint16_t r);

  //! @brief Draw a filled circle in the buffer.
  //! @param[in] x The x coordinate of the center.
  //! @param[in] y The y coordinate of the center.
  //! @param[in] r The radius of the circle.
  void drawFilledCircle(uint16_t x, uint16_t y, uint16_t r);

  //! @brief Draw a triangle in the buffer.
  //! @param[in] x1 The x coordinate of the first point.
  //! @param[in] y1 The y coordinate of the first point.
  //! @param[in] x2 The x coordinate of the second point.
  //! @param[in] y2 The y coordinate of the second point.
  //! @param[in] x3 The x coordinate of the third point.
  //! @param[in] y3 The y coordinate of the third point.
  void drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                    uint16_t x3, uint16_t y3);

  //! @brief Draw a filled triangle in the buffer.
  //! @param[in] x1 The x coordinate of the first point.
  //! @param[in] y1 The y coordinate of the first point.
  //! @param[in] x2 The x coordinate of the second point.
  //! @param[in] y2 The y coordinate of the second point.
  //! @param[in] x3 The x coordinate of the third point.
  //! @param[in] y3 The y coordinate of the third point.
  void drawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                          uint16_t x3, uint16_t y3);

  //! @brief Draw a text in the buffer.
  //! @param[in] x The x coordinate of the text.
  //! @param[in] y The y coordinate of the text.
  //! @param[in] text The text to draw.
  void drawText(uint16_t x, uint16_t y, const std::string& text);

  //! @brief Draw a bitmap in the buffer.
  //! @param[in] x The x coordinate of the bitmap.
  //! @param[in] y The y coordinate of the bitmap.
  //! @param[in] filename The filename of the bitmap to draw.
  void drawBitmap(uint16_t x, uint16_t y, const std::string& filename);

 private:
  //! @brief Send a command to the display.
  //! @param[in] command The command to send.
  void sendCommand(const EInkCommand& command);

  //! @brief Force wake up the display.
  //! @return True if the display is awake, false otherwise.
  void wakeUp();

  //! @brief Put the display to sleep.
  void sleep();

  //! @brief Pointer to the UART hal.
  Uart* m_uart;

  //! @brief Pointer to the display wakeup pin.
  DigitalOutputPin* m_display_wakeup_pin;

  //! @brief Sleep mode of the display. True if the display is sleeping. False
  //! otherwise.
  bool m_display_sleeping;
};