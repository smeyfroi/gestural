#include "particle.hpp"

Particle::Particle(float x, float y) :
position(x, y),
velocity(1, 0),
radius(10.0)
{
  velocity.rotate(ofRandom(360.0));
}

void Particle::update() {
  position += velocity;
}

bool Particle::isDead() const {
  return (position.x+radius < 0 || position.y+radius < 0 || position.x-radius > ofGetWindowWidth() || position.y-radius > ofGetWindowHeight());
}

void Particle::draw() const {
  ofPushView();
  ofNoFill();
  ofSetColor(ofColor::black);
  ofDrawCircle(position, radius);
  ofPopView();
}
