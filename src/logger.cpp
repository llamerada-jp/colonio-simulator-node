#include "logger.hpp"

#include <iostream>

#include "colonio/colonio.hpp"
#include "config.hpp"
#include "mongo.hpp"

Logger::Logger() : mongo(nullptr) {
}

Logger::~Logger() {
}

void Logger::setup(const Config& config, bool enable_stdout, bool debug_mode) {
  this->enable_stdout = enable_stdout;
  this->debug_mode    = debug_mode;

  if (config.check("logs")) {
    const picojson::array& logs = config.get<picojson::array>("logs");
    for (const auto& one : logs) {
      filter.insert(one.get<std::string>());
    }
  }
}

void Logger::set_mongo(Mongo& mongo) {
  this->mongo = &mongo;
}

void Logger::output(const std::string& json) {
  picojson::value v;
  std::string err = picojson::parse(v, json);
  if (!err.empty()) {
    std::cerr << err << std::endl;
    return;
  }
  picojson::object& o = v.get<picojson::object>();

  if (local_nid.empty() && static_cast<bool>(get_local_nid)) {
    local_nid = get_local_nid();
    if (local_nid.empty()) {
      return;
    }
  }

  std::string level = o.at("level").get<std::string>();
  if (enable_stdout && (debug_mode || level == colonio::LogLevel::ERROR || level == colonio::LogLevel::WARN)) {
    std::cout << local_nid << ":" << json << std::endl;
  }

  if (mongo) {
    o.insert(std::make_pair("nid", picojson::value(local_nid)));

    if (!filter.empty()) {
      std::string message = o.at("message").get<std::string>();
      if (filter.find(message) == filter.end()) {
        return;
      }
    }

    mongo->output(local_nid, picojson::value(o).serialize());
  }
}
