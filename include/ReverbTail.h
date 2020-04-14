#ifndef REVERBTAIL_H_INCLUDED
#define REVERBTAIL_H_INCLUDED

#include <array>

#include "../JuceLibraryCode/JuceHeader.h"
#include "DelayLine.h"
#include "Utils.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class ReverbTail
{
	public:
    
		ReverbTail();
		~ReverbTail() {};

		void prepareToPlay(const unsigned int samplesPerBlockExpected, const double sampleRate);
		void updateInternals(const std::vector<float>& rt60Values);
		void addToBus(const unsigned int busId, const AudioBuffer<float>& source);
		void extractBusToBuffer(AudioBuffer<float>& destination);
		void clear();

		std::vector<float> valuesRT60; // in sec
    
		static const int numOctaveBands = 3;
		static const int MAX_FDN_ORDER = 16;
		static const int fdnOrder = 16;
    
	private:
    
		void updateFdnParameters();
		void defineFdnFeedbackMatrix();

		// Local delay line
		DelayLine<float> delayLine;
    
		// Setup FDN (static FDN order of 16 is max for now)
		std::array<unsigned int, MAX_FDN_ORDER> fdnDelays; // in samples
		std::array<std::array<float, MAX_FDN_ORDER>, numOctaveBands> fdnGains; // S.I.
		std::array<std::array<float, MAX_FDN_ORDER>, MAX_FDN_ORDER> fdnFeedbackMatrix; // S.I.
    
		// Audio buffers
		AudioBuffer<float> reverbBusBuffers; // Working buffer
		AudioBuffer<float> workingBuffer; // Working buffer
		AudioBuffer<float> tailBuffer;
    
		// Miscelanneous.
		double localSampleRate;
		int localSamplesPerBlockExpected;
    
JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbTail)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // REVERBTAIL_H_INCLUDED
