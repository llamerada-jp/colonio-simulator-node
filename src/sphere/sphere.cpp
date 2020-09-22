#include "sphere.hpp"

#include <iostream>
#include <random>
#include <string>

#include "../config.hpp"

static std::random_device rd;
static std::mt19937 mt(rd());
static std::uniform_real_distribution<double> rand_ang(-M_PI, M_PI);
// static std::uniform_real_distribution<double> rand_speed(0, 2 * M_PI * 27.77 / 40075000);
static std::uniform_real_distribution<double> rand_speed(0, 1.0 / 100.0);

Sphere::Sphere(const Config& config, Logger& logger) : Base(config, logger, 1000) {
}

void Sphere::setup() {
  connect(config.get<std::string>("seed.url", "ws://localhost"), config.get<std::string>("seed.token", ""));

  current_x          = std::uniform_real_distribution<double>(-M_PI, M_PI)(mt);
  current_y          = std::uniform_real_distribution<double>(-M_PI / 2, M_PI / 2)(mt);
  ang                = rand_ang(mt);
  speed              = rand_speed(mt);
  last_update_target = get_current_msec() - std::uniform_int_distribution<int>(0, 60 * 1000)(mt);

  ps2 = &access_pubsub_2d("ps2");
}

void Sphere::step() {
  // update position
  current_x += cos(ang) * speed;
  current_y += sin(ang) * speed;
  if (current_y < -M_PI / 2) {
    current_y += M_PI;
    current_x += M_PI;
  }
  if (M_PI / 2 <= current_y) {
    current_y = M_PI - current_y;
    current_x += M_PI;
  }
  while (current_x < -M_PI) {
    current_x += 2 * M_PI;
  }
  while (M_PI <= current_x) {
    current_x -= 2 * M_PI;
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
  ps2->publish("ch", current_x, current_y, 2 * M_PI * 1000 / 40075000, message);
}
