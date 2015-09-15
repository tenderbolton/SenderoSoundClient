//
//  AudioBehaviour.h
//  GenericClient
//
//  Created by Palmer on 3/28/13.
//
//

#ifndef AudioBehaviour_
#define AudioBehaviour_

#include <iostream>
#include "SpecificBehaviour.h"
#include "fft.h"
#include "ofxTwBar.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

struct HSVcolor {float H; float S; float V;};       // H in degrees; S & V normalized

struct SphericPointConfig {
	bool enabled;
	bool audio_enabled;
	float gain;
	float filter;
	int hue_min;
	int hue_max;
	float distDecayH;
	float distDecayS;
	float distDecayV;          // inverso del tamaño
	float blendFactor;
	float movNoiseStep;        // velocidad de movimiento
	float hueNoiseStep;        // velocidad de cambio de color
};

class SphericPoint {

  private:
	float radius;
	float lat, lng;                                     // 0<=lat<=PI; 0<=lng<=2*PI
	float latNoiseT, lngNoiseT, hueNoiseT;

	ofVec3f getPosition ();
	float getDistance (ofVec3f position);
	void init (float _radius, SphericPointConfig _config);
	
  public:
	HSVcolor color;
	SphericPointConfig config;
	float energy;

	SphericPoint (float _radius);
	SphericPoint (float _radius, SphericPointConfig _config);
	void colorize (Pixel* px);
	void update (float energy);

};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

#define AUDIO_BUFFER_SIZE  256
#define NUM_BUFFERS        4
#define FFT_BANDS_NUMBER   256

class AudioInput : public ofBaseSoundInput {
  
  private:
	ofSoundStream soundStream;
	void audioReceived (float* input, int bufferSize, int nChannels);
	fft myfft;

	float time_domain [AUDIO_BUFFER_SIZE];
	float magnitude [FFT_BANDS_NUMBER/2];
	int log2n;
	
  public:
	AudioInput ();
	void spectrum (float* data, int size);

};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

class AudioBehaviour : public SpecificBehaviour {
  
  private:
    
	void customSetup(map<int,Pixel*>* pixels, vector<Pixel*>* pixelsFast);
    void update();
    void draw();
    void drawPost();
    void keyPressed(int key);
    void exit();

	// supplementary methods to format options bar
	void setBar ();
	void setBarParam (int index, const char* name, const char* label, const char* help, bool& var);
	void setBarParam (int index, const char* name, const char* label, const char* help, int min, int max, int step, int& var);
	void setBarParam (int i, const char* name, const char* label, const char* help, float min, float max, float step, float& var);

	vector <SphericPoint*> sphericPoints;
	AudioInput audio;
	ofxTwBar bar;
	
	// supplementary strings to format options bar
	char *nameStr, *optionStr;

  public:
	
	float fade2black;
	bool fade2blackType;
	
};


#endif 

