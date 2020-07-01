#pragma once

#include "../base.hpp"

class Sphere : public Base {
 public:
  Sphere();

  void setup() override;
  void step() override;
};
