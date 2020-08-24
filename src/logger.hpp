#pragma once

#include <functional>
#include <set>
#include <string>

class Config;
class Mongo;

class Logger {
 public:
  std::function<std::string()> get_local_nid;

  Logger();
  virtual ~Logger();

  void setup(const Config &config, bool enable_stdout);
  void set_mongo(Mongo &mongo);
  void output(const std::string &json);

 private:
  bool enable_stdout;
  Mongo *mongo;
  std::string local_nid;
  std::set<std::string> filter;
};
