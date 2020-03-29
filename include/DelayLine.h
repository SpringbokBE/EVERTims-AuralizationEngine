#ifndef DELAYLINE_H_INCLUDED
#define DELAYLINE_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class DelayLine
{
	public:
    
		DelayLine();
		~DelayLine() {};

		void prepareToPlay(const unsigned int samplesPerBlockExpected, const double sampleRate);
		void setSize(const unsigned int newNumChannels, unsigned int newNumSamples);
		void copyFrom(const unsigned int destChannel, const AudioBuffer<float>& source, const unsigned int sourceChannel, const unsigned int sourceStartSample, const unsigned int numSamples);
		void addFrom(const unsigned int destChannel, const AudioBuffer<float>& source, const unsigned int sourceChannel, const unsigned int sourceStartSample, const unsigned int numSamples);
		void incrementWritePosition(const unsigned int numSamples);
		void _fillBufferWithDelayedChunk(AudioBuffer<float>& destination, const unsigned int destChannel, const unsigned int destStartSample, const unsigned int sourceChannel, const unsigned int delayInSamples, const unsigned int numSamples);
		void fillBufferWithDelayedChunk(AudioBuffer<float>& destination, const unsigned int destChannel, const unsigned int destStartSample, const unsigned int sourceChannel, const float delayInSamples, const float numSamples);
		void clear();

	private:

		int writeIndex;
		int futureLineSize;
		unsigned int localSamplesPerBlockExpected = 0;

		AudioBuffer<float> buffer;
		AudioBuffer<float> chunkBufferPrev;
		AudioBuffer<float> chunkBufferNext;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayLine)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif