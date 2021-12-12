#pragma once

#include "../base.hpp"

class Plane : public Base {
 public:
  colonio::Pubsub2D* ps2;
  int64_t last_update_target;
  double current_x;
  double current_y;
  double speed;
  double ang;

  Plane(colonio::Colonio& c, const Config& config, Logger& logger);

  void setup() override;
  void step() override;
};
