#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <string>

class Logger {
 public:
  void setup(const std::string& uri);
  void output(const std::string& json);

 private:
  static mongocxx::instance instance;
  std::unique_ptr<mongocxx::client> client;
  mongocxx::database db;
  mongocxx::collection coll;
};
