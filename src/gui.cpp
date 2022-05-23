#include "gui.hpp"
#include "ofSystemUtils.h"

Gui::Gui() {
  panel.setup("Gestural");
  panel.disableHeader();
  panel.setDefaultWidth(300);
  panel.add(performance.setup("", ""));
  
  keysGroup.setup("Keys");
  keysGroup.add(sKey.setup("s", "Save image"));
  keysGroup.add(gKey.setup("g", "Toggle controls"));
  keysGroup.add(rsqbrKey.setup("[", "Save control settings"));
  keysGroup.add(lsqbrKey.setup("]", "Load control settings"));
  keysGroup.add(spaceKey.setup("space", "Pause drawing"));
  keysGroup.add(dotKey.setup(".", "Add permanent"));
  keysGroup.add(fKey.setup("f", "Fade permanent"));
  panel.add(&keysGroup);

  videoGroup.setup("Video");
  loadVideoButton.addListener(this, &Gui::loadVideo);
  videoGroup.add(loadVideoButton.set("Load video file"));
  videoGroup.add(showVideo.setup("Show video", true));
  panel.add(&videoGroup);
  
  markTypesGroup.setup("Mark types");
  markTypesGroup.add(maxAddedParticles.setup("New marks", 300, 0, 1000));
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
  colorGroup.add(intensity.setup("Colour intensity", 1.0, 0.0, 1.0));
  colorGroup.add(fadeWithAge.setup("Fade with age", true));
  colorGroup.add(fadeWithDistance.setup("Fade with distance", false));
  palette1Url.addListener(this, &Gui::palette1UrlChanged);
  colorGroup.add(palette1Url.setup("coolors.co URL", "https://coolors.co/palette/8ecae6-219ebc-023047-ffb703-fb8500"));
  colorGroup.add(mixColorFromPalette2.setup("Mix colour with 2y", false));
  palette2Url.addListener(this, &Gui::palette2UrlChanged);
  colorGroup.add(palette2Url.setup("2y coolors.co URL", "https://coolors.co/palette/dad7cd-a3b18a-588157-3a5a40-344e41"));
  panel.add(&colorGroup);
  
  disruptionGroup.setup("Disruptions");
  disruptionGroup.add(disruptionAmount.set("Amount", 0.50, 0.0, 1.0));
  disruptionGroup.add(disruptionVariation.set("Variation", 0.25, 0.0, 1.0));
  disruptionGroup.add(disruptCurrent.set("Disrupt"));
  disruptionGroup.add(disruptAdd.set("Add"));
  disruptionGroup.add(disruptReduce.set("Reduce"));
  panel.add(&disruptionGroup);

  panel.add(fadeAmount.setup("Fade amount", 32, 4, 128));
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

void Gui::save() {
  panel.saveToFile("settings.xml");
}

void Gui::load() {
  panel.loadFromFile("settings.xml");
}
