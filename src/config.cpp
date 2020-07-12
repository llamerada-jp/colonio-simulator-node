#include "config.hpp"

#include <fstream>
#include <sstream>
#include <string>

void Config::read(const std::string& fname) {
  std::ifstream fs;
  fs.open(fname, std::ios::binary);
  if (!fs) {
    std::cerr << "could not open config file : " << fname << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string err = picojson::parse(v, fs);
  fs.close();

  if (!err.empty()) {
    std::cerr << err << std::endl;
  }
}

const picojson::value* Config::get_value(const std::string& path) const {
  std::stringstream ss(path);
  std::string p;
  const picojson::value* pwd = &v;
  // split path by '.'
  while (getline(ss, p, '.')) {
    if (!pwd->is<picojson::object>()) {
      std::cerr << "config structure is invalid : " << path << std::endl;
      exit(EXIT_FAILURE);
    }
    const picojson::object& o = pwd->get<picojson::object>();

    if (o.find(p) == o.end()) {
      return nullptr;
    }

    pwd = &o.at(p);
  }

  return pwd;
}
