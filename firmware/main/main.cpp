
#include "main/hal/timer/timer.h"
#include "main/logger/logger.h"
#include "main/runtime/runtime.h"

void run(void *pvParameter) {
  Runtime *runtime = (Runtime *)pvParameter;

  while (true) {
    runtime->run();
  }
}

extern "C" void app_main(void) {
  Runtime runtime = Runtime();

  Logger::info("Starting initialization...\n");
  runtime.initialize();
  Logger::info("Finished initialization.\n");

  Logger::info("Starting main loop...\n");
  runtime.run();
  Logger::info("Finished main loop.\n");
  return;
}
