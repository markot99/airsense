#include "main/hal/timer/timer.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_system.h"

void Timer::sleepMS(uint32_t ms) { vTaskDelay(ms / portTICK_PERIOD_MS); }
