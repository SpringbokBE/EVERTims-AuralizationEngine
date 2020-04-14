#ifndef DELAYLINE_H_INCLUDED
#define DELAYLINE_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utils.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class DelayLine
{
	public:
    
		DelayLine();
		~DelayLine(){};

		void prepareToPlay(const uint, const double);
		void setSize(const uint, const uint);
		void copyFrom(const uint, const AudioBuffer<T>&, const uint, const uint, const uint);
		void addFrom(const uint, const AudioBuffer<T>&, const uint, const uint, const uint);
		void incrementWriteIndex(const uint);
		void fillBufferWithDelayedChunk(AudioBuffer<T>&, const uint, const uint, const uint, const uint, const uint) const;
		void fillBufferWithPreciselyDelayedChunk(AudioBuffer<T>&, const uint, const uint, const uint, const T, const uint);
		void clear();

	private:

		int _writeIndex, _futureSize;
		uint _samplesPerBlock;
		AudioBuffer<T> _circularBuffer, _prevChunk, _nextChunk;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayLine)
};

#include "DelayLine.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif