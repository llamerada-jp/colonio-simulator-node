#pragma once

#include "colonio/colonio.hpp"

struct Options {
  std::string simulation_name;

  std::string seed_url;
  std::string seed_token;
};

class Base : public colonio::Colonio {
 public:
  void setup(const Options& options);
  virtual void run() = 0;

 protected:
  Options options;

  void on_output_log(colonio::LogLevel level, const std::string& message) override;
};
