#pragma once

#include <chrono>

#include "colonio/colonio.hpp"

struct Options {
  std::string simulation_name;

  std::string seed_url;
  std::string seed_token;
};

class Logger;

class Base : public colonio::Colonio {
 public:
  Base(Logger& logger_, unsigned int interval_);
  virtual ~Base();

  void run(const Options& options);

 protected:
  Options options;
  Logger& logger;
  const unsigned int interval;

  void on_output_log(const std::string& json) override;

  static std::chrono::system_clock::time_point msec_start;
  static int64_t get_current_msec();

  virtual void setup() = 0;
  virtual void step()  = 0;
};
