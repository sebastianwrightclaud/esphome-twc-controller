#pragma once
namespace esphome { namespace number {
class Number {
 public:
  void publish_state(float) {}
 protected:
  virtual void control(float) = 0;
};
}}
