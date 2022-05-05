#pragma once

#include "ofMain.h"
#include <vector>
#include <memory.h>
#include "ofxSpatialHash.h"
#include "ofColor.h"

class Particle {
public:
  static void makeParticle(float x, float y, ofColor videoColor, ofColor paletteColor);
  static void drawParticles();
  static void updateParticles();
  static size_t particleCount();
  Particle(float x, float y, ofColor videoColor, ofColor paletteColor);
  void update();
  bool isDead() const;
  void draw() const;
  
private:
  static std::vector<Particle> particles;
  static std::vector<ofVec2f> points;
  static unique_ptr<ofx::KDTree<ofVec2f>> spatialIndexPtr;
  ofVec2f position;
  ofVec2f velocity;
  ofVec2f acceleration;
  float spin;
  float radius;
  int age;
  ofColor videoColor;
  ofColor paletteColor;
};
