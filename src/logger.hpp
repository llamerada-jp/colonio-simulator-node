#pragma once

#include <mongoc/mongoc.h>

#include <functional>
#include <string>

class Config;

class Logger {
 public:
  std::function<std::string()> get_local_nid;

  Logger();
  virtual ~Logger();

  void setup(const Config &config);
  void output(const std::string &json);

 private:
  mongoc_uri_t *uri;
  mongoc_client_t *client;
  mongoc_database_t *database;
  mongoc_collection_t *collection;

  std::string local_nid;
};
