#pragma once

#include <picojson/picojson.h>

#include <iostream>
#include <string>

class Config {
 public:
  bool check(const std::string& path) const;
  void read(const std::string& fname);

  template<class T>
  const T& get(const std::string& path) const {
    const picojson::value* value = get_value(path);

    if (value == nullptr) {
      std::cerr << "could not find config : " << path << std::endl;

      exit(EXIT_FAILURE);
    }

    return value->get<T>();
  }

  template<class T>
  const T& get(const std::string& path, const T& d) const {
    const picojson::value* value = get_value(path);

    if (value == nullptr) {
      return d;
    }

    return value->get<T>();
  }

 private:
  picojson::value v;

  const picojson::value* get_value(const std::string& path) const;
};
