#include "main/logger/logger.h"

Logger::Logger() {}

void Logger::debug(const std::string& message) {
  std::cout << "[DEBUG] " << message << std::endl;
}

void Logger::info(const std::string& message) {
  std::cout << "[INFO] " << message << std::endl;
}

void Logger::warn(const std::string& message) {
  std::cout << "[WARN] " << message << std::endl;
}

void Logger::error(const std::string& message) {
  std::cout << "[ERROR] " << message << std::endl;
}
