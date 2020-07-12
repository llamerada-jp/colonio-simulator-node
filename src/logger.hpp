#pragma once

#include <mongoc/mongoc.h>

#include <string>

class Config;

class Logger {
 public:
  Logger();
  virtual ~Logger();

  void setup(const Config &config);
  void output(const std::string &json);

 private:
  mongoc_uri_t *uri;
  mongoc_client_t *client;
  mongoc_database_t *database;
  mongoc_collection_t *collection;
};
