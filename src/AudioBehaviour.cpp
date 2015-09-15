//
//  VideoBehaviour.cpp
//  GenericClient
//
//  Created by Palmer on 3/28/13.
//

#include "AudioBehaviour.h"


////////////////////////////////////////////////////////////////////////////////////////
//                                       BEHAVIOR                                     //
////////////////////////////////////////////////////////////////////////////////////////

#define RADIUS      80.0f
#define NUM_POINTS  7

void AudioBehaviour::customSetup (map<int,Pixel*>* pixels, vector<Pixel*>* pixelsFast) {
    
	// inicializando el vector de spheric points
	sphericPoints = vector <SphericPoint*> (NUM_POINTS);
	
	// configuración para los primeros 5
	SphericPointConfig _config[] = {
		{true, true, 1.7f, .99f, -180, 540, .6f, .4f, .8f, .35f, .003f, .01f},    // lento, grande, multicolor (graves)
		{true, true, 1.1f, .99f, -180, 540, .7f, .4f, 2.0f, .3f , .005f, .004f},  // medio (medios)
		{true, true, 0.16f, .87f, -180, 540, .7f, .4f, 3.4f, .4f , .008f, .002f}, // medio (medios)
		{true, true, 0.12, .84f, -180, 540, .7f, .4f, 3.8f, .5f , .026f, .0015f},   // rápido, chico, color fijo (agudos)
		{true, true, 1.5f, .79f, -180, 540, .7f, .4f, 3.9f, .6f , .08f, .001f},   // rápido, chico, color fijo (agudos)
	};

	for (int i=0; i<5; i++) {
		sphericPoints [i] = new SphericPoint (RADIUS, _config[i]);
	}
	
	// los 2 últimos con la configuración por defecto (apagados)
	sphericPoints [5] = new SphericPoint (RADIUS);         
	sphericPoints [6] = new SphericPoint (RADIUS);
	
	fade2black = .5;
	fade2blackType = false;

	setBar ();

}

void AudioBehaviour::update() {
    	
	// first, some decay to black in all pixels
	vector<Pixel*>::iterator it = pixelsFast->begin();
	while (it != pixelsFast->end()) {
		Pixel* px = *it;
		if (fade2blackType) {
			px->fadeToBlack (1-fade2black);
		} else {
			px->blendRGBA (0, 0, 0, 255, fade2black);
		}
		it++;
	}
	
	// grab spectrum data
	float constQ [NUM_POINTS];
	audio.spectrum (constQ, NUM_POINTS);
	
	// now update spheric points with audio data
	for (int i=0; i<NUM_POINTS; i++) {
		if (sphericPoints [i]->config.enabled) {
			sphericPoints [i]->update (sphericPoints [i]->config.audio_enabled? constQ[i] : 1.0);    
		}
	}

	// iterate through spheric points
	vector<SphericPoint*>::iterator ip = sphericPoints.begin();
	while (ip != sphericPoints.end()) {
		SphericPoint* sp = *ip;

		if (sp->config.enabled) {
			//iterate pixels
			vector<Pixel*>::iterator it = this->pixelsFast->begin();
			while (it!=this->pixelsFast->end()) {
				Pixel* px = *it;
				// colorize through this spheric point
				sp->colorize (px);
				it++;
			}
		}

		ip++;
	}
}

void AudioBehaviour::draw() {
	
}

void AudioBehaviour::drawPost() {
	
	bar.draw();

	// draw spectrum
	int leftMargin = 200, bottomMargin = 50;
	float width = (ofGetWidth()-2*leftMargin) / NUM_POINTS;
	
	ofEnableAlphaBlending();
	float c = 255.0/(NUM_POINTS-1);
	for (int i=0; i<NUM_POINTS; i++) {
		ofSetColor (ofColor::fromHsb(i*c, 50, 255, 80));
		ofRect (leftMargin + i*width, ofGetHeight()-bottomMargin, width*.97, -(sphericPoints[i]->energy*sphericPoints[i]->config.gain*75));
	}
	ofDisableAlphaBlending();

}

void AudioBehaviour::keyPressed(int key) {
   
}

void AudioBehaviour::exit() {
    SpecificBehaviour::exit();
}

void AudioBehaviour::setBar () {
	
	bar.init ("Configuration", 255, 510, 200, 200, 200, 100);
	bar.enable ();
	
	TwDefine (" Configuration valueswidth=80 ");
	//TwDefine (" Configuration position='750 310' ");

	// general settings
	bar.addParam ("black", &fade2black, " group='General Settings' label='Fade to black' help='general fade to black' min=0.01 max=0.99 step=0.01 ", false);
	bar.addSeparator ("separator1");
	bar.addParam ("blacktype", &fade2blackType, " group='General Settings' label='Fade to black mode' help='selects RGB or HSV fade to black type' true='HSV' false='RGB' ", false);
	bar.addSeparator ("separator1");
	
	nameStr = new char [10];
	optionStr = new char [170];

	for (int i=0; i<NUM_POINTS; i++) {
	
		SphericPointConfig& _config = sphericPoints[i]->config;

		setBarParam (i, "enable", "Point Enabled", "enable and disable point", _config.enabled);
		setBarParam (i, "audio", "Audio Enabled", "enable and disable audio input", _config.audio_enabled);
		setBarParam (i, "gain", "Gain", "input gain", -5.0, 5.0, 0.01, _config.gain);
		setBarParam (i, "filter", "Filter", "input low pass filter", 0.01, 0.99, 0.01, _config.filter);
		setBarParam (i, "huemin", "HUE min.", "minimum HUE value", -180, 360, 1, _config.hue_min);
		setBarParam (i, "huemax", "HUE max.", "maximum HUE value", 0, 540, 1, _config.hue_max);
		setBarParam (i, "decayh", "Dist. decay HUE", "amount of color change due to distance", -5.0, 5.0, 0.01, _config.distDecayH);
	    setBarParam (i, "decays", "Dist. decay Sat.", "amount of saturation change due to distance", -5.0, 5.0, 0.01, _config.distDecayS);
	    setBarParam (i, "decayv", "Dist. decay Value", "amount of bright change due to distance (size)", -5.0, 5.0, 0.01, _config.distDecayV);
	    setBarParam (i, "blend", "Blend", "blend factor for this point", 0.01, 1.0, 0.01, _config.blendFactor);
	    setBarParam (i, "movstep", "Movement Speed", "actually a step value for the noise signal which controls the position", 0.0, 0.1, 0.0001, _config.movNoiseStep);
	    setBarParam (i, "huestep", "Color change Speed ", "actually a step value for the noise signal which controls HUE", 0.0, 0.1, 0.0001, _config.hueNoiseStep);
		setBarParam (i, "sat", "Saturation", "saturation of the point", 0.0, 1.0, 0.01, sphericPoints[i]->color.S);
		
		// add separator
		sprintf (nameStr, "%i", i);
		bar.addSeparator (nameStr);
		
		// set group closed (all but first)
 		if (i==0) {continue;} 
 		sprintf (optionStr, " Configuration/'Spheric Point / Band %i' opened=false ", i);
		TwDefine (optionStr);
	
	}

	delete nameStr, optionStr;
	
}

// boolean
void AudioBehaviour::setBarParam (int index, const char* name, const char* label, const char* help, bool& var) {
	sprintf (nameStr, "%s%d", name, index);
	sprintf (optionStr, " group='Spheric Point / Band %d' label='%s' help='%s' ", index, label, help);
	bar.addParam (nameStr, &var, optionStr, false);
}

// integer
void AudioBehaviour::setBarParam (int index, const char* name, const char* label, const char* help, int min, int max, int step, int& var) {
	sprintf (nameStr, "%s%d", name, index);
	sprintf (optionStr, " group='Spheric Point / Band %d' label='%s' help='%s' min=%d max=%d step=%d ", index, label, help, min, max, step);
	bar.addParam (nameStr, &var, optionStr, false);
}

// float
void AudioBehaviour::setBarParam (int index, const char* name, const char* label, const char* help, float min, float max, float step, float& var) {
	sprintf (nameStr, "%s%d", name, index);
	sprintf (optionStr, " group='Spheric Point / Band %d' label='%s' help='%s' min=%f max=%f step=%f ", index, label, help, min, max, step);
	bar.addParam (nameStr, &var, optionStr, false);
}


////////////////////////////////////////////////////////////////////////////////////////
//                                    SPHERIC POINT                                   //
////////////////////////////////////////////////////////////////////////////////////////

#define LAT_BASE (-PI/2)
#define LAT_RANGE (2*PI)
#define LNG_BASE -PI
#define LNG_RANGE (4*PI)


ofVec3f SphericPoint::getPosition () {
	ofVec3f p;
	float rsinlat = radius*sin(lat);
	p.x = rsinlat*cos(lng);
	p.y = rsinlat*sin(lng);
	p.z = radius*cos(lat);
	return p;
}

float SphericPoint::getDistance (ofVec3f position) {
	ofVec3f p = getPosition ();
	return sqrt(pow(p.x-position.x,2)+ pow(p.y-position.y,2)+pow(p.z-position.z,2))/(2*radius);
}

void SphericPoint::colorize (Pixel* px) {
	float dist = getDistance (px->getPosition());
	float H = fmod(color.H/360.0 + 1 + config.distDecayH*dist, 1) * 255;
	float S = ofClamp((color.S - config.distDecayS*dist) * 255, 0 , 255);
	float V = ofClamp((color.V - config.distDecayV*dist) * 255, 0 , 255);
	ofColor c = ofColor::fromHsb(H, S, V);
	px->blendRGB(c.r, c.g, c.b, config.blendFactor);
}

void SphericPoint::update (float new_energy) {
	
	energy = config.filter*energy + (1.0-config.filter)*new_energy;     // LPF 

	// update noises
	latNoiseT += (energy * config.gain * config.movNoiseStep);  // *ofRandom(.95, 1.05));    
	lngNoiseT += (energy * config.gain * config.movNoiseStep);  // *ofRandom(.95, 1.05));    
	hueNoiseT += (energy * config.gain * config.hueNoiseStep);
	
	// move & change color point
	lat = LAT_BASE + LAT_RANGE*ofNoise (latNoiseT);       
	lng = LNG_BASE + LNG_RANGE*ofNoise (lngNoiseT);
	color.H = config.hue_min + (config.hue_max-config.hue_min)*ofNoise (hueNoiseT);
	
	// update color
	color.V = energy * config.gain;
}

void SphericPoint::init (float _radius, SphericPointConfig _config) {
	
	radius = _radius;
	config = _config;
	
	// init noises
	latNoiseT = ofRandom(0, 1);
	lngNoiseT = ofRandom(0, 1);
	hueNoiseT = ofRandom(0, 1);
	energy = 0;

	// init color
	color.S = .95;

}

SphericPoint::SphericPoint (float _radius) {
	SphericPointConfig _config;
	
	_config.enabled = false;
	_config.audio_enabled = true;
	_config.gain = 1.0f;
	_config.filter = .99f;
	_config.hue_min = -180;
	_config.hue_max = 540;
	_config.distDecayH = .7f;
	_config.distDecayS = .4f;
	_config.distDecayV = 1.0f;
	_config.blendFactor = .5f;
	_config.movNoiseStep = .003f;
	_config.hueNoiseStep = .015f;	

	init (_radius, _config);    // Defaults
}

SphericPoint::SphericPoint (float _radius, SphericPointConfig _config) {
	init (_radius, _config);
}



////////////////////////////////////////////////////////////////////////////////////////
//                                         AUDIO                                      //
////////////////////////////////////////////////////////////////////////////////////////

AudioInput::AudioInput () {
	// compute log2n 
	int n = FFT_BANDS_NUMBER/2;
	log2n = 0;
	while (!((n>>log2n)&1)) {log2n++;}
	
	// audio
	soundStream.listDevices();
	soundStream.setInput (this);
	soundStream.setup (0, 2, 44100, AUDIO_BUFFER_SIZE, NUM_BUFFERS);
	soundStream.start();
	
}

void AudioInput::audioReceived (float* input, int bufferSize, int nChannels) {
	
	// samples are "interleaved"
	for (int i = 0; i < bufferSize; i++){
		time_domain[i]	= input [i*nChannels];
	}

}

void AudioInput::spectrum (float* constQ, int size) {                   
	
	myfft.powerSpectrum (time_domain, FFT_BANDS_NUMBER, magnitude);

	/*IMPORTANT: size must be <= log2 (FFT_BANDS_NUMBER/2) */
	if (size > log2n) {size = log2n;}
	
	for (int i=0; i<size; i++) {
		constQ[i] = 0;
		for (int j=1<<i; j<1<<(i+1); j++) {
			constQ[i] += magnitude[j];
		}
	}

}