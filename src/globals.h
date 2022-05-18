#pragma once
#include "ofColor.h"

namespace Constants {
  // Note that camera captures 640x480 not 720x480
  const size_t canvasWidth = 240*36;
  const size_t canvasHeight = 240*24;
  const size_t resizedVideoWidth = 720;
  const size_t resizedVideoHeight = 480;
  const size_t videoGrabWidth = 640;
  const size_t videoGrabHeight = 480;
}

enum class ParticleDisruption {
  angleAbs,
  speedAbs,
  accelerationAngleAbs,
  spinAbs,
  radiusAbs,
};
