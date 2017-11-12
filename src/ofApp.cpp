#include "ofApp.h"
#include "ofxOpenCv.h"

static bool removeShapeOffScreen(shared_ptr<ofxBox2dBaseShape> shape) {
    if (!ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(shape.get()->getPosition())) {
        return true;
    }
    return false;
}

//---------
void ofApp::setupGui(){
  ImGuiIO * io = &ImGui::GetIO();
  io->Fonts->AddFontFromFileTTF(&ofToDataPath("Roboto-Medium.ttf")[0], 14.f);
  //io->Fonts->AddFontFromFileTTF(&ofToDataPath("Raleway-Regular.ttf")[0], 12.f);

  gui.setup();

  showGui = false;
}

//----------
void ofApp::drawGui(){

  if(showGui){
    gui.begin();
      ImGui::Separator();
      ImGui::Text("Application average frame rate = %.1f FPS", ImGui::GetIO().Framerate);
      ImGui::Separator();
      ImGui::Text("Press Space Bar to show de Gui");
      ImGui::Separator();
      ImGui::SliderInt("Near threshold", &nearThreshold, 0, 255);
      ImGui::SliderInt("Far threshold", &farThreshold, 0, 255);
      ImGui::SliderInt("OpenCV threshold", &openCvThreshold, 0, 255);
      ImGui::Separator();
      ImGui::Separator();
      ImGui::SliderFloat("Density", &density, 0, 10.0);
      ImGui::SliderFloat("Bounce", &bounce, 0, 1.0);
      ImGui::SliderFloat("Friction", &friction, 0, 10.0);
    gui.end();
  }
}



//--------------------------------------------------------------
void ofApp::setup(){

  ofDisableAntiAliasing();
	ofBackgroundHex(0x1F2C30);
	//ofSetLogLevel(OF_LOG_NOTICE);
  ofSetLogLevel(OF_LOG_SILENT);

  ofSetFrameRate(30);
  mesh.setMode(OF_PRIMITIVE_POINTS);
  glPointSize(3);

  //----------- openCV--------------------

	#ifdef _USE_LIVE_VIDEO
    #ifdef _USE_KINECT
      kinect.setRegistration(true);
      kinect.init();
      kinect.open();
      kinect.setCameraTiltAngle(0);
    #else
        vidGrabber.setVerbose(true);
        vidGrabber.setup(640,480);
    #endif
	#else
        vidPlayer.load("fingers.mov");
        vidPlayer.play();
        vidPlayer.setLoopState(OF_LOOP_NORMAL);
	#endif

  colorImg.allocate(640,480);
  grayImage.allocate(640,480);
  grayBg.allocate(640,480);
  grayDiff.allocate(640,480);

  bLearnBakground = true;
  openCvThreshold = 30;
  nearThreshold = 255;
  farThreshold = 235;

  //------------------ box2d ------------

	box2d.init();
	box2d.setGravity(0, 5);
	box2d.setFPS(60.0);
	box2d.registerGrabbing();
  //box2d.createGround();

  //ground.setPhysics(0, 0, 0);
  //ground.setup(box2d.getWorld(), ofGetWidth()/2, ofGetHeight()+200, 400);

  //-----------
  float scaleW = ofGetWidth()/640.0;
  float scaleH = ofGetHeight()/480.0;


  scale = scaleW > scaleH ? scaleH : scaleW;

  density = 1.0;
  bounce = 0.0001;
  friction = 10.0;


}


//--------------------------------------------------------------
void ofApp::update(){

  //---------- OpenCV --------

  bool bNewFrame = false;

  #ifdef _USE_LIVE_VIDEO
      #ifdef _USE_KINECT
        kinect.update();
        bNewFrame = kinect.isFrameNew();
      #else
        vidGrabber.update();
        bNewFrame = vidGrabber.isFrameNew();
      #endif
    #else
        vidPlayer.update();
        bNewFrame = vidPlayer.isFrameNew();
  #endif

  if (bNewFrame){

    #ifdef _USE_LIVE_VIDEO
        #ifdef _USE_KINECT
          colorImg.setFromPixels(kinect.getPixels());
          grayImage.setFromPixels(kinect.getDepthPixels());

          ofPixels & pix = grayImage.getPixels();
			    int numPixels = pix.size();
    			for(int i = 0; i < numPixels; i++) {
    				if(pix[i] < nearThreshold && pix[i] > farThreshold) {
    					pix[i] = 255;
    				} else {
    					pix[i] = 0;
    				}
    			}

          grayImage.flagImageChanged();

          contourFinder.findContours(grayImage, 20, (640*480)/10, 10, true);

        #else
          colorImg.setFromPixels(vidGrabber.getPixels());
          grayImage = colorImg;

          grayDiff.absDiff(grayBg, grayImage);
          grayDiff.threshold(openCvThreshold);

          contourFinder.findContours(grayDiff, 20, (640*480)/10, 10, true);
        #endif
    #else
          ofImage img;
          img.setFromPixels(vidPlayer.getPixels());
          img.resize(640,480);
          colorImg.setFromPixels(img.getPixels());
          grayImage = colorImg;

          grayDiff.absDiff(grayBg, grayImage);
          grayDiff.threshold(openCvThreshold);

          contourFinder.findContours(grayDiff, 20, (640*480)/10, 10, true);
    #endif


    if (bLearnBakground == true){
      grayBg = grayImage;
      bLearnBakground = false;
    }



    if(contourFinder.nBlobs > 0){
      //vector <ofPoint> pts = loadPoints("shape.dat");
      shared_ptr<ofxBox2dPolygon> poly = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
      vector<ofPoint> vec;
      vec.resize(contourFinder.blobs[0].nPts);

      for(int i = 0; i < vec.size(); i++){
        vec[i] = contourFinder.blobs[0].pts[i]*scale;
      }
        //vec.simplify();

      poly->addVertices(vec);
      poly->setPhysics(0, 0, 0);
      poly->triangulatePoly();
      poly->create(box2d.getWorld());
      polyShapes.push_back(poly);
    }


  }

  //-------- box2d --------

  ofRemove(circles, removeShapeOffScreen);


  box2d.update();
  box2d.update();
  //box2d.update();

  float r = ofRandom(1, 3);
  for (int i = 0; i  < 5; i++){
  circles.push_back(shared_ptr<myCircles>(new myCircles));
  circles.back().get()->setPhysics(density, bounce, friction);// density, bounce and friction
  circles.back().get()->setup(box2d.getWorld(), ofRandom(10, ofGetWidth() - 10), -200+ofRandom(30, 100), r);
  circles.back().get()->setRadius(r);
  circles.back().get()->setColor(255,255,255);
  circles.back().get()->setFixedRotation(true);
  //circles.back().get()->setColor(ofRandom(255),ofRandom(255),ofRandom(255));
}
  polyShapes.clear();

  mesh.clear();

for (int i = 0; i < circles.size(); i++){
    mesh.addVertex(circles[i]->getPosition());
}



}

//--------------------------------------------------------------
void ofApp::draw(){


  //---- openCV -----

  // draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
	//grayImage.draw(0,0,640*scale,480*scale);
  colorImg.draw(0,0,640*scale,480*scale);

	ofFill();
	ofSetHexColor(0x333333);
	//ofDrawRectangle(360,540,320,240);
	ofSetHexColor(0xffffff);

    // draw my own blob

    for (int i = 0; i < contourFinder.nBlobs; i++){
      for (int j = 0; j < contourFinder.blobs[i].nPts - 1; j++){
        ofSetHexColor(0xffffff);
        ofDrawLine(contourFinder.blobs[i].pts[j]*scale,
                   contourFinder.blobs[i].pts[j+1]*scale);
      }

    }

  // ----------- box2d ---------

  //for(int i=0; i<circles.size(); i++) {
    //ofFill();
    //ofSetHexColor(0xffffff);
    //circles[i].get()->draw();
  //}
  mesh.draw();
    //ofSetColor(255, 100);
    //groundMesh.draw();

    //showInfo(box2d.getBodyCount(),box2d.getJointCount());

    for (int i=0; i<polyShapes.size(); i++) {
		polyShapes[i]->draw();

        //ofCircle(polyShapes[i]->getPosition(), 3);
	  }

    drawGui();

}

//---------------------
void ofApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

  switch (key){
    case ' ':
      bLearnBakground = true;
      break;
    case '+':
     openCvThreshold ++;
      if (openCvThreshold > 255) openCvThreshold = 255;
      break;
    case '-':
     openCvThreshold --;
      if (openCvThreshold < 0) openCvThreshold = 0;
      break;
    case 'g':
      showGui = !showGui;
      break;
    case 'f':
			ofToggleFullscreen();
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
