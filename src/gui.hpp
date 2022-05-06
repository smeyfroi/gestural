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
  
  ofxGuiGroup videoGroup;
  ofParameter<void> loadVideoButton;
  ofxToggle showVideo;

  ofxIntSlider maxAddedParticles;
  ofxIntSlider fadeDelay;
  
  ofxGuiGroup markTypesGroup;
  ofxToggle drawTrails;
  ofxToggle drawConnections;
  
  ofxGuiGroup particleGroup;
  ofxIntSlider particleMaxAge;
  ofxFloatSlider particleVelocity;
  ofxFloatSlider particleAcceleration;
  ofxFloatSlider particleSpin;
  ofxIntSlider particleInfluence;
  ofxFloatSlider particleRepulsion;
  ofxIntSlider lineWidth;
  
  ofxGuiGroup colorGroup;
  ofParameter<ofColor> backgroundColor;
  ofxToggle colorFromVideo;
  ofxFloatSlider intensity;
  ofxToggle fadeWithAge;
  ofxToggle fadeWithDistance;
  ofxInputField<std::string> palette1Url;
  Palette palette1 {""};
  std::string videoPath {""};
  ofEvent<std::string> videoPathChanged;
  
private:
  bool show = true;
  ofxPanel panel;
  void loadVideo();
  void palette1UrlChanged(std::string& url);
};
