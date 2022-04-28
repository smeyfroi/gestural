#pragma once

#include "ofColor.h"
#include <vector>

class Palette {
public:
  Palette(const std::string& coolerUrl);
  ofColor getInterpolated(float amount);
private:
  std::vector<ofColor> colors;
};
