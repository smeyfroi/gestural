#pragma once

#include "ofxGui.h"

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
  ofxIntSlider maxAddedParticles;
  ofxIntSlider fadeDelay;
  ofxToggle showVideo;
  ofxLabel performance;
  ofxGuiGroup particleGroup;
  ofxIntSlider particleMaxAge;
  ofxFloatSlider particleVelocity;
  ofxFloatSlider particleAcceleration;
  ofxIntSlider particleSpin;
  ofxIntSlider particleRadius;
  
private:
  bool show = true;
  ofxPanel panel;
};
