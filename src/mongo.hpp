#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
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
  std::condition_variable cond;
  bool flg_exit;
  std::queue<std::pair<std::string, std::string>> logs;

  std::string uri_str;
  std::string db_str;
  std::string coll_str;

  void run();
};
