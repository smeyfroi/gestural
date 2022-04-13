#pragma once

#include "ofMain.h"

class Particle {
public:
  Particle(float x, float y);
  void update();
  bool isDead() const;
  void draw() const;
  
private:
  ofVec2f position;
  ofVec2f velocity;
  float radius;
};
