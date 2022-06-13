#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxGui.h"

#define NUMCHANNELS 3


class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();
		//-------------------MAXIM---------------------------------//
		void audioOut(float * output, int bufferSize, int nChannels);
		void exit();
		//---------------------------------------------------------//
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

		//--------------------------------------------------------------------------------
		//--SETUP-SOUND--//
		ofSoundStream soundStream;
		unsigned bufferSize, sampleRate;
		double currentSample;
		double mix_0, mix_1, mix_2;							//channel mix
		double masterMix[2], 
			   outputs_0[2], outputs_1[2], outputs_2[2];	//stereo mix

		maxiSample audio;
		double audioTrack;
		//analysis
		ofxMaxiFFT fft;
		ofxMaxiFFTOctaveAnalyzer oct;
		maxiMix channelMix[NUMCHANNELS];	

		float absValue;
		float average;
		float RMS;

		//--------------------------------------------------------------------------------
		//--GUI--//
		ofxPanel gui;
		//Sound Settings
		ofParameter<double> masterGainSlider;
		ofParameter<float> Threshold_0;
		ofParameter<float> Threshold_1;
		ofParameter<float> Threshold_2;
		ofParameter<float> Threshold_3;

		
		//--------------------------------------------------------------------------------

		ofShader shader, shader2;
		ofFbo fbo, fbo1;
		ofEasyCam cam;
		ofImage img;
		ofPlanePrimitive plane;
		vector<ofMeshFace> triangles;
		glm::vec3 faceNormal;

		double visual[5];
};
