#include "logger.hpp"

#include <cassert>
#include <iostream>

#include "config.hpp"

Logger::Logger() : uri(nullptr), client(nullptr), database(nullptr), collection(nullptr) {
  mongoc_cleanup();
}

Logger::~Logger() {
  mongoc_collection_destroy(collection);
  mongoc_database_destroy(database);
  mongoc_uri_destroy(uri);
  mongoc_client_destroy(client);

  collection = nullptr;
  database   = nullptr;
  client     = nullptr;
  uri        = nullptr;

  mongoc_cleanup();
}

void Logger::setup(const Config& config) {
  assert(uri == nullptr);
  assert(client == nullptr);
  assert(database == nullptr);
  assert(collection == nullptr);

  bson_error_t error;

  std::string uri_str   = config.get<std::string>("mongodb.url");
  std::string db_name   = config.get<std::string>("mongodb.database");
  std::string coll_name = config.get<std::string>("mongodb.collection");

  uri = mongoc_uri_new_with_error(uri_str.c_str(), &error);
  if (!uri) {
    fprintf(
        stderr,
        "failed to parse URI: %s\n"
        "error message:       %s\n",
        uri_str.c_str(), error.message);
    exit(EXIT_FAILURE);
  }

  client = mongoc_client_new_from_uri(uri);
  if (!client) {
    exit(EXIT_FAILURE);
  }

  mongoc_client_set_appname(client, "simulator");

  database   = mongoc_client_get_database(client, db_name.c_str());
  collection = mongoc_client_get_collection(client, db_name.c_str(), coll_name.c_str());
}

void Logger::output(const std::string& json) {
  bson_error_t error;
  bson_t* bson;

  std::cout << json << std::endl;

  bson = bson_new_from_json(reinterpret_cast<const uint8_t*>(json.c_str()), -1, &error);
  if (!bson) {
    fprintf(stderr, "%s\n", error.message);
    exit(EXIT_FAILURE);
  }

  if (!mongoc_collection_insert_one(collection, bson, NULL, NULL, &error)) {
    fprintf(stderr, "%s\n", error.message);
    exit(EXIT_FAILURE);
  }

  bson_destroy(bson);
}
