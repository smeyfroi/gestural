#include "ofApp.h"
#include "particle.hpp"

const int MAX_NEW_PARTICLES = 200;
const size_t canvasWidth = 72*37;
const size_t canvasHeight = 72*25;

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetFrameRate(60);
  video.initGrabber(640, 480);
  // init drawing surface
  fbo.allocate(canvasWidth, canvasHeight, GL_RGB);
  fbo.begin();
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
    frame2.mirror(false, true);
    ofxCvColorImage resizedFrame2;
//    resizedFrame2.allocate(frame2.width*0.3, frame2.height*0.3);
    resizedFrame2.allocate(frame2.width, frame2.height);
    resizedFrame2.scaleIntoMe(frame2, CV_INTER_AREA);
    simpleFrame2 = resizedFrame2;
  }
  
  if (frame1.bAllocated && frame2.bAllocated) {
    frameDiff.absDiff(simpleFrame1, simpleFrame2);
    frameDiff.threshold(64);
    frameDiff.blur();
//    frameDiff.erode_3x3();
  }
  
  if (frameDiff.bAllocated) {
    const auto& pixels = frameDiff.getPixels();
    const float scale = float(pixels.getWidth()) / float(canvasWidth);
    for (int i=0; i<MAX_NEW_PARTICLES; ++i) {
      size_t x = ofRandom(pixels.getWidth());
      size_t y = ofRandom(pixels.getHeight());
      if (pixels.getColor(x, y).getBrightness() > 128) {
        Particle::makeParticle(x/scale, y/scale);
      };
    }
  }
  
  Particle::updateParticles();
  
  fbo.begin();
  ofEnableAlphaBlending();
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
//  ofBackground(ofColor::white);
//  drawImage(frameDiff);
  
  ofSetColor(ofColor::white);
  ofDisableBlendMode();
  fbo.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
  
  ofEnableAlphaBlending();
//  drawImage(frameDiff);
  drawImage(simpleFrame1);
//  Particle::drawParticles();
  
  ofSetColor(ofColor::black);
  ofDrawBitmapString(ofGetFrameRate(), 20, 20);
  ofDrawBitmapString(Particle::particleCount(), 20, 40);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
