#include "mongo.hpp"

#include <mongoc/mongoc.h>

#include <cassert>

#include "config.hpp"

Mongo::Mongo() {
}

Mongo::~Mongo() {
  if (th) {
    {
      std::lock_guard<std::mutex> lock(mtx);
      flg_exit = true;
      cond.notify_all();
    }

    th->join();
  }
}

void Mongo::setup(const Config &config) {
  assert(!th);

  uri_str  = config.get<std::string>("mongodb.url");
  db_str   = config.get<std::string>("mongodb.database");
  coll_str = config.get<std::string>("mongodb.collection");

  flg_exit = false;
  th       = std::make_unique<std::thread>(&Mongo::run, this);
}

void Mongo::output(const std::string &nid, const std::string &json) {
  std::lock_guard<std::mutex> lock(mtx);

  logs.push(std::make_pair(nid, json));

  cond.notify_all();
}

void Mongo::run() {
  bson_error_t error;

  mongoc_init();

  mongoc_uri_t *uri = mongoc_uri_new_with_error(uri_str.c_str(), &error);
  if (!uri) {
    fprintf(
        stderr,
        "failed to parse URI: %s\n"
        "error message:       %s\n",
        uri_str.c_str(), error.message);
    exit(EXIT_FAILURE);
  }

  mongoc_client_t *client = mongoc_client_new_from_uri(uri);
  if (!client) {
    exit(EXIT_FAILURE);
  }
  mongoc_client_set_appname(client, "simulator");

  mongoc_collection_t *collection = mongoc_client_get_collection(client, db_str.c_str(), coll_str.c_str());

  while (true) {
    std::unique_lock<std::mutex> lock(mtx);
    cond.wait(lock, [this] { return flg_exit || logs.size() != 0; });

    // check exit flag
    if (flg_exit) {
      break;
    }

    std::string &nid  = logs.front().first;
    std::string &json = logs.front().second;

    // convert json to bson
    bson_t *bson = bson_new_from_json(reinterpret_cast<const uint8_t *>(json.c_str()), -1, &error);
    if (!bson) {
      fprintf(stderr, "%s\n", error.message);
      exit(EXIT_FAILURE);
    }

    // add `nid` to bson
    BSON_APPEND_UTF8(bson, "nid", nid.c_str());

    // intert bson to mongodb
    if (!mongoc_collection_insert_one(collection, bson, NULL, NULL, &error)) {
      fprintf(stderr, "%s\n", error.message);
      exit(EXIT_FAILURE);
    }

    bson_destroy(bson);

    logs.pop();
  }

  mongoc_collection_destroy(collection);

  mongoc_uri_destroy(uri);
  uri = nullptr;

  mongoc_cleanup();
}
