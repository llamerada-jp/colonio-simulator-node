
#include "base.hpp"

#include <iostream>
#include <thread>

#include "logger.hpp"

Base::Base(const Config& config_, Logger& logger_, unsigned int interval_) :
    config(config_), logger(logger_), interval(interval_) {
}

Base::~Base() {
}

void Base::run() {
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

void Base::on_output_log(const std::string& json) {
  logger.output(json);
}

std::chrono::system_clock::time_point Base::msec_start = std::chrono::system_clock::now();

int64_t Base::get_current_msec() {
  auto now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(now - msec_start).count();
}
