#pragma once
#include <cstdint>

//! @brief Timer class
class Timer {
 public:
  //! @brief Sleep for the given number of milliseconds
  //! @param ms The number of milliseconds to sleep
  static void sleepMS(uint32_t ms);
};