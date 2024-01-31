#pragma once

#include <cstdint>
#include <vector>

//! @brief Class to generate eink commands
class EInkCommand {
 public:
  //! @brief Constructor
  //! @param command Command number
  //! @param data Command data
  EInkCommand(uint8_t command, std::vector<uint8_t> data = {});

  //! @brief Destructor
  ~EInkCommand();

  //! @brief Get the command as a vector of bytes
  //! @return Command as a vector of bytes
  std::vector<uint8_t> getCommand() const;

  //! @brief Print the command to the console
  void printCommand() const;

 private:
  //! @brief Command number
  uint8_t m_command;

  //! @brief Command data
  std::vector<uint8_t> m_data;
};