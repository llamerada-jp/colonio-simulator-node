#pragma once

#include <chrono>

#include "colonio/colonio.hpp"

class Config;
class Logger;

class Base : public colonio::Colonio {
 public:
  Base(const Config& config_, Logger& logger_, unsigned int interval_);
  virtual ~Base();

  void run();

 protected:
  const Config& config;
  Logger& logger;
  const unsigned int interval;

  void on_output_log(const std::string& json) override;

  static std::chrono::system_clock::time_point msec_start;
  static int64_t get_current_msec();

  virtual void setup() = 0;
  virtual void step()  = 0;
};
