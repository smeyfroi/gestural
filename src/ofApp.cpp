#include "ofApp.h"
#include "particle.hpp"
#include "globals.h"
#include "ofFileUtils.h"
#include "ofUtils.h"
#include "gui.hpp"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetFrameRate(30);
  
#ifdef USE_CAMERA
  video.initGrabber(Constants::videoGrabWidth, Constants::videoGrabHeight);
#else
  video.load("subject.mp4");
  video.play();
#endif
  
  fbo.allocate(Constants::canvasWidth, Constants::canvasHeight, GL_RGB);
  fbo.begin();
//  ofBackground(Constants::canvasBackgroundColor); // doesn't work, always black
  ofBackground(ofColor::white);
  fbo.end();
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
//    resizedFrame2.allocate(frame2.width, frame2.height);
    resizedFrame2.scaleIntoMe(frame2, CV_INTER_AREA);
    simpleFrame2 = resizedFrame2;
  
    if (frame1.bAllocated && frame2.bAllocated) {
      frameDiff.absDiff(simpleFrame1, simpleFrame2);
      frameDiff.threshold(64);
      frameDiff.blur();
      frameDiff.erode_3x3();
    }
    
    if (frameDiff.bAllocated) {
      const auto& pixels = frameDiff.getPixels();
      const float scale = float(pixels.getWidth()) / float(Constants::canvasWidth);
      for (int i=0; i<Gui::getInstance().maxAddedParticles; ++i) {
        size_t x = ofRandom(pixels.getWidth());
        size_t y = ofRandom(pixels.getHeight());
        if (pixels.getColor(x, y).getBrightness() > 128) {
          Particle::makeParticle(x/scale, y/scale);
        };
      }
    }
  }
  
  Particle::updateParticles();
  
  fbo.begin();
  ofEnableAlphaBlending();
  if (ofGetFrameNum() % Gui::getInstance().fadeDelay == 0) {
    ofSetColor(255, 255, 255, 8);
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
  ofDisableBlendMode();
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
