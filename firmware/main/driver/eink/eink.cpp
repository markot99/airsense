#include "main/driver/eink/eink.h"

#include <vector>

#include "esp_timer.h"
#include "main/logger/logger.h"

EInk::EInk(Uart* uart, DigitalOutputPin* display_wakeup_pin)
    : m_uart(uart),
      m_display_wakeup_pin(display_wakeup_pin),
      m_display_sleeping(false) {
  init();
}

EInk::~EInk() {}

void EInk::init() {
  Logger::info("Initializing eink display.");
  wakeUp();

  setMemory(MemoryMode::MEM_TF);
  Logger::info("Finished initializing eink display.");
}


void EInk::wakeUp() {
  // create rising edge to wake up display
  m_display_wakeup_pin->createRisingEdge();
  m_display_sleeping = false;
}

void EInk::sleep() {
  EInkCommand command(0x08);
  sendCommand(command);
  m_display_sleeping = true;
}

void EInk::setMemory(MemoryMode mode) {
  EInkCommand command(0x07, {static_cast<uint8_t>(mode)});
  sendCommand(command);
}

void EInk::setBaudRate(uint32_t baud_rate) {
  EInkCommand command(0x01,
                      {(uint8_t)(baud_rate >> 24), (uint8_t)(baud_rate >> 16),
                       (uint8_t)(baud_rate >> 8), (uint8_t)(baud_rate)});
  sendCommand(command);
}

void EInk::updateDisplay() {
  EInkCommand command(0x0A);
  sendCommand(command);
  Logger::debug("Trigger update");
}

void EInk::clearDisplay() {
  EInkCommand command(0x2E);
  sendCommand(command);
}

void EInk::setFontSize(FontSize font_size) {
  EInkCommand command(0x1F, {static_cast<uint8_t>(font_size)});
  sendCommand(command);
}

void EInk::setDirection(DisplayDirection direction) {
  EInkCommand command(0x0D, {static_cast<uint8_t>(direction)});
  sendCommand(command);
}

void EInk::drawPoint(uint16_t x, uint16_t y) {
  EInkCommand command(
      0x20, {(uint8_t)(x >> 8), (uint8_t)(x), (uint8_t)(y >> 8), (uint8_t)(y)});
  command.printCommand();
  sendCommand(command);
}

void EInk::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  Logger::debug("Draw line: " + std::to_string(x1) + " " + std::to_string(y1) +
                " " + std::to_string(x2) + " " + std::to_string(y2));
  EInkCommand command(0x22, {
                                (uint8_t)(x1 >> 8),
                                (uint8_t)(x1),
                                (uint8_t)(y1 >> 8),
                                (uint8_t)(y1),
                                (uint8_t)(x2 >> 8),
                                (uint8_t)(x2),
                                (uint8_t)(y2 >> 8),
                                (uint8_t)(y2),
                            });
  sendCommand(command);
}

void EInk::drawFilledRectangle(uint16_t x1, uint16_t y1, uint16_t x2,
                               uint16_t y2) {
  EInkCommand command(0x24, {
                                (uint8_t)(x1 >> 8),
                                (uint8_t)(x1),
                                (uint8_t)(y1 >> 8),
                                (uint8_t)(y1),
                                (uint8_t)(x2 >> 8),
                                (uint8_t)(x2),
                                (uint8_t)(y2 >> 8),
                                (uint8_t)(y2),
                            });
  sendCommand(command);
}

void EInk::drawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  EInkCommand command(0x25, {
                                (uint8_t)(x1 >> 8),
                                (uint8_t)(x1),
                                (uint8_t)(y1 >> 8),
                                (uint8_t)(y1),
                                (uint8_t)(x2 >> 8),
                                (uint8_t)(x2),
                                (uint8_t)(y2 >> 8),
                                (uint8_t)(y2),
                            });
  sendCommand(command);
}

void EInk::drawCircle(uint16_t x, uint16_t y, uint16_t r) {
  EInkCommand command(0x26, {
                                (uint8_t)(x >> 8),
                                (uint8_t)(x),
                                (uint8_t)(y >> 8),
                                (uint8_t)(y),
                                (uint8_t)(r >> 8),
                                (uint8_t)(r),
                            });
  sendCommand(command);
}

void EInk::drawFilledCircle(uint16_t x, uint16_t y, uint16_t r) {
  EInkCommand command(0x27, {
                                (uint8_t)(x >> 8),
                                (uint8_t)(x),
                                (uint8_t)(y >> 8),
                                (uint8_t)(y),
                                (uint8_t)(r >> 8),
                                (uint8_t)(r),
                            });
  sendCommand(command);
}

void EInk::drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                        uint16_t x3, uint16_t y3) {
  EInkCommand command(0x28, {
                                (uint8_t)(x1 >> 8),
                                (uint8_t)(x1),
                                (uint8_t)(y1 >> 8),
                                (uint8_t)(y1),
                                (uint8_t)(x2 >> 8),
                                (uint8_t)(x2),
                                (uint8_t)(y2 >> 8),
                                (uint8_t)(y2),
                                (uint8_t)(x3 >> 8),
                                (uint8_t)(x3),
                                (uint8_t)(y3 >> 8),
                                (uint8_t)(y3),
                            });
  sendCommand(command);
}

void EInk::drawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2,
                              uint16_t y2, uint16_t x3, uint16_t y3) {
  EInkCommand command(0x29, {
                                (uint8_t)(x1 >> 8),
                                (uint8_t)(x1),
                                (uint8_t)(y1 >> 8),
                                (uint8_t)(y1),
                                (uint8_t)(x2 >> 8),
                                (uint8_t)(x2),
                                (uint8_t)(y2 >> 8),
                                (uint8_t)(y2),
                                (uint8_t)(x3 >> 8),
                                (uint8_t)(x3),
                                (uint8_t)(y3 >> 8),
                                (uint8_t)(y3),
                            });
  sendCommand(command);
}

void EInk::drawText(uint16_t x, uint16_t y, const std::string& text) {
  std::vector<uint8_t> data;
  data.push_back((uint8_t)(x >> 8));
  data.push_back((uint8_t)(x));
  data.push_back((uint8_t)(y >> 8));
  data.push_back((uint8_t)(y));
  for (int i = 0; i < text.size(); i++) {
    data.push_back(text[i]);
  }
  EInkCommand command(0x30, data);
  sendCommand(command);
}

void EInk::sendCommand(const EInkCommand& command) {
  if (m_display_sleeping) {
    wakeUp();
  }
  m_uart->sendData(command.getCommand());
  // display should send "OK" after receiving command, but it would be to slow,
  // if we wait
}

void EInk::drawBitmap(uint16_t x, uint16_t y, const std::string& filename) {
  std::vector<uint8_t> data;
  data.push_back((uint8_t)(x >> 8));
  data.push_back((uint8_t)(x));
  data.push_back((uint8_t)(y >> 8));
  data.push_back((uint8_t)(y));
  for (int i = 0; i < filename.size(); i++) {
    data.push_back(filename[i]);
  }
  EInkCommand command(0x70, data);
  sendCommand(command);
}