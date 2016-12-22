#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground( 10, 10, 10);
    
    //easyCam setup
    easyCam.setDistance(3000);
    
    //world setup
	world.setup(); //重力は0
    world.setGravity(ofVec3f(0,0,0));
	//world.enableGrabbing();
    world.enableCollisionEvents(); //衝突判定
    ofAddListener(world.COLLISION_EVENT, this, &ofApp::onCollision);
    
    //地面(ground)setup
    ground.create( world.world, ofVec3f(0.,-0.5, 0.), 0., 5000.f, 1.0f, 5000.f );
    ground.setProperties(.25, .95);
    ground.add();
    // allows manipulation of object. The object will not react to other objects, but will apply forces to them //
    
    //土台(base)setup
	base = new ofxBulletBox();
    base->create(world.world, ofVec3f(0, 160/2, 0), 0, 500, 160, 500);
    base->setProperties(.25, .95);
    base->add();
    
    pan_a = new ofxBulletBox();
    pan_a->create(world.world, ofVec3f(0,160+190/2, 0), 0., 193.,190.,193.);
    pan_a->setProperties(.25,.95);
    pan_a->add();
    pan_a->enableKinematic();
    //pan_a->setActivationState(DISABLE_DEACTIVATION);
                  
    
    tilt_a = new ofxBulletBox();
    tilt_a->create( world.world, ofVec3f(0.,0., 0.), 1., 1000., 193., 193.);
    tilt_a->setProperties(.25,.95);
    tilt_a->add();
    tilt_a->enableKinematic();
    tilt_a->setActivationState(DISABLE_DEACTIVATION);
    
    tilt_b = new ofxBulletBox();
    tilt_b->create( world.world, ofVec3f(0.,0., 0.), 1., 1000.f, 169.f, 169.f );
    tilt_b->setProperties(.25,.95);
    tilt_b->add();
    tilt_b->enableKinematic();
    tilt_b->setActivationState(DISABLE_DEACTIVATION);
    
    
    //gui
    gui.setup("panel");
    gui.add(slider_pan_a.setup("pan_a",0.,-50.,50.));
    gui.add(slider_tilt_a.setup("tilt_a",0.,-50.,50.));
    gui.add(slider_tilt_b.setup("tilt_b",0.,-50.,50.));
    
}

//--------------------------------------------------------------
void ofApp::update() {
	
    world.update();
    //ofSetWindowTitle(ofToString(ofGetFrameRate(), 0));
    
    // store the position of the ground //
    ofVec3f pos = ground.getPosition();
    
    // create a bullet transform object and set the position to that of the object //
    btTransform trans_pan_a;
    btTransform trans_tilt_a;
    btTransform trans_tilt_b;
    
    //setIdentity(初期化)
    trans_pan_a.setIdentity();
    trans_tilt_a.setIdentity();
    trans_tilt_b.setIdentity();
    
    // get the rotation quaternion from the ground //
    ofQuaternion rotQuat = tilt_a->getRotationQuat();

    // rotate it a bit //
    float newRot_pan_a = slider_pan_a*0.1;;
    float newRot_tilt_a = slider_tilt_a*0.1;
    float newRot_tilt_b = slider_tilt_b*0.1;
    
    // set the rotation of the bullet transform to that of the axis of the stored quaternion
    // and apply the new rotation
    
    btQuaternion pan_a_quat =  btQuaternion(btVector3(0.,1.0,0.),newRot_pan_a);
    btQuaternion tilt_a_quat = btQuaternion(btVector3(0.,0.,1.0),newRot_tilt_a);
    btQuaternion tilt_b_quat = btQuaternion(btVector3(0.,0.,1.0),newRot_tilt_b);
    
    
    trans_pan_a.setRotation(pan_a_quat);
    trans_tilt_a.setRotation(pan_a_quat * tilt_a_quat );
    trans_tilt_b.setRotation(pan_a_quat * tilt_a_quat * tilt_b_quat);
    
    float tilt_a_x = cos(newRot_tilt_a)*500.0f;
    float tilt_a_y = sin(newRot_tilt_a)*500.0f;
    
    float tilt_a_x_pan = cos(newRot_pan_a)*tilt_a_x;
    float tilt_a_z_pan = -sin(newRot_pan_a)*tilt_a_x;
    
    btVector3 vector_tilt_a = btVector3(tilt_a_x_pan, tilt_a_y+160+190, tilt_a_z_pan);
    
    float tilt_origin_x = tilt_a_x+cos(ofDegToRad(newRot_tilt_a))*500.0f;
    float tilt_origin_y = tilt_a_y+sin(ofDegToRad(newRot_tilt_a))*500.0f;
    
    //先端座標の計算
    
    float tilt_b_local_x = cos(newRot_tilt_b)*500.0f+1000.0f;
    float tilt_b_local_y = sin(newRot_tilt_b)*500.0f;
    
    float tilt_b_global_x = tilt_b_local_x * cos(newRot_tilt_a) - tilt_b_local_y * sin(newRot_tilt_a);
    float tilt_b_global_y = tilt_b_local_x * sin(newRot_tilt_a) + tilt_b_local_y * cos(newRot_tilt_a);
    
    float tilt_b_global_x_pan = cos(newRot_pan_a)*tilt_b_global_x;
    float tilt_b_global_z_pan = -sin(newRot_pan_a)*tilt_b_global_x;
    
    float tilt_b_x = tilt_b_global_x_pan;
    float tilt_b_y = tilt_b_global_y+160+190;
    float tilt_b_z = tilt_b_global_z_pan;
    
    trans_pan_a.setOrigin(btVector3(0, 160+190/2, 0));
    
    trans_tilt_a.setOrigin(vector_tilt_a);
    trans_tilt_b.setOrigin(btVector3( tilt_b_x, tilt_b_y, tilt_b_z));
    // apply the transform to the rigid body //
    //tilt_a->getRigidBody()->setCenterOfMassTransform(trans);
    
    pan_a->getRigidBody()->setCenterOfMassTransform(trans_pan_a);
    pan_a->getRigidBody()->getMotionState()->setWorldTransform(trans_pan_a);
    pan_a->activate();
    
    tilt_a->getRigidBody()->setCenterOfMassTransform(trans_tilt_a);
    tilt_a->getRigidBody()->getMotionState()->setWorldTransform(trans_tilt_a);
    // tell the ofxBulletWorldRigid that we have moved rigid body and it should update the collision object //
    tilt_a->activate();
    
    tilt_b->getRigidBody()->setCenterOfMassTransform(trans_tilt_b);
    tilt_b->getRigidBody()->getMotionState()->setWorldTransform(trans_tilt_b);
    // tell the ofxBulletWorldRigid that we have moved rigid body and it should update the collision object //
    tilt_b->activate();
    
   }

//--------------------------------------------------------------
void ofApp::draw() {
	glEnable( GL_DEPTH_TEST );
	//camera.begin();
    easyCam.begin();
	
	ofSetLineWidth(1.f);
	ofSetColor(255, 0, 200);
	world.drawDebug();
	
    
    if(ground_colliding == TRUE){
        ofSetColor(255,0,0);
        ground.draw();
    }
    else{
        ofSetColor(100,100,100);
        ground.draw();
    }
    
    //ofSetColor(100,100,100);
    //ground.draw();
    
	
    ofSetColor(225, 225, 225);
	base->draw();
	
    ofSetColor(225, 225, 225);
    pan_a->draw();
    tilt_a->draw();
    tilt_b->draw();
	
    easyCam.end();
    
    glDisable( GL_DEPTH_TEST );
    
    gui.draw();
    
    ground_colliding = FALSE;
}

//--------------------------------------------------------------


void ofApp::onCollision(ofxBulletCollisionData& cdata) {
    
    cout << "ContactCallBack " <<endl;
    
    if(ground == cdata){
        
        ground_colliding = TRUE;
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) { 
	
}
