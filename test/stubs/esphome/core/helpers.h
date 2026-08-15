#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <functional>
#include <cmath>
#include <arpa/inet.h>
#define PACKED __attribute__((packed))
namespace esphome {
template<typename T> T clamp(T v, T lo, T hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline std::string format_hex(uint16_t v) { char b[8]; snprintf(b, sizeof(b), "%04x", v); return std::string(b); }
class GPIOPin {
 public:
  virtual void setup() {}
  virtual void digital_write(bool) {}
};
}
namespace esphome { inline uint32_t random_uint32() { return 12345; } }
