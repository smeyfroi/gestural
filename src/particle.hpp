#pragma once

#include "ofMain.h"
#include <vector>
#include <memory.h>
#include "ofxSpatialHash.h"
#include "ofColor.h"
#include "globals.h"

class Particle {
public:
  static void makeParticle(float x, float y, ofColor videoColor, ofColor paletteColor);
  static void drawParticles();
  static void updateParticles();
  static size_t particleCount();
  static void disruptParticles(ParticleDisruption disruption, float amount, float variation);
  static void add(float amount, float variation);
  static void reduce(float amount, float variation);
  Particle(float x, float y, ofColor videoColor, ofColor paletteColor);
  void update();
  bool isDead() const;
  void draw() const;
  void disrupt(float amount, float variation);
  void disrupt(ParticleDisruption disruption, float amount, float variation);

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
