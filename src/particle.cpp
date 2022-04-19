#include "particle.hpp"

const int MAX_AGE = 8000;
extern const size_t canvasWidth = 72*64;
extern const size_t canvasHeight = 72*48;

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
velocity(0.2, 0.0),
acceleration(0.05, 0.0),
radius(160.0),
age(0)
{
  velocity.rotate(ofRandom(360.0));
  acceleration.rotate(ofRandom(360.0));
}

void Particle::update() {
  velocity += acceleration;
  position += velocity;
  age++;
}

bool Particle::isDead() const {
  return (age > MAX_AGE || position.x+radius < 0 || position.y+radius < 0 || position.x-radius > canvasWidth || position.y-radius > canvasHeight);
}

void Particle::draw() const {
  if (spatialIndexPtr->kdtree_get_point_count() == 0) return;
  ofPushView();
  ofx::KDTree<ofVec2f>::SearchResults searchResults(10);
  const float searchRadius = float(radius);
  spatialIndexPtr->findPointsWithinRadius(position, searchRadius, searchResults);
  for (const auto& searchResult: searchResults) {
    size_t i = searchResult.first;
    float distanceSquared = searchResult.second;
    float distanceScale = (1.0 - (sqrt(distanceSquared)/searchRadius));
//    ofLogNotice()<<distanceScale<<"   "<<distanceSquared<<"  "<<(searchRadius*searchRadius);
    ofColor color = ofColor(255.0*distanceScale*distanceScale);
    ofSetColor(color);
    Particle& otherParticle = particles[i];
    ofDrawLine(position, otherParticle.position);
  }
  ofPopView();
}
