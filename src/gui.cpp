#include "gui.hpp"
#include "ofSystemUtils.h"

Gui::Gui() {
  panel.setup("Gestural");
  panel.disableHeader();
  panel.setDefaultWidth(300);
  panel.add(performance.setup("", ""));

  videoGroup.setup("Video");
  loadVideoButton.addListener(this, &Gui::loadVideo);
  videoGroup.add(loadVideoButton.set("Load video file"));
  videoGroup.add(showVideo.setup("Show video", true));
  panel.add(&videoGroup);
  
  panel.add(maxAddedParticles.setup("New marks", 200, 0, 1000));
  panel.add(fadeDelay.setup("Fade delay", 10, 1, 100));

  markTypesGroup.setup("Mark types");
  markTypesGroup.add(drawTrails.setup("Draw trails", true));
  markTypesGroup.add(drawConnections.setup("Draw connections", false));
  panel.add(&markTypesGroup);
  
  particleGroup.setup("Marks");
  particleGroup.add(particleMaxAge.setup("Lifespan", 400, 50, 4000));
  particleGroup.add(particleVelocity.setup("Speed", 1.0, 0, 10));
  particleGroup.add(particleAcceleration.setup("Acceleration", 0.10, 0, 1));
  particleGroup.add(particleSpin.setup("Spin", 0.0, -5.0, 5.0));
  particleGroup.add(lineWidth.setup("Line width", 6, 1, 20));
  particleGroup.add(particleInfluence.setup("Influence", 200, 0, 1000));
  particleGroup.add(particleRepulsion.setup("Repulsion", 1.0, 0, 10.0));
  panel.add(&particleGroup);
  
  colorGroup.setup("Colours");
  colorGroup.add(backgroundColor.set("Background", ofColor(255, 255), ofColor(0, 255), ofColor(255, 255)));
  colorGroup.add(colorFromVideo.setup("Use video colours", true));
  colorGroup.add(intensity.setup("Colour intensity", 0.5, 0.0, 1.0));
  colorGroup.add(fadeWithAge.setup("Fade with age", true));
  colorGroup.add(fadeWithDistance.setup("Fade with distance", false));
  palette1Url.addListener(this, &Gui::palette1UrlChanged);
  colorGroup.add(palette1Url.setup("coolors.co URL", "https://coolors.co/palette/001219-005f73-0a9396-94d2bd-e9d8a6-ee9b00-ca6702-bb3e03-ae2012-9b2226"));
  colorGroup.add(mixColorFromPalette2.setup("Mix colour with extra palette", false));
  palette2Url.addListener(this, &Gui::palette2UrlChanged);
  colorGroup.add(palette2Url.setup("Extra coolors.co URL", "https://coolors.co/palette/0d3b66-faf0ca-f4d35e-ee964b-f95738"));
  panel.add(&colorGroup);
}

void Gui::loadVideo() {
  auto dialogResult = ofSystemLoadDialog("Load video", false, ofFilePath::getUserHomeDir());
  if (dialogResult.bSuccess) {
    auto ext = ofToUpper(ofFilePath::getFileExt(dialogResult.getPath()));
    if (ext == "MOV" || ext == "MP4") {
      videoPath = dialogResult.getPath();
      ofNotifyEvent(videoPathChanged, videoPath, this);
    } else {
      ofSystemAlertDialog("Videos must be '.mov' or '.mp4' files");
    }
  }
}

void Gui::palette1UrlChanged(std::string& url) {
  palette1 = Palette(url);
}

void Gui::palette2UrlChanged(std::string& url) {
  palette2 = Palette(url);
}

void Gui::toggleShow() {
  show = !show;
}

void Gui::draw() {
  if (show) { panel.draw(); }
}
