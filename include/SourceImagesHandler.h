#ifndef SOURCEIMAGESHANDLER_H_INCLUDED
#define SOURCEIMAGESHANDLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "AmbixEncode/AmbixEncoder.h"
#include "BinauralEncoder.h"
#include "FilterBank.h"
#include "ReverbTail.h"
#include "DirectivityHandler.h"
#include "OSCHandler.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class SourceImagesHandler
{
	public:
    
		SourceImagesHandler(){};
		~SourceImagesHandler(){};

		void prepareToPlay(const unsigned int samplesPerBlockExpected, const double sampleRate);
		void getNextAudioBlock(DelayLine* delayLine, AudioBuffer<float>& ambisonicBuffer);
		float getMaxDelayFuture();
		void updateFromOscHandler(OSCHandler& oscHandler);
		void setFilterBankSize(const unsigned int numFreqBands);

		// Sources images
		int numSourceImages = 0;
		float earlyGain = 1.f;
    
		// Octave filter bank
		FilterBank filterBank;
    
		// Reverb tail
		ReverbTail reverbTail;
		bool enableReverbTail;
		float reverbTailGain = 1.0f;
    
		// Direct path to binaural
		int directPathId = -1;
		float directPathGain = 1.0f;
		bool enableDirectToBinaural = true;
    
		// Crossfade mechanism
		float crossfadeStep = 0.1f;
		bool crossfadeOver = true;
    
		// Direct binaural encoding (for direct path only)
		BinauralEncoder binauralEncoder;
    
		// Source / listener directivity
		DirectivityHandler directivityHandler;
    
		// Prepare struct for thread safe update (pointer swap based)
		struct localVariablesStruct
		{
			std::vector<int> ids; // source images indices
			std::vector<float> delays; // in seconds
			std::vector<float> pathLengths; // in meters
			std::vector< Array<float> > absorptionCoefs; // room frequency absorption coefficients
			std::vector< Array<float> > directivityGains; // source directivity gains
			std::vector< Array<float> > ambisonicGains; // buffer for input data
		};
    
		localVariablesStruct *current = new localVariablesStruct();
		localVariablesStruct *future = new localVariablesStruct();
    
	private:

		void updateCrossfade();

		// Audio buffers
		AudioBuffer<float> workingBuffer; // working buffer
		AudioBuffer<float> workingBufferTemp; // 2nd working buffer, e.g. for crossfade mechanism
		AudioBuffer<float> clipboardBuffer; // to be used as local copy of working buffer when working buffer modified in loops
		AudioBuffer<float> bandBuffer; // N band buffer returned by the filterbank for f(freq) absorption
		AudioBuffer<float> tailBuffer; // FDN_ORDER band buffer returned by the FDN reverb tail
		AudioBuffer<float> binauralBuffer; // stereo buffer to handle binaural encoder output
    
		// Miscellaneaous
		double localSampleRate;
		int localSamplesPerBlockExpected;
    
		// Crossfade mechanism
		float crossfadeGain = 0.0;
    
		// Ambisonic encoding
		AmbixEncoder ambisonicEncoder;
		AudioBuffer<float> ambisonicBuffer; // output buffer, N (Ambisonic) channels
    
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SourceImagesHandler)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SOURCEIMAGESHANDLER_H_INCLUDED
