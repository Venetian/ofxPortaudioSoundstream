/*
 *  ofxPortaudioSoundStream.h
 *  BTrackPortAudioMultipleAnalyser
 *
 *  Created by Andrew on 09/10/2011.
 *  Copyright 2011 QMUL. All rights reserved.
 *
 */

/*
 *      ofxSoundStream.h
 *
 *      v 0.02 March 2010
 *      Arturo Castro & Eduard Prats Molner
 */



#ifndef _ANR_PORTAUDIO_CLASS
#define _ANR_PORTAUDIO_CLASS

#include <iostream>
#include "ofConstants.h"
#include "ofUtils.h"
#include "ofBaseApp.h"
#include "ofEvents.h"
#include "ofMath.h"
//#include "RtAudio.h"
#include "portaudio.h"
#include "ofBaseSoundStream.h"

#include "ofxAudioEvent.h"

//#define SAMPLE_RATE         (44100)
//#define PA_SAMPLE_TYPE      paFloat32
//#define FRAMES_PER_BUFFER   (64)

typedef void PaStream;
struct PaStreamCallbackTimeInfo;
typedef unsigned long PaStreamCallbackFlags;


struct DeviceInformation {
	int ID;
	int maxNumberOfInputs;
	int maxNumberOfOutputs;
	std::string name;
};

class ofxPortaudioSoundStream{
public:
	ofxPortaudioSoundStream();
	virtual ~ofxPortaudioSoundStream();
	void setDeviceId(int deviceID);
//	void setDeviceIdByName(string _deviceName);
	void setup(int nOutputChannels, int nInputChannels, ofBaseApp * OFSA = NULL);
	void setup(int nOutputChannels, int nInputChannels, unsigned int sampleRate, unsigned int bufferSize, unsigned int nBuffers);
	void setup(int nOutputChannels, int nInputChannels, ofBaseApp * OFSA, unsigned int sampleRate, unsigned int bufferSize, unsigned int nBuffers);
	void stop();
	void start();
	void close();
	void listDevices();
	void printDeviceListInformation();
	void terminate();

//	void PrintSupportedStandardSampleRates(
//										   const PaStreamParameters *inputParameters,
//										   const PaStreamParameters *outputParameters );
	
	static int anrPaAudioCallback(const void *inputBuffer,
					   void *outputBuffer,
					   unsigned long bufferSize,
					   const PaStreamCallbackTimeInfo* streamTime,
					   PaStreamCallbackFlags status,
					   void *data );
	
	int 	receiveAudioBuffer(const void *inputBuffer, void *outputBuffer, int bufferSize);
	
	ofEvent<ofxAudioEventArgs> 	audioReceivedEvent;
	ofEvent<ofxAudioEventArgs> 	audioRequestedEvent;
	ofxAudioEventArgs 	audioEventArgs;
	
	vector<DeviceInformation> deviceList;
	
	std::vector<unsigned int> allowedInputSampleRates;
	PaStream	*		audio;

	
//	PaStream *stream;
	PaError err;
	 
	PaStreamParameters inputParameters ;
	PaStreamParameters outputParameters ;
	
	PaDeviceInfo info;
	
	int					deviceID;
	string				deviceName;
	string getDeviceNameFromID(int _deviceID);
	
protected:
	ofBaseApp 	* 		OFSAptr;
	long unsigned long	tickCount;
	int 				nInputChannels;
	int 				nOutputChannels;

	//set the input output here
void 	setInput(ofBaseSoundInput * soundInput);
	void setOutput(ofBaseSoundOutput * soundOutput);
	
	ofBaseSoundInput *  soundInputPtr;
	ofBaseSoundOutput * soundOutputPtr;

	bool streamInitialised;
	
	//string getDeviceName();
	
};

#endif
