#include "logger.hpp"

#include <iostream>

#include "mongo.hpp"

Logger::Logger() : mongo(nullptr) {
}

Logger::~Logger() {
}

void Logger::setup(bool enable_stdout) {
  this->enable_stdout = enable_stdout;
}

void Logger::set_mongo(Mongo& mongo) {
  this->mongo = &mongo;
}

void Logger::output(const std::string& json) {
  if (enable_stdout) {
    std::cout << json << std::endl;
  }

  if (mongo) {
    if (local_nid.empty() && static_cast<bool>(get_local_nid)) {
      local_nid = get_local_nid();
      if (local_nid.empty()) {
        return;
      }
    }

    mongo->output(local_nid, json);
  }
}
