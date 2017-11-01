#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "ofxOpenCv.h"
#include "myCircles.h"


//#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file

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

		// --------- ofxBox2d ---------

		ofxBox2d                                box2d;
		ofxBox2dCircle                          ground;
		ofVboMesh                               groundMesh;
		vector		<shared_ptr<myCircles> >    circles;

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

		int 				threshold;
		bool				bLearnBakground;

		//--------- ConmplexPolygon ------------
		vector <ofPoint> loadPoints(string file);
		ofPolyline                          shape;
		vector <shared_ptr<ofxBox2dPolygon> >	polyShapes;

};
