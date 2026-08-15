#pragma once
#include <cstdio>
#define ESP_LOGD(tag, ...) do { (void)(tag); printf(__VA_ARGS__); } while (0)
#define ESP_LOGE(tag, ...) do { (void)(tag); printf(__VA_ARGS__); } while (0)
#define ESP_LOGW(tag, ...) do { (void)(tag); printf(__VA_ARGS__); } while (0)
#define ESP_LOGV(tag, ...) do { (void)(tag); printf(__VA_ARGS__); } while (0)
#define ESP_LOGCONFIG(tag, ...) do { (void)(tag); printf(__VA_ARGS__); } while (0)
#define LOG_PIN(s, p) do { (void)(s); (void)(p); } while (0)
#define ESP_LOGI(tag, ...) do { (void)(tag); printf(__VA_ARGS__); } while (0)
