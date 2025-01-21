/**
 * ofxPuffersphere
 *	
 * Copyright (c) 2011 Flightphase
 *
 * implementaiton by James George (@obviousjim) and Tim Gfrerer (@tgfrerer) for the 
 * Voyagers gallery National Maritime Museum 
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------
 *
 * ofxPuffersphere 
 * Allows you to draw an arbitrary objects onto the puffersphere!
 * Since this screen is spherical, you don't specify a x/y location like normal, instead you specify
 * a rotation from the center to move this object to.
 */

#include "ofxPuffersphere.h"
#include <float.h>

ofxPuffersphereObject::ofxPuffersphereObject(){
	rotation = ofQuaternion(0,0,0,1);
	tint = ofFloatColor(1.0, 1.0, 1.0, 1.0);
	scale = ofVec2f(1.,1.0);

	image = NULL;
	offaxis = NULL;
}

void ofxPuffersphereObject::makeTransparent(){
	tint = ofFloatColor(0,0,0,0);
}

bool ofxPuffersphereObject::isTransparent(){
	return tint.a < FLT_EPSILON;
}

void ofxPuffersphereObject::setup(float dimenions){
	targetSize.x = dimenions*2;
	targetSize.y = dimenions;

	canvasCoords.clear();
	canvasCoords.push_back(ofVec2f(0,0));
	canvasCoords.push_back(ofVec2f(targetSize.x,0));
	canvasCoords.push_back(ofVec2f(targetSize.x,targetSize.y));
	canvasCoords.push_back(ofVec2f(0,targetSize.y));

	alpha = 255;
	targetAlpha = 255;
	oneStep = 10;
}


void ofxPuffersphereObject::update () {
	if ( image && isVideo ) {
		ofVideoPlayer* vid = ( ofVideoPlayer* ) image;
		//      vid->play();        // I have to uncomment this for it to work!  
		vid->update ();
	}

	// fading
	if ( targetAlpha != alpha ) {
		if ( targetAlpha < alpha ) {
			alpha -= oneStep;
			//cout << " - " << alpha << "|" << targetAlpha;
			if ( alpha - targetAlpha <= oneStep ) {
				ofLogVerbose ("fade reached 0; load new video");
				alpha = targetAlpha;
				loadVideo ( filenameNextContent );
				targetAlpha = 255;
			}
		}
		else if ( targetAlpha > alpha ) {
			//cout << " + " << alpha << "|" << targetAlpha;
			alpha += oneStep;
			if ( targetAlpha - alpha <= oneStep ) {
				ofLogVerbose ( "fade reached 100%; DONE" );
				alpha = targetAlpha;
			}
		}
		tint = ofFloatColor ( 255,255,255, alpha/255 );
	}
}


void ofxPuffersphereObject::draw(){

	if (isTransparent()) {
		return;
	}

	if(offaxis == NULL){
		ofLog(OF_LOG_ERROR, "ofxPuffersphere --- the shader is null!");
		return;
	}
	if(image == NULL){
		ofLog(OF_LOG_ERROR, "ofxPuffersphere --- the drawable is null!");
	}
	
	offaxis->begin();

	//should be 2:1, like 2100x1050 since the puffersphere we are working with draws to 1050x1050
	offaxis->setUniform2f("canvasDimensions", targetSize.x, targetSize.y);
	offaxis->setUniform2f("textureDimensions", image->getTexture().getWidth(), image->getTexture().getHeight());
	offaxis->setUniform2f("textureScale", scale.x, scale.y);

	ofVec4f quatVec = rotation.asVec4();
	offaxis->setUniform4f("quat", quatVec.x,quatVec.y,quatVec.z,quatVec.w);
	
	ofFloatColor colors[4] = { tint,tint,tint,tint }; //color arrays

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(ofVec2f), &(canvasCoords[0].x));

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, sizeof(ofFloatColor), &(colors[0]).r);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(ofVec2f), &(canvasCoords[0]).x);

	image->getTexture().bind();
	glDrawArrays(GL_QUADS, 0, 4);
	image->getTexture().unbind();

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	offaxis->end();
}


void ofxPuffersphereObject::setTexture(ofBaseHasTexture* newTexture){
	if(image != NULL && textureIsOurs){
		delete image;
	}
	image = newTexture;
	isVideo = false;
	textureIsOurs = false;
}

void ofxPuffersphereObject::loadImage(string filename){
	if(textureIsOurs && image != NULL){
		delete image;
		isVideo = false;
		image = NULL;
	}

	ofImage* newimage = new ofImage();
	if(!newimage->loadImage(filename)){
		ofLog(OF_LOG_ERROR, "ofxPuffersphere --- image couldn't load!");
		delete newimage;
	}
	else{
		textureIsOurs = true;
		isVideo = false;
		image = newimage;
	}
}

void ofxPuffersphereObject::loadVideo(string filename){
	if(textureIsOurs && image != NULL){
		delete image;
	}

	ofVideoPlayer* newvideo = new ofVideoPlayer();
	if(!newvideo->loadMovie(filename)){
		ofLog(OF_LOG_ERROR, "ofxPuffersphere --- video couldn't load! " + filename);
		isVideo = false;
		delete newvideo;
	}
	else{
		textureIsOurs = true;
		ofLog(OF_LOG_VERBOSE, "ofxPuffersphere --- video loaded successfully.");
		newvideo->play();
		isVideo = true;
		image = newvideo;
	}
}

void ofxPuffersphereObject::fadeToVideo ( string filename, int millis ) {
	ofLogVerbose ( "fade , then next video: " + filename );

	filenameNextContent = filename;
	targetAlpha = 0;
}




