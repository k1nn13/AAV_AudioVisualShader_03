#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	//-----OF-SETUP-------//
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofEnableSmoothing();


	//load shaders
	shader.load("shader.vert", "shader.frag");
	shader2.load("shader2.vert", "shader2.frag");

	ofDisableArbTex();

	//load images
	img.load(ofToDataPath("./triangles.jpeg"));//load images
	img.mirror(true, true);//flip images

	//setup 3d primitives
	plane.set(1024, 768, 10, 10);
	plane.setResolution(100, 100);
	plane.setMode(OF_PRIMITIVE_TRIANGLES);


	//allocate fbo size
	fbo.allocate(plane.getWidth(), plane.getHeight(), GL_RGBA);
	fbo1.allocate(plane.getWidth(), plane.getHeight(), GL_RGBA);

	// clear fbo
	fbo.begin();
	ofClear(255, 255, 255, 0);
	fbo.end();

	//clear fbo1
	fbo1.begin();
	ofClear(255, 255, 255, 0);
	fbo1.end();


	gui.setup();
	gui.add(masterGainSlider.set("master gain", 0.5, 0.0, 1));
	gui.add(Threshold_0.set("Threshold 0:", 0.2, 0.0, 1));
	gui.add(Threshold_1.set("Threshold 1:", 0.3, 0.0, 1));
	gui.add(Threshold_2.set("Threshold 2:", 0.4, 0.0, 1));
	gui.add(Threshold_3.set("Threshold 3:", 0.5, 0.0, 1));
	



	audio.load(ofToDataPath("./car_show_room.wav"));

	//---MAXIM-SETUP---//
	fft.setup(1024, 512, 256);
	oct.setup(44100, 1024, 10);
	sampleRate = 44100;
	bufferSize = 512;

	//--SoundStream-Settings--//
	ofSoundStreamListDevices();
	ofSoundStreamSettings settings;
	settings.setApi(ofSoundDevice::MS_ASIO);  //select sound card

	/*
	auto devices = soundStream.getMatchingDevices("default");

	if (devices.empty()) {
		settings.setInDevice(devices[0]);
	}

	settings.setInListener(this);
	settings.setOutListener(this);
	settings.sampleRate = sampleRate;
	*/
	ofSoundStreamSetup(2, 2, this, sampleRate, bufferSize, 4);
	ofxMaxiSettings::setup(sampleRate, 2, bufferSize);
	//=======================================================//
}

//--------------------------------------------------------------
void ofApp::update(){


}

//--------------------------------------------------------------
void ofApp::draw(){

	ofBackground(0);

	fbo.begin();
		img.draw(0, 0, plane.getWidth(), plane.getHeight());
	fbo.end();

	//=============//
    //draw fbo within fbo and apply frag shader
	fbo1.begin();
	ofClear(0, 0, 0, 255);
	shader.begin();
	ofEnableDepthTest();
	shader.setUniform2f("u_resolution", plane.getWidth(), plane.getHeight());
	shader.setUniform2f("u_mouse", ofGetMouseX(), ofGetHeight() - ofGetMouseY());
	shader.setUniform1f("u_time", ofGetElapsedTimef() / 2);
	shader.setUniform1f("iTime", ofGetElapsedTimef() / 2);
	shader.setUniformTexture("imageMask", fbo.getTexture(), 4);
	
	//-------------------------------------//
	if (RMS > .0 && RMS < Threshold_0) {
		visual[0] = 1;
		visual[1] = 0;
		visual[2] = 0;
		visual[3] = 0;
		//std::cout << "true_1" << std::endl;
	} 
	else if (RMS >= Threshold_0 && RMS < Threshold_1) {
		visual[0] = 0;
		visual[1] = 1;
		visual[2] = 0;
		visual[3] = 0;
		//std::cout << "true_2" << std::endl;
	}
	else if (RMS >= Threshold_1 && RMS < Threshold_2) {
		visual[0] = 0;
		visual[1] = 0;
		visual[2] = 1;
		visual[3] = 0;
		//std::cout << "true_3" << std::endl;
	}
	else if (RMS >= Threshold_2 && RMS < Threshold_3) {
		visual[0] = 0;
		visual[1] = 0;
		visual[2] = 0;
		visual[3] = 1;
		//std::cout << "true_4" << std::endl;
	}
	

	//-------------------------------------//
	//setup spiral shader with gui
	shader.setUniform1i("u_spiralFX",visual[3]);
	shader.setUniform3f("u_spiralParam", RMS * 80, RMS*80, RMS * 80);
	shader.setUniform3f("u_spiralParam2", 0, 0, 0);
	//-------------------------------------//
	//setup water effect shader with gui
	shader.setUniform1i("u_waterFX", visual[1]);
	shader.setUniform1i("u_waterAmount", RMS * 30);
	shader.setUniform2f("u_waterSpeed", RMS * 2, RMS * 2);
	//-------------------------------------//
	//setup noise effect shader with gui
	shader.setUniform1i("u_noiseFX", visual[3]);
	shader.setUniform1f("u_noiseDepth", RMS*2000);
	//-------------------------------------//
	//setup cross hatch effect shader with gui
	shader.setUniform1i("u_crossHatchFX", visual[2]);
	shader.setUniform2f("u_coord", RMS*100, RMS*100);
	//-------------------------------------//
	shader.setUniform1i("u_morphFX", visual[0]);
	shader.setUniform2f("u_morphAmount", RMS*10, RMS*10);
	//-------------------------------------//
	fbo.draw(0, 0);
	ofDisableDepthTest();
	shader.end();
	fbo1.end();

	ofEnableDepthTest();
	ofEnableAlphaBlending();
	cam.begin();

	ofPushMatrix();
	fbo1.getTextureReference().bind(); //bind fbo to primtivies/mesh
	shader2.begin();// shader used to as vertex shader
	//-----------------------------//
	//set uniforms for vert shader
	shader2.setUniform1f("time", ofGetElapsedTimef());
	shader2.setUniform1i("u_magnifyingFX", 0);
	shader2.setUniform1i("u_texturedFX", visual[3]);
	shader2.setUniform1f("u_texturedScale", RMS*-300);
	shader2.setUniform1i("u_waveFX", visual[2]);
	shader2.setUniform1f("u_waveDisplacement", RMS*150);
	//----------------------------------------------------------
	//modified from openframeworks example simple vertex displacement
	float cx = ofGetWidth() / 2.0;
	float cy = ofGetHeight() / 2.0;
	// the plane is being positioned in the middle of the screen,
	// so we have to apply the same offset to the mouse coordinates before passing into the shader.
	float mx = mouseX - cx;
	float my = (mouseY - cy)*-1;
	shader2.setUniform1f("mouseRange", 150); // SET A UNIFORM
	shader2.setUniform2f("mousePos", mx, my);  // SET A UNIFORM
	//-----------------------------//

	//select primitive to draw
	triangles = plane.getMesh().getUniqueFaces();
	float angles = (ofGetElapsedTimef() * 0.03);
	float strength = ofMap((sin(angles + 0.25)) * .5f * 5.f, -2.5, 2.5,RMS*10.2, 5);

	for (size_t i = 0; i < triangles.size(); i++) {
		float frc = ofSignedNoise(angles * (float)i * .01, angles * .05) * 0.2;
		faceNormal = triangles[i].getFaceNormal();
		for (int j = 0; j < 3; j++) {
			if (visual[3] == 1) {
				triangles[i].setVertex(j, triangles[i].getVertex(j) + faceNormal * strength * frc);
			}
			else {
				triangles[i].setVertex(j, triangles[i].getVertex(j));
			}
		}
	}
	

	plane.getMesh().setFromTriangles(triangles);
	if (visual[2] == 1) {
		plane.drawWireframe();
	}
	else {
		plane.draw();
	}
	
	
	shader2.end();
	fbo1.getTextureReference().unbind();
	//-----------------------------//
	ofPopMatrix();

	cam.end();
	ofDisableDepthTest();
	ofDisableAlphaBlending();

	gui.draw();

}

//-----------------------AUDIO----------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels) {


	float sum = 0;

	for (int i = 0; i < bufferSize; i++) {

		audioTrack = audio.play() * 4;
		channelMix[0].stereo(audioTrack, outputs_0, 0.5);
		masterMix[0] = (outputs_0[0]);							//add stereo outputs to master mix
		masterMix[1] = (outputs_0[1]);

		//==========MASTER-OUT==============//
		output[ i*nChannels     ] = masterMix[0] * masterGainSlider;
		output[ i*nChannels + 1 ] = masterMix[1] * masterGainSlider;
		//==================================//


		//used for drawing fft
		//-----------PROCESS-FFT------------//
		double fftMix = (masterMix[0] + masterMix[1]) * .5;
		if (fft.process(fftMix)) {
			oct.calculate(fft.magnitudes);
		}
		//----------------------------------//

		sum += fabs(fftMix); //take absolute value of output
	}

	//take average of samples
	average = sum / bufferSize;
	RMS = sqrt(average);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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

//--------------------------------------------------------------
void ofApp::exit() {
	ofSoundStreamClose();
}
