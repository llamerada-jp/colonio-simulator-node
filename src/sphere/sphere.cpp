#include "sphere.hpp"

#include <iostream>

Sphere::Sphere() : Base(100) {
}

void Sphere::setup() {
  connect(options.seed_url, options.seed_token);

  colonio::Pubsub2D& ps2 = access_pubsub_2d("ps2");
}

void Sphere::step() {
  std::cout << "hello" << std::endl;
}
