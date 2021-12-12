#pragma once

#include <chrono>

#include "colonio/colonio.hpp"

class Config;
class Logger;

class Base {
 public:
  Base(colonio::Colonio& c_, const Config& config_, Logger& logger_, unsigned int interval_);
  virtual ~Base();

  void run();

 protected:
  colonio::Colonio& c;
  const Config& config;
  Logger& logger;
  const unsigned int interval;

  static std::chrono::system_clock::time_point msec_start;
  static int64_t get_current_msec();

  virtual void setup() = 0;
  virtual void step()  = 0;
};
