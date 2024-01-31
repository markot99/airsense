#include "main/driver/eink/eink_command.h"

#include <iostream>

#include "main/logger/logger.h"

EInkCommand::EInkCommand(uint8_t command, std::vector<uint8_t> data)
    : m_command(command), m_data(data) {}

EInkCommand::~EInkCommand() {}

std::vector<uint8_t> EInkCommand::getCommand() const {
  // Generate Command
  std::vector<uint8_t> command;

  // add frame header
  command.push_back(0xA5);

  // add frame length
  // calculate length and add to command
  // 1 byte for frame header, 2 byte frame length, 1 byte command type, 0 - 1024
  // bytes data, 4 bytes frame end, 1 byte parity
  // -> 9 static bytes + command parameter length

  uint16_t command_length = 9 + m_data.size();
  command.push_back((uint8_t)(command_length >> 8));
  command.push_back((uint8_t)(command_length));

  // add command type
  command.push_back(m_command);

  // add command data
  for (int i = 0; i < m_data.size(); i++) {
    command.push_back(m_data[i]);
  }

  // add frame end
  command.push_back(0xCC);
  command.push_back(0x33);
  command.push_back(0xC3);
  command.push_back(0x3C);

  // add parity
  uint8_t parity = 0;
  for (int i = 0; i < command.size(); i++) {
    parity ^= command[i];
  }
  command.push_back(parity);
  return command;
}

void EInkCommand::printCommand() const {
  std::vector<uint8_t> command = getCommand();
  for (int i = 0; i < command.size(); i++) {
    printf("status = 0x%02X\n", command[i]);
  }
  Logger::debug("\n");
}