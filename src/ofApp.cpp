#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetFrameRate(60);
  video.initGrabber(640, 480);
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
  }
}

//--------------------------------------------------------------
void drawImage(const ofxCvImage& image) {
  if (image.bAllocated) {
    ofSetColor(ofColor::white);
    image.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
  }
}

void ofApp::draw(){
  ofBackground(ofColor::white);
  drawImage(frameDiff);
//  drawImage(simpleFrame1);
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
