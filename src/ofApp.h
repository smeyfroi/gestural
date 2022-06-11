#pragma once
#define USE_CAMERA 1
#undef USE_CAMERA

#include "ofMain.h"
#include "ofxOpenCv.h"

class ofApp : public ofBaseApp{

public:
  void setup();
  void videoFilePathChanged(std::string&);
  void update();
  void draw();
  void disruptedCurrent();
  void disruptedAdd();
  void disruptedReduce();
  
  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y );
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void mouseEntered(int x, int y);
  void mouseExited(int x, int y);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);
		
private:
#ifdef USE_CAMERA
  ofVideoGrabber video;
#else
  ofVideoPlayer video;
  bool videoPaused;
#endif
  ofxCvColorImage frame1, frame2;
  ofxCvGrayscaleImage simpleFrame1, simpleFrame2;
  ofxCvGrayscaleImage frameDiff;
  ofFbo savedFbo;
  ofFbo activeFbo;
  ofEventListener backgroundColorChangeListener;
  bool paused;
  ofFbo makeComposite();
  void drawComposite(int w, int h);
  int show;
  void fadeSaved();
};
