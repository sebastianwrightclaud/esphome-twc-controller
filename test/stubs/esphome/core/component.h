#pragma once
#include "esphome/core/helpers.h"
namespace esphome {
namespace setup_priority { const float AFTER_CONNECTION = 5.0f; }
class Component {
 public:
  virtual void setup() {}
  virtual void loop() {}
  virtual void dump_config() {}
  virtual float get_setup_priority() const { return 0.0f; }
};
}
