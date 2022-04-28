#include "gui.hpp"

Gui::Gui() {
  panel.setup("Gestural");
  panel.disableHeader();
  panel.setDefaultWidth(300);
  panel.add(performance.setup("", ""));
  
  panel.add(maxAddedParticles.setup("New marks", 100, 0, 1000));
  panel.add(fadeDelay.setup("Fade delay", 10, 1, 100));
  panel.add(showVideo.setup("Show video", true));
  
  particleGroup.setup("Marks");
  particleGroup.add(particleMaxAge.setup("Lifespan", 400, 50, 4000));
  particleGroup.add(particleVelocity.setup("Start speed", 0, 0, 10));
  particleGroup.add(particleAcceleration.setup("Start acceleration", 0.05, 0, 1));
  particleGroup.add(particleAccelerationDamping.setup("Damping", 10, 1, 100));
  particleGroup.add(particleSpin.setup("Spin", 0, 2, 10));
  particleGroup.add(particleRadius.setup("Influence", 50, 0, 500));
  panel.add(&particleGroup);
  
  palette1Url.addListener(this, &Gui::palette1UrlChanged);
  panel.add(palette1Url.setup("coolers.co URL 1", "https://coolors.co/palette/000000-14213d-fca311-e5e5e5-ffffff"));
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
