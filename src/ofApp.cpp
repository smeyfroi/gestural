#include "ofApp.h"
#include "particle.hpp"
#include "globals.h"
#include "ofFileUtils.h"
#include "ofUtils.h"
#include "gui.hpp"
#include "palette.hpp"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetFrameRate(30);
  
  ofSeedRandom();
  
#ifdef USE_CAMERA
  video.initGrabber(Constants::videoGrabWidth, Constants::videoGrabHeight);
#else
  video.load("subject.mp4");
  video.setVolume(0);
  video.play();
#endif
  
  backgroundColorChangeListener = Gui::getInstance().backgroundColor.newListener(this, &ofApp::backgroundColorChanged);
  ofColor c = Gui::getInstance().backgroundColor;
  backgroundColorChanged(c);
  
  ofAddListener(Gui::getInstance().videoPathChanged, this, &ofApp::videoFilePathChanged);
  Gui::getInstance().disruptAngleAbs.addListener(this, &ofApp::disruptedRotationAbs);
}

void ofApp::backgroundColorChanged(ofColor& c) {
  fbo.clear();
  fbo.allocate(Constants::canvasWidth, Constants::canvasHeight, GL_RGB);
  fbo.begin();
  ofBackground(Gui::getInstance().backgroundColor);
  fbo.end();
}

void ofApp::videoFilePathChanged(string& path) {
  video.load(path);
  video.setVolume(0);
  video.play();
}

void ofApp::disruptedRotationAbs() {
  Particle::disruptParticles(ParticleDisruption::angleAbs, Gui::getInstance().disruptionAmount);
}

//--------------------------------------------------------------
void ofApp::update(){
  video.update();
  
  if (video.isFrameNew()) {
    if (frame2.bAllocated) {
      frame1 = frame2;
      simpleFrame1 = simpleFrame2;
    }
    frame2.setFromPixels(video.getPixels());
#ifdef USE_CAMERA
    frame2.mirror(false, true);
#else
    frame2.mirror(false, false);
#endif
    ofxCvColorImage resizedFrame2;
    resizedFrame2.allocate(Constants::resizedVideoWidth, Constants::resizedVideoHeight);
    resizedFrame2.scaleIntoMe(frame2, CV_INTER_AREA);
    simpleFrame2 = resizedFrame2;
  
    if (frame1.bAllocated && frame2.bAllocated) {
      frameDiff.absDiff(simpleFrame1, simpleFrame2);
      frameDiff.threshold(64);
      frameDiff.blur();
      frameDiff.erode_3x3();
    }
    
    if (frameDiff.bAllocated && ofGetFrameRate() > 15) {
      const auto& pixels = frameDiff.getPixels();
      const float scale = float(pixels.getWidth()) / float(Constants::canvasWidth);
      const auto& framePixels = frame2.getPixels();
      const float scaleSimpleToFrameW = float(framePixels.getWidth()) / float(pixels.getWidth());
      const float scaleSimpleToFrameH = float(framePixels.getHeight()) / float(pixels.getHeight());
      const float scaleSimpleToFrame = std::min(scaleSimpleToFrameH, scaleSimpleToFrameW);
      for (int i=0; i<Gui::getInstance().maxAddedParticles; ++i) {
        size_t x = ofRandom(pixels.getWidth());
        size_t y = ofRandom(pixels.getHeight());
        if (pixels.getColor(x, y).getBrightness() > 128) {
          ofColor videoColor = framePixels.getColor(x*scaleSimpleToFrame, y*scaleSimpleToFrame);
          float noiseScale = 0.01;
          float noise = ofNoise(x*noiseScale, y*noiseScale, ofGetFrameNum()*noiseScale, 1.0);
          // mix palette 2 with the base video or palette 1 colour
          ofColor particleColor;
          if (Gui::getInstance().colorFromVideo) {
            particleColor = videoColor;
          } else {
            particleColor = ofColor(Gui::getInstance().palette1.getInterpolated(noise));
          }
          if (Gui::getInstance().mixColorFromPalette2) {
            float noise2 = ofNoise(x*noiseScale, y*noiseScale, ofGetFrameNum()*noiseScale, 10.0);
            ofColor palette2Color = ofColor(Gui::getInstance().palette2.getInterpolated(noise));
            particleColor.lerp(palette2Color, noise2);
          }
          Particle::makeParticle(x/scale, y/scale, videoColor, particleColor);
        };
      }
    }
  }
  
  Particle::updateParticles();
  
  fbo.begin();
  
  if (ofGetFrameNum() % Gui::getInstance().fadeDelay == 0) {
    ofColor c = Gui::getInstance().backgroundColor;
    c.a = 8;
    ofSetColor(c);
    ofEnableAlphaBlending();
    ofDrawRectangle(0, 0, Constants::canvasWidth, Constants::canvasHeight);
    ofClearAlpha();
  }

  ofBlendMode(OF_BLENDMODE_MULTIPLY);
  Particle::drawParticles();
  fbo.end();
}

//--------------------------------------------------------------
void drawImage(const ofxCvImage& image) {
  if (image.bAllocated) {
    ofSetColor(255, 255, 255, 64);
    image.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
  }
}

void ofApp::draw(){
  ofSetColor(ofColor::white);
  ofEnableAlphaBlending();
//  ofDisableBlendMode();
  fbo.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
  
  if (Gui::getInstance().showVideo) {
    ofEnableAlphaBlending();
    drawImage(simpleFrame1);
  }
  
  Gui::getInstance().performance =  "FPS: "+ofToString(int(ofGetFrameRate())) + " Marks: "+ofToString(Particle::particleCount());
  
  Gui::getInstance().draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (key == 's') {
    ofPixels pixels;
    fbo.readToPixels(pixels);
    ofSaveImage(pixels, ofFilePath::getUserHomeDir()+"/gestural/snapshot-"+ofGetTimestampString()+".png", OF_IMAGE_QUALITY_BEST);
  } else if (key == ' ') {
    Gui::getInstance().toggleShow();
  } else if(key == '[') {
    Gui::getInstance().save();
  } else if(key == ']') {
    Gui::getInstance().load();
  }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
