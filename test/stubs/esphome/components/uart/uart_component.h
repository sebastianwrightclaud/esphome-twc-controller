#pragma once
#include "esphome/core/helpers.h"
#include <vector>
#include <deque>
namespace esphome { namespace uart {
class UARTComponent {
 public:
  std::vector<uint8_t> tx;
  std::deque<uint8_t> rx;
  int available() { return (int)rx.size(); }
  bool read_byte(uint8_t *b) { if (rx.empty()) return false; *b = rx.front(); rx.pop_front(); return true; }
  int read_array(uint8_t *b, size_t n) { for (size_t i = 0; i < n; i++) { if (rx.empty()) return (int)i; b[i] = rx.front(); rx.pop_front(); } return (int)n; }
  void write_array(const uint8_t *b, size_t n) { for (size_t i = 0; i < n; i++) tx.push_back(b[i]); }
  void flush() {}
};
class UARTDevice { public: UARTComponent *parent_{nullptr}; };
}}
