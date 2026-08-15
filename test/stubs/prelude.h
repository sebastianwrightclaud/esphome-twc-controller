#pragma once
#include <cstdint>
#include <cstddef>
#include <cstdio>
// FreeRTOS surface used by twc_protocol.cpp (globally available in the ESP32 build)
typedef void *TaskHandle_t;
#define portTICK_PERIOD_MS 1
inline int xTaskCreate(void (*fn)(void *), const char *name, uint32_t stack, void *param,
                       uint32_t prio, TaskHandle_t *handle) {
  (void)fn; (void)name; (void)stack; (void)param; (void)prio; (void)handle; return 1;
}
inline void vTaskDelay(uint32_t ticks) { (void)ticks; }
inline void vTaskDelete(TaskHandle_t h) { (void)h; }
