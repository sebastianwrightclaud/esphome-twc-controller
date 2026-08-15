#pragma once
#include <string>
namespace esphome { namespace text_sensor {
class TextSensor { public: void publish_state(std::string) {} };
}}
#define SUB_TEXT_SENSOR(name) \
 public: void set_##name##_text_sensor(esphome::text_sensor::TextSensor *s) { this->name##_text_sensor_ = s; } \
 protected: esphome::text_sensor::TextSensor *name##_text_sensor_{nullptr}; \
 public:
