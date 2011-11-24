/*
 *  ofxPortaudioSoundStream.cpp
 *  BTrackPortAudioMultipleAnalyser
 *
 *  Created by Andrew on 09/10/2011.
 *  Copyright 2011 QMUL. All rights reserved.
 *
 */

#include "ofxPortaudioSoundStream.h"
//#include "ofPaSoundStream.h"

typedef float SAMPLE;

//static int gNumNoInputs = 0;

//TO DO

//Go through ofxRtAudioClass
//check there may be cool functions we can keep

//check that we can do input and output here

//clean up the setup routiune

//look at start and stop commands

//get neames of the devices


//------------------------------------------------------------------------------
ofxPortaudioSoundStream::ofxPortaudioSoundStream(){
	OFSAptr 		= NULL;
	audio 			= NULL;
	nInputChannels	= 0;
	nOutputChannels = 0;
	deviceID		= 0;
	deviceName		= "";
	
	deviceID		= -1;
	soundOutputPtr	= NULL;
	soundInputPtr	= NULL;
	tickCount= 0;

	streamInitialised = false;
	
	if(!streamInitialised){
		PaError err;
		err = Pa_Initialize();
		if( err != paNoError ){
			ofLog(OF_LOG_ERROR,"PortAudio error: %s\n",Pa_GetErrorText( err ));
		}else{
			streamInitialised = true;
			cout << "PortAudio launched in OF" << endl;
		}
	}
	
	
	listDevices();
	printDeviceListInformation();
	printf("sound stream setup finished\n");

	err = Pa_Initialize();
}

//------------------------------------------------------------------------------
ofxPortaudioSoundStream::~ofxPortaudioSoundStream(){
	std::cout << "destructor of anr port audio class called" << endl;
	
	close();
	
	if (streamInitialised){
	//call this only when the Pa_initialize fn was successful
	//terminate();
	streamInitialised = false;
	}
	//don't quite know what to do with pointers so setting them null below:
/*
	OFSAptr 		= NULL;
	audio 			= NULL;
	soundOutputPtr	= NULL;
	soundInputPtr	= NULL;	
*/
 }


//------------------------------------------------------------------------------
void ofxPortaudioSoundStream::setDeviceId(int _deviceID){
	if(audio && deviceID != _deviceID){
		//ofLog(OF_ERROR,"cannot change device with stream already setup");
		cout << "ERROR: cannot change device with stream already setup" << endl;
	}
	else{
		cout << "ANR PA Device ID set to : " << _deviceID << endl;
		deviceID = _deviceID;
		deviceName = getDeviceNameFromID(_deviceID);
	}

	
//	deviceName = audio->PaDeviceIndex;//;//	getDeviceName();
}

string ofxPortaudioSoundStream::getDeviceNameFromID(int _deviceID){
	string name;
	const PaDeviceInfo* infoPtr;
	infoPtr = Pa_GetDeviceInfo(_deviceID);
	if (infoPtr != NULL){
		string deviceName;
		name = string(infoPtr->name);
		cout << "Audio device name is " << name;
	}else{
		cout << "null pointer from name" << endl;
	}
	return name;
}

/*
 NOT IMPLEMENTED YET
//------------------------------------------------------------------------------
void ofxPortaudioSoundStream::setDeviceIdByName(string _deviceName){
	PaStream *audioTemp = 0;
	bool found = false;
	try {
		audioTemp = new PaStream();//RtAudio();
	} catch (PaError &error){//RtError &error) {
		error.printMessage();
	}
 	int devices = audioTemp->getDeviceCount();
//	RtAudio::DeviceInfo info;
	PaStream::PaDeviceInfo info;
	
	for(int i=0;i<devices;i++){
		info = audioTemp->getDeviceInfo(i);
		if(info.name == _deviceName){
			found = true;
			if(audio && deviceID != i){
				//ofLog(OF_ERROR,"cannot change device with stream already setup");
				cout << "ERROR: cannot change device with stream already setup" << endl;
			}
			else{
				deviceID = i;
				deviceName = _deviceName;
				cout << "ANR PA Device ID set to : " << i << endl;
			}
			break;
		}
	}
	
	if(!found){
		cout << "ANR PA Device Name not found!" << endl;
	}
	
	// free memory
	delete audioTemp;
}
*/
//---------------------------------------------------------
void ofxPortaudioSoundStream::setup(int nOutputs, int nInputs, ofBaseApp * OFSA){
	setup(nOutputs, nInputs, OFSA, 44100, 256, 4);
}

//---------------------------------------------------------
void ofxPortaudioSoundStream::setup(int nOutputs, int nInputs, unsigned int sampleRate, unsigned int bufferSize, unsigned int nBuffers){
	setup(nOutputs, nInputs, NULL, sampleRate, bufferSize, nBuffers);
}

//---------------------------------------------------------
void ofxPortaudioSoundStream::setup(int nOutputs, int nInputs, ofBaseApp * OFSA, unsigned int sampleRate, unsigned int bufferSize, unsigned int nBuffers){

	//FROM PORTAUDIO WIKI
	//err = Pa_Initialize();
	//taken to inialised routine

	cout << "ANR_PA_class: setting up with "<<nInputs << "inputs "<< " and "<<nOutputs<< " outputs and buffersize "<< bufferSize << endl;
	
	if( err != paNoError ) goto errorLine;

	
	if (nInputs > 0){
		setInput(OFSA);
		
		//inputParameters = new PaStreamParameters;
		inputParameters.device = deviceID;
		inputParameters.channelCount = nInputs;
		inputParameters.sampleFormat = paFloat32;
		
			if (inputParameters.device >= 0){
			inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
			}else{
				cout <<"NO LATENCY PROBLEM" << endl;
			}

		
		inputParameters.hostApiSpecificStreamInfo = NULL;
		cout << "set up input with "<<inputParameters.channelCount <<" channels"<<endl;
	}
	
	if (nOutputs > 0){
		setOutput(OFSA);
	//	outputParameters = new PaStreamParameters;
		outputParameters.device = deviceID;
		outputParameters.channelCount = nOutputs;
		outputParameters.sampleFormat = paFloat32;
		if (outputParameters.device >= 0){
			outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowInputLatency;
		}
	else{
		cout << "suggested output latency error!!! - " << endl;
		}
		
		
		outputParameters.hostApiSpecificStreamInfo = NULL;
		cout << "set up output with "<<outputParameters.channelCount <<" channels"<<endl;
	}

	cout << "starting stream " << endl;
	
	nInputChannels 		=  nInputs;
	nOutputChannels 	=  nOutputs;
	OFSAptr 			=  OFSA;
	
	bufferSize = ofNextPow2(bufferSize);

	//could add in flag
	//	streamOptions.numberOfBuffers = nBuffers;
	//	streamOptions.flags = paNoFlag;// RTAUDIO_SCHEDULE_REALTIME; - from RtAudio version
	//	streamOptions.priority = 1;
	
	err = Pa_OpenStream( &audio,
						nInputs==0?NULL:&inputParameters,
						nOutputs==0?NULL:&outputParameters,
						sampleRate,
						bufferSize,
						paNoFlag,
						&anrPaAudioCallback,//ofxPortaudioSoundStreamCallback,
						this );

	cout << "error text from opening is " << Pa_GetErrorText( err ) << endl;
	
	err = Pa_StartStream( audio );
	if( err != paNoError ){
		ofLog(OF_LOG_ERROR,"PortAudio error: %s\n",Pa_GetErrorText( err ));
	}
	
	
	return;	
errorLine:
	cout << "got an error"<< endl;
	return;
errorLineNoDevice:
	cout << "got an error there's no device"<< endl;
	return;
errorLineOutput:
	cout << "got an error there's on output"<< endl;
	return;
	

	
}

//---------------------------------------------------------
void ofxPortaudioSoundStream::stop(){
	
	if(!audio){
		cout << "ERROR: call setup first" << endl;
		return;
	}
	PaError err = Pa_StopStream(audio);
	
	if (err != paNoError)
		ofLog(OF_LOG_ERROR,"PortAudio stop, error: %s\n",Pa_GetErrorText( err ));
	
}



//---------------------------------------------------------
void ofxPortaudioSoundStream::start(){
	if(!audio){
		cout << "ERROR: call setup first" << endl;
		return;
	}
	PaError err = Pa_StartStream(audio);
	if (err != paNoError)
		ofLog(OF_LOG_ERROR,"PortAudio start, error: %s\n",Pa_GetErrorText( err ));
	
}


//---------------------------------------------------------
void ofxPortaudioSoundStream::close(){

	int err = Pa_AbortStream(audio);

	if( err != paNoError )
    	ofLog(OF_LOG_ERROR,"PortAudio close, error: %s\n",Pa_GetErrorText( err ));
	else 
		cout << "Closed PA stream successfully" << endl;
	
		audio = NULL;
}

void ofxPortaudioSoundStream::terminate(){

	int err = Pa_Terminate() ;
	if( err != paNoError ){
		cout << "Terminated portaudio stream" << endl;
	}else{
		cout << "terminate had error" << Pa_GetErrorText(err) << endl;
	}
	
}

int ofxPortaudioSoundStream::receiveAudioBuffer(const void *inputBuffer, void *outputBuffer, int bufferSize){
	// 	rtAudio uses a system by which the audio
	// 	can be of different formats
	// 	char, float, etc.
	// 	we choose float
	
	//cout<<"calling here"<<endl;
	
	float * fPtrOut = (float *)outputBuffer;
	float * fPtrIn = (float *)inputBuffer;
	// [zach] memset output to zero before output call
	// this is because of how rtAudio works: duplex w/ one callback
	// you need to cut in the middle. if the simpleApp
	// doesn't produce audio, we pass silence instead of duplex...
	
	
	if (nInputChannels > 0){
		OFSAptr->audioReceived(fPtrIn, bufferSize, nInputChannels);
		
#ifdef OF_USING_POCO
		audioEventArgs.buffer = fPtrIn;
		audioEventArgs.bufferSize = bufferSize;
		audioEventArgs.nChannels = nInputChannels;
		audioEventArgs.deviceID = deviceID;
		audioEventArgs.deviceName = deviceName;
		ofNotifyEvent( audioReceivedEvent, audioEventArgs, this);
#endif
		memset(fPtrIn, 0, bufferSize * nInputChannels * sizeof(float));
	}
	
	
	if (nOutputChannels > 0) {
		OFSAptr->audioRequested(fPtrOut, bufferSize, nOutputChannels);
#ifdef OF_USING_POCO
		audioEventArgs.buffer = fPtrOut;
		audioEventArgs.bufferSize = bufferSize;
		audioEventArgs.nChannels = nOutputChannels;
		audioEventArgs.deviceID = deviceID;
		audioEventArgs.deviceName = deviceName;
		ofNotifyEvent( audioRequestedEvent, audioEventArgs, this);
#endif
	}

	
	return 0;
}

void ofxPortaudioSoundStream::setInput(ofBaseSoundInput * soundInput){
	soundInputPtr = soundInput;
}

void ofxPortaudioSoundStream::setOutput(ofBaseSoundOutput * soundOutput){
	soundOutputPtr = soundOutput;
}


//------------------------------------------------------------------------------
int ofxPortaudioSoundStream::anrPaAudioCallback(const void *inputBuffer,
									 void *outputBuffer,
									 unsigned long bufferSize,
									 const PaStreamCallbackTimeInfo* streamTime,
									 PaStreamCallbackFlags status,
									 void *data ){
	
	ofxPortaudioSoundStream* instance = (ofxPortaudioSoundStream*) data;
	
	//THIS LINE WAS IN BUT GETS CALLED A LOT! - NEED TO INVESTIGATE WHY
	if ( status ) std::cout << "Stream over/underflow detected." << std::endl;
	
	// 	rtAudio uses a system by which the audio
	// 	can be of different formats
	// 	char, float, etc.
	// 	we choose float
	float * fPtrOut = (float *)outputBuffer;
	float * fPtrIn = (float *)inputBuffer;
	// [zach] memset output to zero before output call
	// this is because of how rtAudio works: duplex w/ one callback
	// you need to cut in the middle. if the simpleApp
	// doesn't produce audio, we pass silence instead of duplex...
	

		instance->receiveAudioBuffer(fPtrIn, fPtrOut, bufferSize);
		instance->tickCount++;
	
	/*
	if (instance->nInputChannels > 0 && instance->soundInputPtr){
		// send incoming data to all the sinks
		instance->soundInputPtr->audioIn( fPtrIn, bufferSize, instance->nInputChannels, instance->deviceID, instance->tickCount );		
//		instance->soundInputPtr->audioReceived(fPtrIn, bufferSize, instance->nInputChannels );
		
		memset(fPtrIn, 0, bufferSize * instance->nInputChannels * sizeof(float));
			cout << "called instance audio recieved"<<endl;
	}
	else{
	cout << "FAILED at clled instance audio recieved"<<endl;
	}
	 */
	
	// increment tick count

	/*


	
	if (instance->nInputChannels > 0 && instance->soundInputPtr){
		// send incoming data to all the sinks
		instance->soundInputPtr->audioIn( fPtrIn, bufferSize, instance->nInputChannels, instance->deviceID, instance->tickCount );
		
		memset(fPtrIn, 0, bufferSize * instance->nInputChannels * sizeof(float));
	}
	
	
	if (instance->nOutputChannels > 0) {
		memset( fPtrOut, 0, sizeof(float)*bufferSize*instance->nOutputChannels );
		if(instance->soundOutputPtr){
			instance->soundOutputPtr->audioOut( fPtrOut, bufferSize, instance->nOutputChannels, instance->deviceID, instance->tickCount );
		}
	}
	*/
	return 0;
}



/*
//------------------------------------------------------------------------------
string ofxPortaudioSoundStream::getDeviceName(){
//	RtAudio *audioTemp = 0;
	PaStream *audioTemp = 0;
	bool found = false;
	try {
		audioTemp = new PaStream();//RtAudio();
	} catch (PaError &error){//RtError &error) {
		error.printMessage();
	}
 	int devices = audioTemp->getDeviceCount();
	PaStream::DeviceInfo info;
//	RtAudio::DeviceInfo info;
	info = audioTemp->getDeviceInfo(deviceID);
	
	// free memory
	delete audioTemp;
	
	return info.name;
}
 */

/*******************************************************************/
static void PrintSupportedStandardSampleRates(
											  const PaStreamParameters *inputParameters,
											  const PaStreamParameters *outputParameters )
{
    static double standardSampleRates[] = {
        8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
        44100.0, 48000.0, 88200.0, 96000.0, 192000.0, -1 /* negative terminated  list */
    };
    int     i, printCount;
    PaError err;
	
    printCount = 0;
    for( i=0; standardSampleRates[i] > 0; i++ )
    {
        err = Pa_IsFormatSupported( inputParameters, outputParameters, standardSampleRates[i] );
        if( err == paFormatIsSupported )
        {
            if( printCount == 0 )
            {
                printf( "\t%8.2f", standardSampleRates[i] );
                printCount = 1;
            }
            else if( printCount == 4 )
            {
                printf( ",\n\t%8.2f", standardSampleRates[i] );
                printCount = 1;
            }
            else
            {
                printf( ", %8.2f", standardSampleRates[i] );
                ++printCount;
            }
        }
    }
    if( !printCount )
        printf( "None\n" );
    else
        printf( "\n" );
}


void ofxPortaudioSoundStream::listDevices(){
//begin devices
	//this is straight from the portaudio example
	//pa_devs.c

		int     i, numDevices, defaultDisplayed;
		const   PaDeviceInfo *deviceInfo;
		PaStreamParameters inputParameters, outputParameters;
		PaError err;
		
		
//		Pa_Initialize(); - moved to creation
		
		printf( "PortAudio version number = %d\nPortAudio version text = '%s'\n",
			   Pa_GetVersion(), Pa_GetVersionText() );
		
		
		numDevices = Pa_GetDeviceCount();
		if( numDevices < 0 )
		{
			printf( "ERROR: Pa_GetDeviceCount returned 0x%x\n", numDevices );
			err = numDevices;
			goto error;
		}
		
		printf( "Number of devices = %d\n", numDevices );
		for( i=0; i<numDevices; i++ )
		{
			deviceInfo = Pa_GetDeviceInfo( i );
			printf( "--------------------------------------- device #%d\n", i );
			
			/* Mark global and API specific default devices */
			defaultDisplayed = 0;
			if( i == Pa_GetDefaultInputDevice() )
			{
				printf( "[ Default Input" );
				defaultDisplayed = 1;
			}
			else if( i == Pa_GetHostApiInfo( deviceInfo->hostApi )->defaultInputDevice )
			{
				const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo( deviceInfo->hostApi );
				printf( "[ Default %s Input", hostInfo->name );
				defaultDisplayed = 1;
			}
			
			if( i == Pa_GetDefaultOutputDevice() )
			{
				printf( (defaultDisplayed ? "," : "[") );
				printf( " Default Output" );
				defaultDisplayed = 1;
			}
			else if( i == Pa_GetHostApiInfo( deviceInfo->hostApi )->defaultOutputDevice )
			{
				const PaHostApiInfo *hostInfo = Pa_GetHostApiInfo( deviceInfo->hostApi );
				printf( (defaultDisplayed ? "," : "[") );                
				printf( " Default %s Output", hostInfo->name );
				defaultDisplayed = 1;
			}
			
			if( defaultDisplayed )
				printf( " ]\n" );
			
			/* print device info fields */
			printf( "Name                        = %s\n", deviceInfo->name );
			printf( "Host API                    = %s\n",  Pa_GetHostApiInfo( deviceInfo->hostApi )->name );
			printf( "Max inputs = %d", deviceInfo->maxInputChannels  );
			printf( ", Max outputs = %d\n", deviceInfo->maxOutputChannels  );
			
			printf( "Default low input latency   = %8.3f\n", deviceInfo->defaultLowInputLatency  );
			printf( "Default low output latency  = %8.3f\n", deviceInfo->defaultLowOutputLatency  );
			printf( "Default high input latency  = %8.3f\n", deviceInfo->defaultHighInputLatency  );
			printf( "Default high output latency = %8.3f\n", deviceInfo->defaultHighOutputLatency  );
			
			//added by anr to hold this info
			DeviceInformation listInfo;
			listInfo.name = deviceInfo->name;
			listInfo.ID = i;
			listInfo.maxNumberOfInputs = deviceInfo->maxInputChannels;
			listInfo.maxNumberOfOutputs = deviceInfo->maxOutputChannels;
			deviceList.push_back(listInfo);
			//
			
#ifdef WIN32
#if PA_USE_ASIO
			/* ASIO specific latency information */
			if( Pa_GetHostApiInfo( deviceInfo->hostApi )->type == paASIO ){
				long minLatency, maxLatency, preferredLatency, granularity;
				
				err = PaAsio_GetAvailableLatencyValues( i,
													   &minLatency, &maxLatency, &preferredLatency, &granularity );
				
				printf( "ASIO minimum buffer size    = %ld\n", minLatency  );
				printf( "ASIO maximum buffer size    = %ld\n", maxLatency  );
				printf( "ASIO preferred buffer size  = %ld\n", preferredLatency  );
				
				if( granularity == -1 )
					printf( "ASIO buffer granularity     = power of 2\n" );
				else
					printf( "ASIO buffer granularity     = %ld\n", granularity  );
			}
#endif /* PA_USE_ASIO */
#endif /* WIN32 */
			
			printf( "Default sample rate         = %8.2f\n", deviceInfo->defaultSampleRate );
			
			/* poll for standard sample rates */
			inputParameters.device = i;
			inputParameters.channelCount = deviceInfo->maxInputChannels;
			inputParameters.sampleFormat = paInt16;
			inputParameters.suggestedLatency = 0; /* ignored by Pa_IsFormatSupported() */
			inputParameters.hostApiSpecificStreamInfo = NULL;
			
			outputParameters.device = i;
			outputParameters.channelCount = deviceInfo->maxOutputChannels;
			outputParameters.sampleFormat = paInt16;
			outputParameters.suggestedLatency = 0; /* ignored by Pa_IsFormatSupported() */
			outputParameters.hostApiSpecificStreamInfo = NULL;
			
			if( inputParameters.channelCount > 0 )
			{
				printf("Supported standard sample rates\n for half-duplex 16 bit %d channel input = \n",
					   inputParameters.channelCount );
				PrintSupportedStandardSampleRates( &inputParameters, NULL );
			}
			
			if( outputParameters.channelCount > 0 )
			{
				printf("Supported standard sample rates\n for half-duplex 16 bit %d channel output = \n",
					   outputParameters.channelCount );
				PrintSupportedStandardSampleRates( NULL, &outputParameters );
			}
			
			if( inputParameters.channelCount > 0 && outputParameters.channelCount > 0 )
			{
				printf("Supported standard sample rates\n for full-duplex 16 bit %d channel input, %d channel output = \n",
					   inputParameters.channelCount, outputParameters.channelCount );
				PrintSupportedStandardSampleRates( &inputParameters, &outputParameters );
			}
		}
		
//		Pa_Terminate();
		
		printf("----------------------------------------------\n");
		return ;
		
	error:
		//Pa_Terminate();
		fprintf( stderr, "An error occured while using the portaudio stream\n" );
		fprintf( stderr, "Error number: %d\n", err );
		fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
	return;// err;
		
//end devices

}


void ofxPortaudioSoundStream::printDeviceListInformation(){
	for (int i = 0;i < deviceList.size();i++){
		printf("Device ID %d: %s\n", deviceList[i].ID, (deviceList[i].name).c_str());
	}
}



