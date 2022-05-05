#include "gui.hpp"
#include "ofSystemUtils.h"

Gui::Gui() {
  panel.setup("Gestural");
  panel.disableHeader();
  panel.setDefaultWidth(300);
  panel.add(performance.setup("", ""));
  
  panel.add(maxAddedParticles.setup("New marks", 200, 0, 1000));
  panel.add(fadeDelay.setup("Fade delay", 10, 1, 100));
  loadVideoButton.addListener(this, &Gui::loadVideo);
  panel.add(loadVideoButton.set("Load video file"));
  panel.add(showVideo.setup("Show video", true));

  panel.add(drawTrails.setup("Draw trails", true));
  panel.add(drawConnections.setup("Draw connections", false));
  
  particleGroup.setup("Marks");
  particleGroup.add(particleMaxAge.setup("Lifespan", 400, 50, 4000));
  particleGroup.add(particleVelocity.setup("Start speed", 1.0, 0, 10));
  particleGroup.add(particleAcceleration.setup("Start acceleration", 0.00, 0, 1));
  particleGroup.add(particleSpin.setup("Spin", 0.0, -5.0, 5.0));
  particleGroup.add(lineWidth.setup("Line width", 6, 1, 20));
//  panel.add(&particleGroup);
  
  
  particleGroup.add(particleAccelerationDamping.setup("Damping", 10, 1, 20));
  particleGroup.add(particleRadius.setup("Influence", 200, 0, 500));
  panel.add(&particleGroup);
  
  colorGroup.setup("Colours");
  colorGroup.add(backgroundColor.set("Background", ofColor(255, 255), ofColor(0, 255), ofColor(255, 255)));
  colorGroup.add(colorFromVideo.setup("Use video colours", true));
  colorGroup.add(fadeWithAge.setup("Fade with age", true));
  colorGroup.add(fadeWithDistance.setup("Fade with distance", false));
  palette1Url.addListener(this, &Gui::palette1UrlChanged);
  colorGroup.add(palette1Url.setup("coolors.co URL", "https://coolors.co/palette/001219-005f73-0a9396-94d2bd-e9d8a6-ee9b00-ca6702-bb3e03-ae2012-9b2226"));
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

void Gui::toggleShow() {
  show = !show;
}

void Gui::draw() {
  if (show) { panel.draw(); }
}
