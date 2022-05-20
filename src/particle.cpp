#include "particle.hpp"
#include "globals.h"
#include "gui.hpp"
#include <algorithm>

std::vector<Particle> Particle::particles;
std::vector<ofVec2f> Particle::points;
unique_ptr<ofx::KDTree<ofVec2f>> Particle::spatialIndexPtr;

void Particle::makeParticle(float x, float y, ofColor videoColor, ofColor paletteColor) {
  particles.push_back(Particle(x, y, videoColor, paletteColor));
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
  
  if (ofGetFrameNum() % 15 != 0) return;
  
  // remove dead particles
  auto it = std::remove_if(particles.begin(), particles.end(), [](Particle& p) { return p.isDead(); });
  auto r = std::distance(it, particles.end());
  particles.erase(it, particles.end());
  
  // rebuild spatial index
  if (ofGetFrameRate() > 15) {
    points.clear();
    points.reserve(particles.size());
    std::transform(particles.begin(), particles.end(), std::back_inserter(points), [](Particle& p) -> ofVec2f { return p.position; });
    Particle::spatialIndexPtr = make_unique<ofx::KDTree<ofVec2f>>(points);
  }
}

size_t Particle::particleCount() {
  return particles.size();
}

// amount is 0.0-1.0
void Particle::disruptParticles(ParticleDisruption disruption, float amount, float variation) {
  for (auto& particle : particles) {
    particle.disrupt(disruption, amount, variation);
  }
}

Particle::Particle(float x, float y, ofColor videoColor_, ofColor paletteColor_) :
position(x, y),
velocity(Gui::getInstance().particleVelocity, 0.0),
acceleration(Gui::getInstance().particleAcceleration, 0.0),
spin(Gui::getInstance().particleSpin),
radius(Gui::getInstance().particleInfluence),
age(0),
videoColor(videoColor_),
paletteColor(paletteColor_)
{
  velocity.rotate(ofRandom(360.0));
  acceleration.rotate(ofRandom(360.0));
}

void Particle::update() {
  ofx::KDTree<ofVec2f>::SearchResults searchResults(50);
  spatialIndexPtr->findPointsWithinRadius(position, radius, searchResults);
  
  int count = 0;
  ofVec2f centroid;
  for (const auto& searchResult: searchResults) {
    const Particle& otherParticle = particles[searchResult.first];
    if (position == otherParticle.position) continue;
    centroid += otherParticle.position;
    ++count;
  }
  
  if (count != 0 && radius > 1.0) {
    centroid /= count;
    float distance = (centroid-position).length();
    float scale = (radius-distance)/radius;
    acceleration = (acceleration + (position-centroid)*scale*Gui::getInstance().particleRepulsion).normalize() * Gui::getInstance().particleAcceleration;
  }
  
  velocity.rotate(spin);
  acceleration.rotate(spin);
  velocity += acceleration;
  position += velocity;
  age++;
}

bool Particle::isDead() const {
  return (age > Gui::getInstance().particleMaxAge || position.x+radius < 0 || position.y+radius < 0 || position.x-radius > Constants::canvasWidth || position.y-radius > Constants::canvasHeight);
}

// alpha 0.0-1.0
void setMarkColor(ofColor c, float alpha) {
  c.a = 255.0*alpha*Gui::getInstance().intensity;
  ofSetColor(c);
}

void Particle::draw() const {
  ofPushView();

  ofx::KDTree<ofVec2f>::SearchResults searchResults(50);
  const float searchRadius = float(radius);
  spatialIndexPtr->findPointsWithinRadius(position, searchRadius, searchResults);

  // find alpha except when drawConnection and fadeWithDistance (that's calculated per connection)
  float alpha = 1.0; // keep this 0.0-1.0 until it gets applied
  if (Gui::getInstance().fadeWithAge) {
    float maxAge = Gui::getInstance().particleMaxAge;
    alpha *= (maxAge - age) / maxAge;
  }
  if (Gui::getInstance().drawTrails && Gui::getInstance().fadeWithDistance) {
    // 0.2 to 1.0 for 0 to 4 particles in its radius
    std::size_t m = 4;
    auto n = std::max(m, spatialIndexPtr->kdtree_get_point_count());
    alpha *= (1.0-0.2)/m*n+0.2;
  }

  if (Gui::getInstance().drawTrails) {
    if (Gui::getInstance().colorFromVideo) {
      setMarkColor(videoColor, alpha);
    } else {
      setMarkColor(paletteColor, alpha);
    }
    ofFill();
    ofDrawCircle(position.x, position.y, Gui::getInstance().lineWidth);
  }
  
  if (Gui::getInstance().drawConnections && searchRadius > 1.0) {
    for (const auto& searchResult: searchResults) {
      const Particle& otherParticle = particles[searchResult.first];
      if (position == otherParticle.position) continue;
      float distanceSquared = searchResult.second;
      float distanceScale = distanceSquared/(searchRadius*searchRadius); // 0 close, 1 far
      if (!Gui::getInstance().fadeWithDistance) {
        alpha *= (1.0-distanceScale);
      }
      if (Gui::getInstance().colorFromVideo) {
        setMarkColor(videoColor, alpha);
      } else {
        setMarkColor(paletteColor, alpha);
      }
      
      if (distanceSquared > 1.0) {
        ofFill();
        ofSetLineWidth(0);
        ofDrawCircle(position, Gui::getInstance().lineWidth/2.0-2.0);
        ofDrawCircle(otherParticle.position, Gui::getInstance().lineWidth/2.0-2.0);
      }
      ofSetLineWidth(Gui::getInstance().lineWidth);
      ofDrawLine(position, otherParticle.position);
    }
  }
 
  ofPopView();
}

// amount is 0.0-1.0
void Particle::disrupt(ParticleDisruption disruption, float amount, float variation) {
  float randomizedAmount = (amount+ofRandom(variation)) - (amount+variation)/2.0;
  switch(disruption) {
    case ParticleDisruption::angle: {
      velocity.rotate(randomizedAmount*360.0);
      break;
    }
    case ParticleDisruption::speed: {
      velocity *= randomizedAmount * 5.0;
      break;
    }
    case ParticleDisruption::accelerationAngle: {
      acceleration.rotate(randomizedAmount*360.0);
      break;
    }
    case ParticleDisruption::spin: {
      spin *= amount;
      break;
    }
    case ParticleDisruption::radius: {
      radius *= amount;
      break;
    }
    default:
      break;
  }
}
