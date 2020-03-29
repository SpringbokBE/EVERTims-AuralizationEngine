#include "DelayLine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

DelayLine::DelayLine()
{
	writeIndex = 0;
	futureLineSize = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DelayLine::prepareToPlay(const unsigned int samplesPerBlockExpected, const double sampleRate)
// local equivalent of prepareToPlay
{
	localSamplesPerBlockExpected = samplesPerBlockExpected;
	buffer.clear();

	chunkBufferPrev.setSize(1, samplesPerBlockExpected);
	chunkBufferNext.setSize(1, samplesPerBlockExpected);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DelayLine::setSize(const unsigned int newNumChannels, unsigned int newNumSamples)
// set delay line size
{
	// update number of channels (split to simplify code reading)
	if (newNumChannels != buffer.getNumChannels())
	{
		buffer.setSize(newNumChannels, buffer.getNumSamples(), true, true);
	}

	// update num samples: increased size -> zero pad at end
	if (newNumSamples > buffer.getNumSamples())
	{
		buffer.setSize(newNumChannels, newNumSamples, true, true);
	}

	// update num samples: decreased size -> flag reduction, will happen next time writeIndex is reset
	else if (newNumSamples < buffer.getNumSamples() && newNumSamples >= localSamplesPerBlockExpected)
	{
		futureLineSize = newNumSamples;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DelayLine::copyFrom(const unsigned int destChannel, const AudioBuffer<float>& source, const unsigned int sourceChannel, const unsigned int sourceStartSample, const unsigned int numSamples)
// Add samples from buffer to delay line (replace)
{
	// make sure delay line is long enough
	jassert(numSamples <= buffer.getNumSamples());

	// either simple copy
	if (writeIndex + numSamples <= buffer.getNumSamples())
	{
		buffer.copyFrom(destChannel, writeIndex, source, sourceChannel, 0, numSamples);
	}

	// or circular copy (last samples of audio buffer will go at delay line buffer begining)
	else
	{
		int numSamplesTail = buffer.getNumSamples() - writeIndex;
		buffer.copyFrom(destChannel, writeIndex, source, sourceChannel, 0, numSamplesTail);
		buffer.copyFrom(destChannel, 0, source, sourceChannel, numSamplesTail, numSamples - numSamplesTail);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DelayLine::addFrom(const unsigned int destChannel, const AudioBuffer<float>& source, const unsigned int sourceChannel, const unsigned int sourceStartSample, const unsigned int numSamples)
// Add samples from buffer to delay line (add)
{
	// make sure delay line is long enough
	jassert(numSamples <= buffer.getNumSamples());

	// either simple copy
	if (writeIndex + numSamples <= buffer.getNumSamples())
	{
		buffer.addFrom(destChannel, writeIndex, source, sourceChannel, 0, numSamples);
	}

	// or circular copy (last samples of audio buffer will go at delay line buffer begining)
	else
	{
		int numSamplesTail = buffer.getNumSamples() - writeIndex;
		buffer.addFrom(destChannel, writeIndex, source, sourceChannel, 0, numSamplesTail);
		buffer.addFrom(destChannel, 0, source, sourceChannel, numSamplesTail, numSamples - numSamplesTail);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DelayLine::incrementWritePosition(const unsigned int numSamples)
// Increment write position, apply circular shift if need be
{
	// delay line size reduction flagged earlier: reduce size now
	if (futureLineSize > 0 && writeIndex >= futureLineSize)
	{
		// update size
		buffer.setSize(buffer.getNumChannels(), futureLineSize, true, true);

		// reset flag
		futureLineSize = 0;
	}

	// increment (modulo) write position
	writeIndex += numSamples;
	writeIndex %= buffer.getNumSamples();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DelayLine::_fillBufferWithDelayedChunk(AudioBuffer<float>& destination, const unsigned int destChannel, const unsigned int destStartSample, const unsigned int sourceChannel, const unsigned int delayInSamples, const unsigned int numSamples)
// Get delayed buffer out of delay line
{

	int writePos = writeIndex - delayInSamples;

	if (writePos < 0)
	{
		writePos = buffer.getNumSamples() + writePos;
		if (writePos < 0) // if after an update the first delay force to go fetch far to far: not best option yet (to set write pointer to zero)
			writePos = 0;
	}

	if ((writePos + numSamples) < buffer.getNumSamples())
	{
		// simple copy
		destination.copyFrom(destChannel, destStartSample, buffer, sourceChannel, writePos, numSamples);
	}
	else
	{
		// circular loop
		int numSamplesTail = buffer.getNumSamples() - writePos;
		destination.copyFrom(destChannel, destStartSample, buffer, sourceChannel, writePos, numSamplesTail);
		destination.copyFrom(destChannel, destStartSample + numSamplesTail, buffer, sourceChannel, 0, numSamples - numSamplesTail);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DelayLine::fillBufferWithDelayedChunk(AudioBuffer<float>& destination, const unsigned int destChannel, const unsigned int destStartSample, const unsigned int sourceChannel, const float delayInSamples, const float numSamples)
// Get interpolated delayed buffer out of delay line (linear interpolation between previous and next)
{
	// get previous and next positions in delay line
	_fillBufferWithDelayedChunk(chunkBufferPrev, 0, 0, sourceChannel, ceil(delayInSamples), numSamples);
	_fillBufferWithDelayedChunk(chunkBufferNext, 0, 0, sourceChannel, floor(delayInSamples), numSamples);

	// apply linear interpolation gains
	float gainPrev = (float)(delayInSamples - floor(delayInSamples));
	chunkBufferPrev.applyGain(gainPrev);
	chunkBufferNext.applyGain(1.f - gainPrev);

	// sum buffer in output
	destination.copyFrom(destChannel, destStartSample, chunkBufferPrev, 0, 0, numSamples);
	destination.addFrom(destChannel, destStartSample, chunkBufferNext, 0, 0, numSamples);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void DelayLine::clear()
// Remove all content from delay line main buffer
{
	buffer.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////