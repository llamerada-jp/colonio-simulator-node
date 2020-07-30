#include "plane.hpp"

#include <iostream>
#include <random>
#include <string>

#include "../config.hpp"

static std::random_device rd;
static std::mt19937 mt(rd());
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
  while (1.0 <= current_x) {
    current_x -= 2.0;
  }
  while (current_x < -1.0) {
    current_x += 2.0;
  }
  while (1.0 <= current_y) {
    current_y -= 2.0;
  }
  while (current_y < -1.0) {
    current_y += 2.0;
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
