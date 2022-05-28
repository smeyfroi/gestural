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
  static void eraseDeadParticles();
  static void updateParticles();
  static size_t particleCount();
  static void disruptParticles(float amount, float variation);
  static void add(float amount, float variation);
  static void reduce(float amount, float variation);
  Particle(float x, float y, ofColor videoColor, ofColor paletteColor);
  void update();
  bool isDead() const;
  void draw();
  void disrupt(float amount, float variation);

private:
  static std::vector<Particle> particles;
  static std::vector<ofVec2f> points;
  static unique_ptr<ofx::KDTree<ofVec2f>> spatialIndexPtr;
  static void createSpatialIndex();
  const ofVec2f createForce(ofVec2f target, float attraction, float influence);
  int edgeDistanceH() const;
  int edgeDistanceW() const;
  float edgeProximityH() const;
  float edgeProximityW() const;
  int edgeDistanceV() const;
  void setMarkColor(ofColor c, float alpha) const;
  ofVec2f lastDrawnPosition;
  ofVec2f position;
  ofVec2f velocity;
  float spin;
  float radius;
  int age;
  ofColor videoColor;
  ofColor paletteColor;
};
