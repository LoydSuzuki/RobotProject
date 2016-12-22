#pragma once

#include "ofMain.h"
#include "ofxBullet.h"
#include "ofxGui.h"
#include "btGhostObject.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
    
    void onCollision(ofxBulletCollisionData& cdata);
    //bool ContactCallback( btManifoldPoint& manifold, void* object0, void* object1 );
    
    ofEasyCam                   easyCam;
	
	ofxBulletWorldRigid			world;
	ofxBulletBox				ground;
    ofxBulletBox*				base;
    
    ofxBulletBox*               pan_a;
    ofxBulletBox*               tilt_a;
    ofxBulletBox*               tilt_b;
    ofxBulletBox*               tilt_c;
    
    ofxPanel gui;
    ofxFloatSlider slider_pan_a;
    ofxFloatSlider slider_tilt_a;
    ofxFloatSlider slider_tilt_b;
    ofxFloatSlider slider_tilt_c;

    bool ground_colliding;
    
    ///TestData boxdata;
    
};
