#include "ofMain.h"
#include "myCircles.h"

//------------------------------------------------
myCircles::myCircles() {

  radius = 0;
  color.r = 0;
  color.g = 0;
  color.b = 0;

};

//------------------
void myCircles::setColor(int r, int g, int b){
  color.r = r;
  color.g = g;
  color.b = b;
}

//--------------
void myCircles::setRadius(float r){
  radius = r;
}

//------------------------------------------------
void myCircles::draw() {

	if(!isBody()) return;

	ofPushMatrix();
	ofTranslate(getPosition().x, getPosition().y, 0);
	ofRotate(getRotation(), 0, 0, 1);
    ofPushStyle();
    ofSetColor(color);
  	ofDrawCircle(0, 0, radius);
    ofPopStyle();
  ofPopMatrix();

};
