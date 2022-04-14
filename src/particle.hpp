#pragma once

#include "ofMain.h"
#include <vector>
#include <memory.h>

class Particle {
public:
  static void makeParticle(float x, float y);
  static void drawParticles();
  static void updateParticles();
  Particle(float x, float y);
  void update();
  bool isDead() const;
  void draw() const;
  
private:
  static std::vector<Particle> particles;
  ofVec2f position;
  ofVec2f velocity;
  float radius;
};
