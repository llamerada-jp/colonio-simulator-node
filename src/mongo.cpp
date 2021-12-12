#include "mongo.hpp"

#include <mongoc/mongoc.h>

#include <cassert>

#include "config.hpp"

thread_local Mongo::EachThread *Mongo::eth;

Mongo::Mongo() {
}

Mongo::~Mongo() {
  if (th) {
    {
      std::lock_guard<std::mutex> lock(mtx);
      flg_exit = true;
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
  bson_error_t error;

  if (eth == nullptr) {
    std::unique_ptr<EachThread> e = std::make_unique<EachThread>();
    eth                           = e.get();

    {
      std::lock_guard<std::mutex> lock(mtx);
      log_pool.insert(std::move(e));
    }
  }

  {
    std::lock_guard<std::mutex> lock(eth->mtx);
    if (eth->nid.empty()) {
      eth->nid = nid;
    }
    assert(eth->nid == nid);

    eth->logs.push(json);
  }
}

void Mongo::run() {
  bson_error_t error;
  std::map<std::string, unsigned int> log_counts;

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
    mongoc_bulk_operation_t *bulk = mongoc_collection_create_bulk_operation_with_opts(collection, nullptr);
    bool has_data                 = false;

    {
      std::lock_guard<std::mutex> lock(mtx);

      // check exit flag
      if (flg_exit) {
        break;
      }

      for (auto &eth : log_pool) {
        std::lock_guard<std::mutex> lock(eth->mtx);
        std::queue<std::string> &logs = eth->logs;

        auto log_count = log_counts.find(eth->nid);
        if (log_count == log_counts.end()) {
          log_counts.insert(std::make_pair(eth->nid, logs.size()));
        } else {
          log_count->second += logs.size();
        }

        while (!logs.empty()) {
          // convert json to bson
          std::string &json = logs.front();
          bson_t *bson      = bson_new_from_json(reinterpret_cast<const uint8_t *>(json.c_str()), -1, &error);
          if (!bson) {
            fprintf(stderr, "%s\n", error.message);
            exit(EXIT_FAILURE);
          }

          mongoc_bulk_operation_insert(bulk, bson);
          bson_destroy(bson);

          has_data = true;
          logs.pop();
        }
      }
    }

    if (has_data) {
      bson_t reply;
      bool ret = mongoc_bulk_operation_execute(bulk, &reply, &error);
      if (!ret) {
        fprintf(stderr, "%s\n", error.message);
        exit(EXIT_FAILURE);
      }
      bson_destroy(&reply);

      picojson::object o;
      for (const auto &it : log_counts) {
        o.insert(std::make_pair(it.first, picojson::value(static_cast<double>(it.second))));
      }
      std::cout << picojson::value(o).serialize() << std::endl;
    } else {
      sleep(1);
    }

    mongoc_bulk_operation_destroy(bulk);
  }

  mongoc_collection_destroy(collection);

  mongoc_uri_destroy(uri);
  uri = nullptr;

  mongoc_cleanup();
}
