#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofEnableAlphaBlending();
	ofEnableSmoothing();
	ofSetFrameRate(60);
	ofSetVerticalSync(true);

	puffersphere = new ofxPuffersphere();
	puffersphere->offaxisLocation  = "shaders/offaxis";
	puffersphere->spherizeLocation = "shaders/spherize";
	puffersphere->setup(1050);

	puffersphere->addGuiParams();
	
	//create assets like this if you're using normal images
	fullscaleBG = puffersphere->createObject("spherical.png");
	videoQuad = puffersphere->createObject("video_frame.png");
	//videoQuad->scale = ofVec2f(.5, .5);
	
	//but if you're using your own, then do it like this:
//	ofxPuffersphereObject* o = puffersphere->createObject();
//	o->setTexture(<custom class>);
	
	//ofxPuffersphere manages all the internal objects so when you call ->render and ->draw they all show up
	
	drawSphere = false;
	
    gui.loadFromXML();
	gui.toggleDraw();
}


//--------------------------------------------------------------
void ofApp::update(){
	ofQuaternion bgquad;
	bgquad.makeRotate(-ofGetFrameNum()/10.0, 0, 0, 1);
	fullscaleBG->rotation = bgquad;
	
	ofQuaternion q1, q2, q3;
	q1.makeRotate(30*sin(ofGetFrameNum()/20.0), 0, 1, 0); //rotate this between 30 and -30
	q2.makeRotate(ofGetFrameNum(), 0, 0, 1); //spin it around the center
	q3.makeRotate(ofGetFrameNum()*3, 1, 0, 0);
	videoQuad->rotation = q3*q2*q1;
	
	
	//you can also do things like this to change the color
//	videoQuad->makeTransparent();
//	videoQuad->tint = ofColor(1.0, 0., 0. 1.0);
	
	puffersphere->render();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(255);
	
	if(drawSphere){
		puffersphere->drawSphere(ofVec3f( ofGetWidth()/2.0, ofGetHeight()/2.0, 10) );
	}
	else{
		puffersphere->draw();
	}

	ofPushStyle();
	gui.draw();
	ofEnableAlphaBlending();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == 'p'){
		drawSphere = !drawSphere;
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
void ofApp::windowResized(int w, int h){

}

