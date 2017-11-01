#include "ofApp.h"
#include "ofxOpenCv.h"

static bool removeShapeOffScreen(shared_ptr<ofxBox2dBaseShape> shape) {
    if (!ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(shape.get()->getPosition())) {
        return true;
    }
    return false;
}


//--------------------------------------------------------------
void ofApp::setup(){

  ofDisableAntiAliasing();
	ofBackgroundHex(0x1F2C30);
	//ofSetLogLevel(OF_LOG_NOTICE);

  ofSetFrameRate(30);

  //----------- openCV--------------------

	#ifdef _USE_LIVE_VIDEO
        vidGrabber.setVerbose(true);
        vidGrabber.setup(320,240);
	#else
        vidPlayer.load("fingers.mov");
        vidPlayer.play();
        vidPlayer.setLoopState(OF_LOOP_NORMAL);
	#endif

  colorImg.allocate(320,240);
  //grayImage.allocate(320,240);
  //grayBg.allocate(320,240);
  //grayDiff.allocate(320,240);

  bLearnBakground = true;
  threshold = 30;

  //------------------ box2d ------------

	box2d.init();
	box2d.setGravity(0, 10);
	box2d.setFPS(60.0);
	box2d.registerGrabbing();
  box2d.createGround();

  //ground.setPhysics(0, 0, 0);
  //ground.setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()+200, 400);



}


//--------------------------------------------------------------
void ofApp::update(){

  //---------- OpenCV --------

  bool bNewFrame = false;

  #ifdef _USE_LIVE_VIDEO
       vidGrabber.update();
     bNewFrame = vidGrabber.isFrameNew();
    #else
        vidPlayer.update();
        bNewFrame = vidPlayer.isFrameNew();
  #endif

  if (bNewFrame){

    #ifdef _USE_LIVE_VIDEO
          colorImg.setFromPixels(vidGrabber.getPixels());
    #else
          colorImg.setFromPixels(vidPlayer.getPixels());
    #endif

    grayImage = colorImg;
    if (bLearnBakground == true){
      grayBg = grayImage;
      bLearnBakground = false;
    }

    grayDiff.absDiff(grayBg, grayImage);
    grayDiff.threshold(threshold);

    contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);

    if(contourFinder.nBlobs > 0){
      //vector <ofPoint> pts = loadPoints("shape.dat");
      shared_ptr<ofxBox2dPolygon> poly = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
      vector<ofPoint> vec;
      vec.resize(contourFinder.blobs[0].nPts);

      for(int i = 0; i < vec.size(); i++){
        vec[i] = contourFinder.blobs[0].pts[i]*3;
      }

      poly->addVertices(vec);
      poly->setPhysics(1000.0, 1.0, 0.0);
      poly->triangulatePoly();
      poly->create(box2d.getWorld());
      polyShapes.push_back(poly);
    }


  }

  //-------- box2d --------

  ofRemove(circles, removeShapeOffScreen);

  box2d.update();

  float r = ofRandom(2, 8);
  circles.push_back(shared_ptr<myCircles>(new myCircles));
  circles.back().get()->setPhysics(3.0, 0.33, 0.3);// density, bounce and friction
  circles.back().get()->setup(box2d.getWorld(), ofRandom(10, ofGetWidth() - 10), -200+ofRandom(30, 100), r);
  circles.back().get()->setRadius(r);
  circles.back().get()->setColor(ofRandom(255),ofRandom(255),ofRandom(255));
  polyShapes.clear();

}

//--------------------------------------------------------------
void ofApp::draw(){

  //---- openCV -----

  // draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
	colorImg.draw(0,0,320*3,240*3);

	ofFill();
	ofSetHexColor(0x333333);
	//ofDrawRectangle(360,540,320,240);
	ofSetHexColor(0xffffff);

    // draw my own blob

    for (int i = 0; i < contourFinder.nBlobs; i++){
      for (int j = 0; j < contourFinder.blobs[i].nPts - 1; j++){
        ofSetHexColor(0xffffff);
        ofDrawLine(contourFinder.blobs[i].pts[j]*3,contourFinder.blobs[i].pts[j+1]*3);
      }

    }

	// finally, a report:
	ofSetHexColor(0xffffff);
	stringstream reportStr;
	reportStr << " fps: " << ofGetFrameRate();
	ofDrawBitmapString(reportStr.str(), 20, 600);


  // ----------- box2d ---------

  for(int i=0; i<circles.size(); i++) {
    //ofFill();
    //ofSetHexColor(0xffffff);
    circles[i].get()->draw();
  }

    //ofSetColor(255, 100);
    //groundMesh.draw();

    //showInfo(box2d.getBodyCount(),box2d.getJointCount());

    for (int i=0; i<polyShapes.size(); i++) {
		polyShapes[i]->draw();

        //ofCircle(polyShapes[i]->getPosition(), 3);
	  }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

  switch (key){
    case ' ':
      bLearnBakground = true;
      break;
    case '+':
      threshold ++;
      if (threshold > 255) threshold = 255;
      break;
    case '-':
      threshold --;
      if (threshold < 0) threshold = 0;
      break;
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
