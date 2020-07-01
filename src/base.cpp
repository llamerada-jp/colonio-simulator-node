
#include "base.hpp"

#include <chrono>
#include <iostream>
#include <thread>

static std::chrono::system_clock::time_point msec_start = std::chrono::system_clock::now();
int64_t get_current_msec() {
  auto now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now - msec_start).count();
}

Base::Base(unsigned int interval_) : interval(interval_) {
}

Base::~Base() {
}

void Base::run(const Options& options_) {
  options = options_;

  setup();

  int64_t next_msec = get_current_msec();

  while (true) {
    step();

    int64_t current_msec = get_current_msec();
    while (next_msec <= current_msec) {
      next_msec += interval;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(next_msec - current_msec));
  }
}

void Base::on_output_log(colonio::LogLevel level, const std::string& message) {
  std::cout << message << std::endl;
}
