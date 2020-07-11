#include "logger.hpp"

#include <bsoncxx/json.hpp>
#include <iostream>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

mongocxx::instance Logger::instance{};

void Logger::setup(const std::string& uri) {
  mongocxx::uri mongo_uri(uri);
  client = std::make_unique<mongocxx::client>(mongo_uri);
  db     = (*client)["mydb"];
  coll   = db["test"];
}

void Logger::output(const std::string& json) {
  std::cout << json << std::endl;
  bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(bsoncxx::from_json(json));
}
