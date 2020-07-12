#include "logger.hpp"

#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/make_unique.hpp>
#include <iostream>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/logger.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

class MongoLogger final : public mongocxx::logger {
 public:
  explicit MongoLogger(std::ostream* stream) : _stream(stream) {
  }

  void operator()(
      mongocxx::log_level level, bsoncxx::stdx::string_view domain,
      bsoncxx::stdx::string_view message) noexcept override {
    if (level >= mongocxx::log_level::k_trace) return;
    *_stream << '[' << mongocxx::to_string(level) << '@' << domain << "] " << message << '\n';
  }

 private:
  std::ostream* const _stream;
};

mongocxx::instance Logger::instance{bsoncxx::stdx::make_unique<MongoLogger>(&std::cout)};

void Logger::setup(const std::string& uri) {
  mongocxx::uri mongo_uri(uri);

  client = std::make_unique<mongocxx::client>(mongo_uri);
  if (!(*client)) {
    std::cout << "failed to connect mongodb : " << uri << std::endl;
    exit(EXIT_FAILURE);
  }

  db = client->database("mydb");
  if (!db) {
    std::cout << "failed to get database : "
              << "mydb" << std::endl;
  }

  coll = db.collection("test");
  if (!coll) {
    std::cout << "failed to get collection : "
              << "test" << std::endl;
  }

  try {
    coll.name();
  } catch (const mongocxx::logic_error& e) {
    std::cout << "Using an uninitialized collection throws:" << std::endl;
  }
}

void Logger::output(const std::string& json) {
  std::cout << json << std::endl;
  bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(bsoncxx::from_json(json));
}
