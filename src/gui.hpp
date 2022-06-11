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
  
  ofxGuiGroup keysGroup;
  ofxLabel sKey;
  ofxLabel gKey;
  ofxLabel rsqbrKey;
  ofxLabel lsqbrKey;
  ofxLabel spaceKey;
  ofxLabel tabKey;
  ofxLabel dotKey;
  ofxLabel fKey;
  ofxLabel shiftKey;

  ofxGuiGroup videoGroup;
  ofParameter<void> loadVideoButton;
  std::string videoPath {""};
  ofEvent<std::string> videoPathChanged;
  ofxToggle showVideo;

  ofxGuiGroup markTypesGroup;
  ofxIntSlider maxAddedParticles;
  ofxToggle drawTrails;
  ofParameter<bool> completeTrails;
  ofxToggle drawConnections;
  ofxIntSlider lineWidth;

  ofxGuiGroup particleGroup;
  ofParameter<float> mouseAttraction;
  ofxIntSlider particleMaxAge;
  ofxFloatSlider particleDamping;
  ofxFloatSlider particleSpin;
  ofxIntSlider particleInfluence;
  ofxFloatSlider particleRepulsion;
  
  ofxGuiGroup colorGroup;
  ofParameter<ofColor> backgroundColor;
  ofxFloatSlider intensity;
  ofParameter<float> intensityAtEdges;
  ofParameter<float> edgeWidth;
  ofxToggle fadeWithAge;
  ofxToggle fadeWithDistance;
  ofxToggle colorFromVideo;
  
  ofxInputField<std::string> palette1Url;
  Palette palette1 {""};
  ofxToggle mixColorFromPalette2;
  ofxInputField<std::string> palette2Url;
  Palette palette2 {""};
  
  // Instantanous disruptions of existing particles
  ofxGuiGroup disruptionGroup;
  ofParameter<float> disruptionAmount; // 0-1, per disruption semantics
  ofParameter<float> disruptionVariation; // 0-1, applied to raw amount, per disruption semantics
  ofParameter<void> disruptCurrent;
  ofParameter<void> disruptAdd;
  ofParameter<void> disruptReduce;
  
private:
  bool show = true;
  ofxPanel panel;
  void loadVideo();
  void palette1UrlChanged(std::string& url);
  void palette2UrlChanged(std::string& url);
};
