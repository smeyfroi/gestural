#include "particle.hpp"

const int MAX_AGE = 300;
extern const size_t canvasWidth = 72*37;
extern const size_t canvasHeight = 72*25;

std::vector<Particle> Particle::particles;
std::vector<ofVec2f> Particle::points;
unique_ptr<ofx::KDTree<ofVec2f>> Particle::spatialIndexPtr;

void Particle::makeParticle(float x, float y) {
  particles.push_back(Particle(x, y));
}

void Particle::drawParticles() {
  for (const auto& particle : particles) {
    particle.draw();
  }
}

void Particle::updateParticles() {
  for (auto& particle : particles) {
    particle.update();
  }
  
  if (ofGetFrameNum() % 25 != 0) return;
  
  // remove dead particles
  auto it = std::remove_if(particles.begin(), particles.end(), [](Particle p) { return p.isDead(); });
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
velocity(1, 0),
radius(15.0),
age(0)
{
  velocity.rotate(ofRandom(360.0));
}

void Particle::update() {
  position += velocity;
  age++;
}

bool Particle::isDead() const {
  return (age > MAX_AGE || position.x+radius < 0 || position.y+radius < 0 || position.x-radius > canvasWidth || position.y-radius > canvasHeight);
}

void Particle::draw() const {
  if (spatialIndexPtr->kdtree_get_point_count() == 0) return;
  ofPushView();
  ofNoFill();
//  ofSetColor(ofColor::black);
//  ofDrawCircle(position, radius);
  ofx::KDTree<ofVec2f>::SearchResults searchResults(10);
  spatialIndexPtr->findPointsWithinRadius(position, radius*radius, searchResults);
  for (const auto& searchResult: searchResults) {
    size_t i = searchResult.first;
    float distanceSquared = searchResult.second;
    ofSetColor(0, 0, 0, 1/distanceSquared*5000);
    ofDrawLine(position, particles[i].position);
  }
  ofPopView();
}
