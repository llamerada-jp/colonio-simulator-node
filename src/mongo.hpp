#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <string>
#include <thread>

class Config;

class Mongo {
 public:
  Mongo();
  virtual ~Mongo();

  void setup(const Config &config);
  void output(const std::string &nid, const std::string &json);

 private:
  std::unique_ptr<std::thread> th;
  std::mutex mtx;
  bool flg_exit;

  struct EachThread {
    std::mutex mtx;
    std::string nid;
    std::queue<std::string> logs;
  };

  static thread_local EachThread *eth;
  std::set<std::unique_ptr<EachThread>> log_pool;

  std::string uri_str;
  std::string db_str;
  std::string coll_str;

  void run();
};
