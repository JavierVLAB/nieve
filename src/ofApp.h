#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "myCircles.h"
#include "ofxImGui.h"


//#define _USE_LIVE_VIDEO
//#define _USE_KINECT

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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
		void exit();

		float scale;


		// --------- ofxBox2d ---------

		ofxBox2d                                box2d;
		ofxBox2dCircle                          ground;
		ofVboMesh                               groundMesh;
		vector		<shared_ptr<myCircles> >    circles;
		float 									density;
		float 									bounce;
		float	 									friction;

		// ----------- ofxOpenCv -----------

		#ifdef _USE_LIVE_VIDEO
			ofVideoGrabber 		vidGrabber;
		#else
			ofVideoPlayer 		vidPlayer;
		#endif

		ofxCvColorImage			colorImg;

		ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;

		ofxCvContourFinder 	contourFinder;

		int 				openCvThreshold;
		bool				bLearnBakground;

		//--------- ConmplexPolygon ------------
		vector <ofPoint> loadPoints(string file);
		ofPolyline                          shape;
		vector <shared_ptr<ofxBox2dPolygon> >	polyShapes;

		//--------- ofxKinect ------

		ofxKinect kinect;

	bool bThreshWithOpenCV;
	bool bDrawPointCloud;

	int nearThreshold;
	int farThreshold;

	int angle;

		//--------- ofxImGui ------------

	void drawGui();
	void setupGui();

	ofxImGui::Gui gui;
	bool     showGui;


	//------

	ofFbo fbo;
	ofMesh mesh;




};
