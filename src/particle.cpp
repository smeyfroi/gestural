#include "particle.hpp"
#include "globals.h"
#include "gui.hpp"

std::vector<Particle> Particle::particles;
std::vector<ofVec2f> Particle::points;
unique_ptr<ofx::KDTree<ofVec2f>> Particle::spatialIndexPtr;

void Particle::makeParticle(float x, float y) {
  particles.push_back(Particle(x, y));
}

void Particle::drawParticles() {
  for (auto& particle : particles) {
    particle.draw();
  }
}

void Particle::updateParticles() {
  for (auto& particle : particles) {
    particle.update();
  }
  
  if (ofGetFrameNum() % 30 != 0) return;
  
  // remove dead particles
  auto it = std::remove_if(particles.begin(), particles.end(), [](Particle& p) { return p.isDead(); });
  auto r = std::distance(it, particles.end());
  particles.erase(it, particles.end());
  
  // rebuild spatial index
  points.clear();
  points.reserve(particles.size());
  std::transform(particles.begin(), particles.end(), std::back_inserter(points), [](Particle& p) -> ofVec2f { return p.position; });
  Particle::spatialIndexPtr = make_unique<ofx::KDTree<ofVec2f>>(points);
}

size_t Particle::particleCount() {
  return particles.size();
}

Particle::Particle(float x, float y) :
position(x, y),
velocity(Gui::getInstance().particleVelocity, 0.0),
acceleration(Gui::getInstance().particleAcceleration, 0.0),
spin(Gui::getInstance().particleSpin),
radius(Gui::getInstance().particleRadius),
age(0)
{
  velocity.rotate(ofRandom(360.0));
  acceleration.rotate(ofRandom(360.0));
}

void Particle::update() {
  velocity += acceleration;
  velocity.rotate(spin);
  position += velocity;
  age++;
}

bool Particle::isDead() const {
  return (age > Gui::getInstance().particleMaxAge || position.x+radius < 0 || position.y+radius < 0 || position.x-radius > Constants::canvasWidth || position.y-radius > Constants::canvasHeight);
}

void Particle::draw() {
//  ofNoFill();
//  ofSetColor(0, 0, 0, 32);
//  ofDrawCircle(position.x, position.y, radius);
  
//  ofSetColor(0, 0, 0, 32);
//  ofFill();
//  ofDrawCircle(position.x, position.y, 1);
  
  if (spatialIndexPtr->kdtree_get_point_count() == 0) return;
  
  ofx::KDTree<ofVec2f>::SearchResults searchResults(50);
  const float searchRadius = float(radius);
  spatialIndexPtr->findPointsWithinRadius(position, searchRadius, searchResults);
  if (searchResults.size() == 0) return;
  
  ofPushView();
  int count = 0;
  ofVec2f centroid;
  for (const auto& searchResult: searchResults) {
    float distanceSquared = searchResult.second;
    size_t i = searchResult.first;
    Particle& otherParticle = particles[i];
    if (position == otherParticle.position) continue;
    
    centroid += otherParticle.position;
    ++count;
    
    float distanceScale = 1-(distanceSquared/(searchRadius*searchRadius));
    ofColor color = Gui::getInstance().palette1.getInterpolated(distanceScale);
    ofSetColor(color);
    
    if (distanceSquared > 1.0) {
      ofFill();
      ofSetLineWidth(0);
      ofDrawCircle(position, Gui::getInstance().lineWidth/2.0-2.0);
      ofDrawCircle(otherParticle.position, Gui::getInstance().lineWidth/2.0-2.0);
    }
    ofSetLineWidth(Gui::getInstance().lineWidth);
    ofDrawLine(position, otherParticle.position);
  }
  
  centroid /= count;
  acceleration += (position-centroid).normalize()/Gui::getInstance().particleAccelerationDamping;
  acceleration = acceleration.normalize()/Gui::getInstance().particleAccelerationDamping;
  ofPopView();
}
