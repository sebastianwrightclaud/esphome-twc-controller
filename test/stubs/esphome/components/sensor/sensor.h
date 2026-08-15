#pragma once
#include "esphome/core/entity_base.h"
namespace esphome { namespace sensor {
class Sensor { public: void publish_state(float) {} };
}}
#define SUB_SENSOR(name) \
 public: void set_##name##_sensor(esphome::sensor::Sensor *s) { this->name##_sensor_ = s; } \
 protected: esphome::sensor::Sensor *name##_sensor_{nullptr}; \
 public:
