#pragma once

#include "ofMain.h"
#include <vector>
#include <memory.h>
#include "ofxSpatialHash.h"

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
  static std::vector<ofVec2f> points;
  static unique_ptr<ofx::KDTree<ofVec2f>> spatialIndexPtr;
  ofVec2f position;
  ofVec2f velocity;
  float radius;
  int age;
};
