#pragma once

#include <iostream>
#include <string>

//! @brief Logger class to log messages to the console.
class Logger {
 public:
  //! @brief Log a debug message.
  //! @param[in] message The message to log.
  static void debug(const std::string& message);

  //! @brief Log an info message.
  //! @param[in] message The message to log.
  static void info(const std::string& message);

  //! @brief Log a warning message.
  //! @param[in] message The message to log.
  static void warn(const std::string& message);

  //! @brief Log an error message.
  //! @param[in] message The message to log.
  static void error(const std::string& message);

 private:
  //! @brief Private constructor to prevent instantiation.
  Logger();
};
