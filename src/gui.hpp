#pragma once

#include "ofxGui.h"
#include <string>
#include "palette.hpp"

class Gui {
public:
  static Gui& getInstance() {
    static Gui instance;
    return instance;
  }
  Gui(Gui const&) = delete;
  void operator=(Gui const&) = delete;
private:
  Gui();

public:
  void toggleShow();
  void draw();
  
  ofxLabel performance;
  
  ofxIntSlider maxAddedParticles;
  ofxIntSlider fadeDelay;
  ofxToggle showVideo;
  
  ofxToggle drawTrails;
  ofxToggle drawConnections;
  
  ofxGuiGroup particleGroup;
  ofxIntSlider particleMaxAge;
  ofxFloatSlider particleVelocity;
  ofxFloatSlider particleAcceleration;
  ofxFloatSlider particleAccelerationDamping;
  ofxFloatSlider particleSpin;
  ofxIntSlider particleRadius;
  ofxIntSlider lineWidth;
  
  ofxGuiGroup colorGroup;
  ofParameter<ofColor> backgroundColor;
  ofxToggle colorFromVideo;
  ofxToggle fadeWithAge;
  ofxInputField<std::string> palette1Url;
  Palette palette1 {""};
  
private:
  bool show = true;
  ofxPanel panel;
  void palette1UrlChanged(std::string& url);
};
