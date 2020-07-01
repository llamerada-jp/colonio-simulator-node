#pragma once

#include "colonio/colonio.hpp"

struct Options {
  std::string simulation_name;

  std::string seed_url;
  std::string seed_token;
};

class Base : public colonio::Colonio {
 public:
  Base(unsigned int interval_);
  virtual ~Base();

  void run(const Options& options);

  virtual void setup() = 0;
  virtual void step()  = 0;

 protected:
  Options options;
  const unsigned int interval;

  void on_output_log(colonio::LogLevel level, const std::string& message) override;
};
