#include "particle.hpp"

std::vector<Particle> Particle::particles;

void Particle::makeParticle(float x, float y) {
  particles.push_back(Particle(x, y));
}

void Particle::drawParticles() {
  for (const auto& particle : particles) {
    particle.draw();
  }
}

bool particleIsDead(const Particle& particle) {
  return particle.isDead();
}

void Particle::updateParticles() {
  for (auto& particle : particles) {
    particle.update();
  }
  
  std::remove_if(particles.begin(), particles.end(), particleIsDead);
}

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
