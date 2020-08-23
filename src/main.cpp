
#include <getopt.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "base.hpp"
#include "config.hpp"
#include "logger.hpp"
#include "mongo.hpp"
#include "plane/plane.hpp"
#include "sphere/sphere.hpp"

Mongo mongo;
int parallel;
bool enable_log_mongodb;
bool enable_log_stdout;

option options[] = {
    {"enable_mongodb", no_argument, nullptr, 'm'},
    {"enable_stdout", no_argument, nullptr, 's'},
    {"help", no_argument, nullptr, 'h'},
    {"parallel", required_argument, nullptr, 'p'},
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
  parallel           = 1;
  enable_log_mongodb = false;
  enable_log_stdout  = false;

  int opt;
  int idx_long = 0;
  while ((opt = getopt_long(argc, argv, "hmp:s", options, &idx_long)) != -1) {
    switch (opt) {
      case 'h':
        print_help(std::cout);
        exit(EXIT_SUCCESS);
        break;

      case 'm':
        enable_log_mongodb = true;
        break;

      case 'p':
        parallel = std::atoi(optarg);
        if (parallel < 1) {
          parallel = 1;
        }
        break;

      case 's':
        enable_log_stdout = true;
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
  } else if (name == "plane") {
    sim = std::make_unique<Plane>(config, logger);
  }

  if (!sim) {
    std::cerr << "wrong simulation name" << std::endl;
    exit(EXIT_FAILURE);
  }

  return std::move(sim);
}

void run(const Config& config) {
  Logger logger;
  logger.setup(enable_log_stdout);

  if (enable_log_mongodb) {
    logger.set_mongo(mongo);
  }

  std::shared_ptr<Base> sim = get_simulation(config, logger, config.get<std::string>("simulation name"));
  logger.get_local_nid      = [sim]() { return sim->get_local_nid(); };

  sim->run();
}

int main(int argc, char* argv[]) {
  Config config = decode_options(argc, argv);

  if (enable_log_mongodb) {
    mongo.setup(config);
  }

  if (parallel == 1) {
    run(config);

  } else {
    std::vector<std::thread> threads(parallel);

    for (int idx = 0; idx < parallel; idx++) {
      sleep(1);
      std::thread new_thread(run, config);
      threads[idx].swap(new_thread);
    }

    for (int idx = 0; idx < parallel; idx++) {
      threads[idx].join();
    }
  }
  return 0;
}
