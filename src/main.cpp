
#include <getopt.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "base.hpp"
#include "config.hpp"
#include "logger.hpp"
#include "sphere/sphere.hpp"

option options[] = {
    {"help", no_argument, nullptr, 'h'},
    {0, 0, 0, 0},
};

void print_help(std::ostream& out) {
  out << std::endl;
  out << "`simulations` is a implement some simulations for colonio." << std::endl << std::endl;
  out << "Usage:" << std::endl;
  out << "  simulations [Flags] <config file>" << std::endl << std::endl;
  out << "Flags:" << std::endl;
  out << "  -h, --help                help for simulations" << std::endl;
}

Config decode_options(int argc, char* argv[]) {
  int opt;
  int idx_long = 0;
  while ((opt = getopt_long(argc, argv, "h", options, &idx_long)) != -1) {
    switch (opt) {
      case 'h':
        print_help(std::cout);
        exit(EXIT_SUCCESS);
        break;

      default:
        print_help(std::cerr);
        exit(EXIT_FAILURE);
    }
  }

  if (optind + 1 != argc) {
    std::cerr << argv[0] << ": config file is required" << std::endl;
    print_help(std::cerr);
    exit(EXIT_FAILURE);
  }

  Config config;
  config.read(argv[optind]);
  return config;
}

std::unique_ptr<Base> get_simulation(const Config& config, Logger& logger, const std::string& name) {
  std::unique_ptr<Base> sim;

  if (name == "sphere") {
    sim = std::make_unique<Sphere>(config, logger);
  }

  if (!sim) {
    std::cerr << "wrong simulation name" << std::endl;
    exit(EXIT_FAILURE);
  }

  return std::move(sim);
}

int main(int argc, char* argv[]) {
  Logger logger;

  Config config = decode_options(argc, argv);
  logger.setup(config);

  std::unique_ptr<Base> sim = get_simulation(config, logger, config.get<std::string>("simulation name"));

  sim->run();

  return 0;
}
