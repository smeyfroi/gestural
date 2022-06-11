#include "ofApp.h"
#include "particle.hpp"
#include "globals.h"
#include "ofFileUtils.h"
#include "ofUtils.h"
#include "gui.hpp"
#include "palette.hpp"

float Globals::screenToCanvasWidthScale = 1.0;
float Globals::screenToCanvasHeightScale = 1.0;

//--------------------------------------------------------------
ofFbo createFbo() {
  ofFbo fbo;
  fbo.allocate(Constants::canvasWidth, Constants::canvasHeight, GL_RGBA);
  fbo.begin();
  ofClear(255, 255, 255, 0);
  fbo.end();
  return fbo;
}

void ofApp::setup(){
  Globals::screenToCanvasWidthScale = float(ofGetScreenWidth()) / Constants::canvasWidth;
  Globals::screenToCanvasHeightScale = float(ofGetScreenHeight()) / Constants::canvasHeight;

  ofSetFrameRate(30);
  ofSeedRandom();
  ofEnableAlphaBlending();

  paused = false;
  
#ifdef USE_CAMERA
  video.initGrabber(Constants::videoGrabWidth, Constants::videoGrabHeight);
#else
  video.load("subject.mp4");
  video.setVolume(0);
  video.play();
#endif
  
  ofAddListener(Gui::getInstance().videoPathChanged, this, &ofApp::videoFilePathChanged);
  Gui::getInstance().disruptCurrent.addListener(this, &ofApp::disruptedCurrent);
  Gui::getInstance().disruptAdd.addListener(this, &ofApp::disruptedAdd);
  Gui::getInstance().disruptReduce.addListener(this, &ofApp::disruptedReduce);
  
  activeFbo = createFbo();
  savedFbo = createFbo();
}

void ofApp::videoFilePathChanged(string& path) {
#ifndef USE_CAMERA
  video.load(path);
  video.setVolume(0);
  video.play();
#endif
}

void ofApp::disruptedCurrent() {
  Particle::disruptParticles(Gui::getInstance().disruptionAmount, Gui::getInstance().disruptionVariation);
}

void ofApp::disruptedAdd() {
  Particle::add(Gui::getInstance().disruptionAmount, Gui::getInstance().disruptionVariation);
}

void ofApp::disruptedReduce() {
  Particle::reduce(Gui::getInstance().disruptionAmount, Gui::getInstance().disruptionVariation);
}

//--------------------------------------------------------------
void ofApp::update(){
  if (paused) return;
  
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
  
  activeFbo.begin();
  Particle::drawParticles();
  activeFbo.end();
}

//--------------------------------------------------------------
void drawGhostImage(const ofxCvImage& image) {
  if (image.bAllocated) {
    ofSetColor(255, 255, 255, 16);
    image.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
  }
}

void drawFbo(ofFbo& fbo) {
  ofClear(Gui::getInstance().backgroundColor);
  ofSetColor(255, 255, 255, 255);
//  ofBlendMode(OF_BLENDMODE_MULTIPLY);
  fbo.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
}

void ofApp::drawComposite(int w, int h) {
  ofClear(Gui::getInstance().backgroundColor);
  ofSetColor(255, 255, 255, 255);
//  ofBlendMode(OF_BLENDMODE_ALPHA);
  savedFbo.draw(0, 0, w, h);
//  ofBlendMode(OF_BLENDMODE_ALPHA);
  activeFbo.draw(0, 0, w, h);
}

void ofApp::draw(){
  if (show==1) drawFbo(activeFbo);
  else if (show==2) drawFbo(savedFbo);
  else drawComposite(ofGetWindowWidth(), ofGetWindowHeight());
  
  if (Gui::getInstance().showVideo) drawGhostImage(simpleFrame1);
  
  Gui::getInstance().performance =  "FPS: "+ofToString(int(ofGetFrameRate())) + " Marks: "+ofToString(Particle::particleCount());
  Gui::getInstance().draw();
}

//--------------------------------------------------------------
ofFbo ofApp::makeComposite() {
  ofFbo compositeFbo;
  compositeFbo.allocate(Constants::canvasWidth, Constants::canvasHeight, GL_RGB);
  compositeFbo.begin();
  drawComposite(Constants::canvasWidth, Constants::canvasHeight);
  compositeFbo.end();
  return compositeFbo;
}

const int FADE_ALPHA = 254;
void ofApp::fadeSaved() {
  ofFbo fadedFbo = createFbo();
  fadedFbo.begin();
  ofSetColor(255, 255, 255, FADE_ALPHA);
  savedFbo.draw(0, 0);
  fadedFbo.end();
  savedFbo = fadedFbo;
}

void ofApp::keyPressed(int key){
  if (key == 's') {
    ofPixels pixels;
    makeComposite().readToPixels(pixels);
    ofSaveImage(pixels, ofFilePath::getUserHomeDir()+"/gestural/snapshot-"+ofGetTimestampString()+".png", OF_IMAGE_QUALITY_BEST);
  } else if (key == 'g') {
    Gui::getInstance().toggleShow();
  } else if(key == '[') {
    Gui::getInstance().save();
  } else if(key == ']') {
    Gui::getInstance().load();
  } else if (key == ' ') {
    paused = !paused;
#ifndef USE_CAMERA
    videoPaused = paused;
    video.setPaused(paused);
  } else if (key == 9) {
    videoPaused = !videoPaused;
    video.setPaused(videoPaused);
#endif
  } else if (key == '.' && !paused) {
    savedFbo.begin();
    ofSetColor(255, 255, 255, 255);
//    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // should be this?
//    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // should be this?
//    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    activeFbo.draw(0, 0);
    savedFbo.end();
    activeFbo.begin();
    ofClear(255, 255, 255, 0);
    activeFbo.end();
//    fadeSaved();
  } else if (key == 'f') {
    fadeSaved();
  }
  // debug
  if(key=='1') show=1;
  else if (key=='2') show=2;
  else if (key=='0') show=0;
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
