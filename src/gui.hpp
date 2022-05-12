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
  void save();
  void load();

  ofxLabel performance;
  
  ofxGuiGroup videoGroup;
  ofParameter<void> loadVideoButton;
  std::string videoPath {""};
  ofEvent<std::string> videoPathChanged;
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
  ofxFloatSlider intensity;
  ofxToggle fadeWithAge;
  ofxToggle fadeWithDistance;
  ofxToggle colorFromVideo;
  ofxInputField<std::string> palette1Url;
  Palette palette1 {""};
  ofxToggle mixColorFromPalette2;
  ofxInputField<std::string> palette2Url;
  Palette palette2 {""};
  
private:
  bool show = true;
  ofxPanel panel;
  void loadVideo();
  void palette1UrlChanged(std::string& url);
  void palette2UrlChanged(std::string& url);
};
