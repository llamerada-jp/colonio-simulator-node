#include "plane.hpp"

#include <iostream>
#include <random>
#include <string>

#include "../config.hpp"

int get_random_seed() {
  std::random_device seed_gen;
  int seed = seed_gen();
  // avoid WSL2 issue https://github.com/microsoft/WSL/issues/5767
  if (seed = 0xFFFFFFFF) {
    seed = static_cast<int>(clock());
  }
  return seed;
}

static std::mt19937 mt(get_random_seed());
static std::uniform_real_distribution<double> rand_ang(-M_PI, M_PI);
static std::uniform_real_distribution<double> rand_speed(0, 0.01);

Plane::Plane(const Config& config, Logger& logger) : Base(config, logger, 1000) {
}

void Plane::setup() {
  connect(config.get<std::string>("seed.url", "ws://localhost"), config.get<std::string>("seed.token", ""));
  current_x          = std::uniform_real_distribution<double>(-1.0, 1.0)(mt);
  current_y          = std::uniform_real_distribution<double>(-1.0, 1.0)(mt);
  ang                = rand_ang(mt);
  speed              = rand_speed(mt);
  last_update_target = get_current_msec() - std::uniform_int_distribution<int>(0, 60 * 1000)(mt);

  ps2 = &access_pubsub_2d("ps2");
}

void Plane::step() {
  // update position
  current_x += cos(ang) * speed;
  current_y += sin(ang) * speed;
  if (1.0 <= current_x) {
    current_x = 0.999;
    ang       = rand_ang(mt);
  }
  if (current_x < -1.0) {
    current_x = -1.0;
    ang       = rand_ang(mt);
  }
  if (1.0 <= current_y) {
    current_y = 0.999;
    ang       = rand_ang(mt);
  }
  while (current_y < -1.0) {
    current_y = -1.0;
    ang       = rand_ang(mt);
  }

  set_position(current_x, current_y);

  // update angle and speed
  if (get_current_msec() - last_update_target > 60 * 1000) {
    ang                = rand_ang(mt);
    speed              = rand_speed(mt);
    last_update_target = get_current_msec();
  }

  // send message
  colonio::Value message;
  message.set("message");
  ps2->publish("ch", current_x, current_y, 0.001, message);
}
