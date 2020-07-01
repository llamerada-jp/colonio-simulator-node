
#include <getopt.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "base.hpp"
#include "sphere/sphere.hpp"

option options[] = {
    {"help", no_argument, nullptr, 'h'},
    {"seed-url", required_argument, nullptr, 's'},
    {0, 0, 0, 0},
};

void print_help(std::ostream& out) {
  out << std::endl;
  out << "`simulations` is a implement some simulations for colonio." << std::endl << std::endl;
  out << "Usage:" << std::endl;
  out << "  simulations [Flags] <simulation name>" << std::endl << std::endl;
  out << "Simulation names:" << std::endl;
  out << "  sphere" << std::endl << std::endl;
  out << "Flags:" << std::endl;
  out << "  -h, --help                help for simulations" << std::endl;
  out << "  -s, --seed-url string     seed server URL (default is ws://localhost)" << std::endl;
}

void decode_options(int argc, char* argv[], Options* op) {
  op->seed_url = "ws://localhost";

  int opt;
  int idx_long = 0;
  while ((opt = getopt_long(argc, argv, "hs:", options, &idx_long)) != -1) {
    switch (opt) {
      case 'h':
        print_help(std::cout);
        exit(EXIT_SUCCESS);
        break;

      case 's':
        op->seed_url = std::string(optarg);
        break;

      default:
        print_help(std::cerr);
        exit(EXIT_FAILURE);
    }
  }

  if (optind + 1 == argc) {
    op->simulation_name = std::string(argv[optind]);
  } else {
    std::cerr << argv[0] << ": simulation name is required" << std::endl;
    print_help(std::cerr);
    exit(EXIT_FAILURE);
  }
}

std::unique_ptr<Base> get_simulation(const std::string& name) {
  std::unique_ptr<Base> sim;

  if (name == "sphere") {
    sim = std::make_unique<Sphere>();
  }

  if (!sim) {
    std::cerr << "wrong simulation name" << std::endl;
    exit(EXIT_FAILURE);
  }

  return std::move(sim);
}

int main(int argc, char* argv[]) {
  Options op;

  decode_options(argc, argv, &op);
  std::unique_ptr<Base> sim = get_simulation(op.simulation_name);

  sim->run(op);

  return 0;
}
