
#include "base.hpp"

#include <iostream>

void Base::setup(const Options& options_) {
  options = options_;

  connect(options.seed_url, options.seed_token);
}

void Base::on_output_log(colonio::LogLevel level, const std::string& message) {
  std::cout << message << std::endl;
}
