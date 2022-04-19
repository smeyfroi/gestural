#include "ofApp.h"
#include "particle.hpp"

const int MAX_NEW_PARTICLES = 500;
const size_t canvasWidth = 72*37;
const size_t canvasHeight = 72*25;
const size_t resizedVideoWidth = 640;
const size_t resizedVideoHeight = 480;

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetFrameRate(60);
#ifdef USE_CAMERA
  video.initGrabber(640, 480);
#else
  video.load("subject.mp4");
  video.play();
#endif
  // init drawing surface
  fbo.allocate(canvasWidth, canvasHeight, GL_RGB);
  fbo.begin();
  ofBackground(ofColor::black);
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
    resizedFrame2.allocate(resizedVideoWidth, resizedVideoHeight);
//    resizedFrame2.allocate(frame2.width, frame2.height);
    resizedFrame2.scaleIntoMe(frame2, CV_INTER_AREA);
    simpleFrame2 = resizedFrame2;
  }
  
  if (frame1.bAllocated && frame2.bAllocated) {
    frameDiff.absDiff(simpleFrame1, simpleFrame2);
    frameDiff.threshold(64);
    frameDiff.blur();
    frameDiff.erode_3x3();
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
//  ofEnableAlphaBlending();
//  if (ofGetFrameNum() % 10 == 0) {
//    ofSetColor(255, 255, 255, 4);
//    ofDrawRectangle(0, 0, canvasWidth, canvasHeight);
////    ofClearAlpha();
//  }
//  ofBlendMode(OF_BLENDMODE_MULTIPLY);
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
  
//  ofEnableAlphaBlending();
//  drawImage(simpleFrame1);
  
  ofSetColor(ofColor::white);
  ofDrawBitmapString(ofGetFrameRate(), 20, 20);
  ofDrawBitmapString(Particle::particleCount(), 20, 40);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (key == 's') {
    ofPixels pixels;
    fbo.readToPixels(pixels);
    ofSaveImage(pixels, "/Users/stevemeyfroidt/Desktop/Gestural-"+ofToString(ofGetFrameNum())+".png", OF_IMAGE_QUALITY_BEST);
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
