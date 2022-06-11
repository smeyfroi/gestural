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
  std::for_each(particles.begin(), particles.end(), [](Particle& p){ p.draw(); });
}

// Run this with createSpatialIndex to ensure indices are kept in sync between `particles` and the index
void Particle::eraseDeadParticles() {
  auto it = std::remove_if(particles.begin(), particles.end(), [](Particle& p) { return p.isDead(); });
  particles.erase(it, particles.end());
}

// Run this with createSpatialIndex to ensure indices are kept in sync between `particles` and the index
void Particle::createSpatialIndex() {
  points.clear();
  points.reserve(particles.size());
  std::transform(particles.begin(), particles.end(), std::back_inserter(points), [](Particle& p) -> ofVec2f { return p.position; });
  Particle::spatialIndexPtr = make_unique<ofx::KDTree<ofVec2f>>(points);
}

void Particle::updateParticles() {
  std::for_each(particles.begin(), particles.end(), [](Particle& p){ p.update(); });
  if (ofGetFrameNum() % 20 != 0) return;
  if (ofGetFrameRate() > 15) {
    eraseDeadParticles();
    createSpatialIndex();
  }
}

size_t Particle::particleCount() {
  return particles.size();
}

// amount is 0.0-1.0
void Particle::disruptParticles(float amount, float variation) {
  std::for_each(particles.begin(), particles.end(), [=](Particle& p){ p.disrupt(amount, variation); });
}

float createRandomizedAmount(float amount, float variation) {
  float a = amount+ofRandom(variation) - (amount+variation)/2.0;
  return std::max(0.0f, a);
}

void Particle::add(float amount, float variation) {
  float num = createRandomizedAmount(amount, variation) * (float)particles.size();
  std::set<size_t> chosenIndexes;
  for (int i=0; i<num || chosenIndexes.size()==particles.size(); ++i) {
    chosenIndexes.insert(ofRandom(particles.size()));
  }
  for (const auto& i: chosenIndexes) {
    Particle p = particles[i];
    p.disrupt(Gui::getInstance().disruptionAmount, Gui::getInstance().disruptionVariation);
    particles.push_back(p);
  }
  createSpatialIndex();
}

void Particle::reduce(float amount, float variation) {
  float num = createRandomizedAmount(amount, variation);
  float probability = std::min(1.0f, num);
  auto it = std::remove_if(particles.begin(), particles.end(), [=](Particle& p) { return ofRandomf() < probability; });
  auto r = std::distance(it, particles.end());
  particles.erase(it, particles.end());
  createSpatialIndex();
}

Particle::Particle(float x, float y, ofColor videoColor_, ofColor paletteColor_) :
lastDrawnPosition(x, y),
position(x, y),
velocity(0.5, 0.0),
spin(Gui::getInstance().particleSpin),
radius(Gui::getInstance().particleInfluence),
age(0),
videoColor(videoColor_),
paletteColor(paletteColor_)
{
  velocity.rotate(ofRandom(360.0));
}

float step(float edge, float t) {
  return (float) (t >= edge);
}

const ofVec2f Particle::createForce(ofVec2f target, float attraction, float influence) {
  ofVec2f direction = target - position;
  float distance = direction.length();
  float forceMagnitude = attraction * (1.0 - distance/influence);
  return step(distance, influence) * forceMagnitude * direction.getNormalized();
}

void Particle::update() {
  if (isDead()) return;
  
  ofx::KDTree<ofVec2f>::SearchResults searchResults(20);
  spatialIndexPtr->findPointsWithinRadius(position, radius, searchResults);
  
  int count = 0;
  ofVec2f centroid;
  for (const auto& searchResult: searchResults) {
    const Particle& otherParticle = particles[searchResult.first];
    if (position == otherParticle.position) continue;
    centroid += otherParticle.position;
    ++count;
  }
  if (count != 0) velocity += createForce(centroid/count, Gui::getInstance().particleRepulsion, radius);
  
  if (!ofGetKeyPressed(OF_KEY_SHIFT)) {
    velocity += createForce(ofVec2f(ofGetMouseX()/Globals::screenToCanvasWidthScale, ofGetMouseY()/Globals::screenToCanvasHeightScale), Gui::getInstance().mouseAttraction, ofGetWindowWidth()/3);
  }
  
  velocity.rotate(spin);
  velocity *= Gui::getInstance().particleDamping;
  position += velocity;
  age++;
}

bool Particle::isDead() const {
  return (age > Gui::getInstance().particleMaxAge || position.x+radius < 0 || position.y+radius < 0 || position.x-radius > Constants::canvasWidth || position.y-radius > Constants::canvasHeight);
}

int Particle::edgeDistanceW() const {
  return std::min(std::abs(position.x), std::abs(Constants::canvasWidth-position.x));
}

int Particle::edgeDistanceH() const {
  return std::min(std::abs(position.y), std::abs(Constants::canvasHeight-position.y));
}

// 0 at edge, 1 not at edge
float Particle::edgeProximityW() const {
  int edgeMargin = float(Gui::getInstance().edgeWidth)*Constants::canvasWidth/2.0;
  return std::min((float)1.0, float(edgeDistanceW())/edgeMargin);
}

// 0 at edge, 1 not at edge
float Particle::edgeProximityH() const {
  int edgeMargin = float(Gui::getInstance().edgeWidth)*Constants::canvasHeight/2.0;
  return std::min((float)1.0, float(edgeDistanceH())/edgeMargin);
}

// alpha 0.0-1.0
void Particle::setMarkColor(ofColor c, float alpha) const {
  alpha = std::max(alpha*edgeProximityH()*edgeProximityW() + ofRandom(-0.1, 0.1), (float)Gui::getInstance().intensityAtEdges);
  
  if (Gui::getInstance().fadeWithAge) {
    float maxAge = Gui::getInstance().particleMaxAge;
    alpha *= (maxAge - age) / maxAge;
  }
  c.a = 255.0 * alpha * Gui::getInstance().intensity;
  ofSetColor(c);
}

void Particle::draw() {
  if (isDead()) return;
  
  ofPushView();
  ofEnableAlphaBlending();
  ofBlendMode(OF_BLENDMODE_MULTIPLY);

  ofx::KDTree<ofVec2f>::SearchResults searchResults(20);
  const float searchRadius = float(radius);
  spatialIndexPtr->findPointsWithinRadius(position, searchRadius, searchResults);

  // find alpha except when drawConnection and fadeWithDistance (that's calculated per connection)
  float alpha = 1.0; // keep this 0.0-1.0 until it gets applied
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
    if (Gui::getInstance().completeTrails) {
      ofSetLineWidth(Gui::getInstance().lineWidth);
      ofDrawLine(lastDrawnPosition, position);
    } else {
      ofFill();
      ofDrawCircle(position.x, position.y, Gui::getInstance().lineWidth);
    }
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
 
  lastDrawnPosition = position;
  ofPopView();
}

// amount is 0.0-1.0
void Particle::disrupt(float amount, float variation) {
  float randomizedAmount = createRandomizedAmount(amount, variation);
  velocity.rotate(randomizedAmount*360.0);
  velocity *= randomizedAmount * 5.0;
  spin *= amount;
  radius *= amount;
}
