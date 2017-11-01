
	#ifndef MYCIRCLES
	#define MYCIRCLES

	#include "ofMain.h"
	#include "ofxBox2d.h"	// Es importante incorporar el .h a la clase padre

	class myCircles : public ofxBox2dCircle{

  private:

    float radius;

	public:
    ofColor color;

		myCircles();
    void setColor(int r, int g, int b);
    void setRadius(float r);
		void draw();
	};

	#endif
