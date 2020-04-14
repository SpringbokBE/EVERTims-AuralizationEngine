#include "DelayLine.h"

#ifdef DELAYLINE_H_INCLUDED

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	// The DelayLine is implemented by means of a (dynamic) circular buffer.
	// Possible optimisations:
	//	- Use a fixed size buffer such that there is less memory overhead, but higher memory usage.
	//	- Round buffer sizes to next power of two, such that the modular arithmetic simplifies.
	//	- Round buffer size to a multiple of the number of samples per (input) block.
	//	- Is it worthy to interpolate between samples instead of rounding to nearest sample?
	// Problem cases: marked with PROBLEM.

template <class T>
DelayLine<T>::DelayLine()
	:_writeIndex(0),
	 _futureSize(0),
	 _samplesPerBlock(0)
{}

///////////////////////////////////////////////////////////////////////////////////////////////////

// Local equivalent of prepareToPlay.

template <class T>
void DelayLine<T>::prepareToPlay(const uint samplesPerBlock,
																 const double sampleRate)
{
	_samplesPerBlock = samplesPerBlock;
	_circularBuffer.clear();

	_prevChunk.setSize(1, _samplesPerBlock);
	_nextChunk.setSize(1, _samplesPerBlock);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

// Resize the DelayLine. If the DelayLine grows in either the number of channels or the number of
// samples, then keep all present data in the buffer. If the number of samples decreases, then the
// DelayLine will postpone the shrink process to the moment the _writeIndex is reaches the
// _futureSize, such that the oldest data is always discarded from the circular buffer.

template <class T>
void DelayLine<T>::setSize(const uint numChannels,
													 const uint numSamples)
{
	if (numChannels != _circularBuffer.getNumChannels())
		_circularBuffer.setSize(numChannels, _circularBuffer.getNumSamples(), true, true);

	if (numSamples > _circularBuffer.getNumSamples())
		_circularBuffer.setSize(numChannels, numSamples, true, true);
	else if (numSamples < _circularBuffer.getNumSamples() && numSamples >= _samplesPerBlock)
		_futureSize = numSamples;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

// Copy samples from the source buffer to the DelayLine (at the current _writeIndex). When there is
// a need to copy more samples than are available in the remaining part of the circular buffer, the
// copy will wrap around.

template <class T>
void DelayLine<T>::copyFrom(const uint destChannel,
														const AudioBuffer<T>& source,
														const uint sourceChannel,
														const uint sourceStartSample,
														const uint numSamples)
{
	jassert(numSamples <= _circularBuffer.getNumSamples());

	// Simple copy: the source buffer doesn't wrap around the circular buffer.
	if (_writeIndex + numSamples <= _circularBuffer.getNumSamples())
	{
		_circularBuffer.copyFrom(destChannel, _writeIndex, source, sourceChannel, 0, numSamples);
	}
	// Advanced copy: the source buffer wraps around the circular buffer.
	else
	{
		int numTailSamples = _circularBuffer.getNumSamples() - _writeIndex;
		_circularBuffer.copyFrom(destChannel, _writeIndex, source, sourceChannel, 0, numTailSamples);
		_circularBuffer.copyFrom(destChannel, 0, source, sourceChannel, numTailSamples, numSamples - numTailSamples);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

// Add samples from the source buffer to the DelayLine (at the current _writeIndex). When there is
// a need to add more samples than are available in the remaining part of the circular buffer, the
// addition will wrap around.

template <class T>
void DelayLine<T>::addFrom(const uint destChannel,
													 const AudioBuffer<T>& source,
													 const uint sourceChannel,
													 const uint sourceStartSample,
													 const uint numSamples)
{
	jassert(numSamples <= _circularBuffer.getNumSamples());

	// Simple copy: the source buffer doesn't wrap around the circular buffer.
	if (_writeIndex + numSamples <= _circularBuffer.getNumSamples())
	{
		_circularBuffer.addFrom(destChannel, _writeIndex, source, sourceChannel, 0, numSamples);
	}
	// Advanced copy: the source buffer wraps around the circular buffer.
	else
	{
		int numTailSamples = _circularBuffer.getNumSamples() - _writeIndex;
		_circularBuffer.addFrom(destChannel, _writeIndex, source, sourceChannel, 0, numTailSamples);
		_circularBuffer.addFrom(destChannel, 0, source, sourceChannel, numTailSamples, numSamples - numTailSamples);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

// Update the _writeIndex, eventually resize the buffer when the _writeIndex reaches _futureSize,
// because then the oldest data will correctly be discarded from the circular buffer.

template <class T>
void DelayLine<T>::incrementWriteIndex(const uint numSamples)
{
	if (_futureSize > 0 && _writeIndex >= _futureSize)
	{
		_circularBuffer.setSize(_circularBuffer.getNumChannels(), _futureSize, true, true);
		_futureSize = 0;
	}

	_writeIndex += numSamples;
	_writeIndex %= _circularBuffer.getNumSamples();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

// Get a delayed buffer out of the DelayLine. Number of samples and delay are positive and integer.

template <class T>
void DelayLine<T>::fillBufferWithDelayedChunk(AudioBuffer<T>& dest,
																							const uint destChannel,
																							const uint destStartSample,
																							const uint sourceChannel,
																							const uint delayInSamples,
																							const uint numSamples) const
{

	int readPos = _writeIndex - delayInSamples;

	if (readPos < 0)
	{
		readPos += _circularBuffer.getNumSamples();
		// PROBLEM : readPos can become negative even after this, when the delay is sufficiently high. Temporary solution.
		if (readPos < 0) readPos = 0;
	}

	// Simple copy: the dest buffer doesn't wrap around the circular buffer.
	if ((readPos + numSamples) < _circularBuffer.getNumSamples())
	{
		dest.copyFrom(destChannel, destStartSample, _circularBuffer, sourceChannel, readPos, numSamples);
	}
	// Advanced copy: the source buffer wraps around the circular buffer.
	else
	{
		int numTailSamples = _circularBuffer.getNumSamples() - readPos;
		dest.copyFrom(destChannel, destStartSample, _circularBuffer, sourceChannel, readPos, numTailSamples);
		dest.copyFrom(destChannel, destStartSample + numTailSamples, _circularBuffer, sourceChannel, 0, numSamples - numTailSamples);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

// Get a delayed buffer out of the DelayLine. Applies linear interpolation between the two closest
// buffers, since the number of samples and the delay need not to be integer.

template <class T>
void DelayLine<T>::fillBufferWithPreciselyDelayedChunk(AudioBuffer<T>& dest,
																											 const uint destChannel,
																											 const uint destStartSample,
																											 const uint sourceChannel,
																											 const T delayInSamples,
																											 const uint numSamples)
{
	fillBufferWithDelayedChunk(_prevChunk, 0, 0, sourceChannel, static_cast<uint>(delayInSamples + 1), numSamples);
	fillBufferWithDelayedChunk(_nextChunk, 0, 0, sourceChannel, static_cast<uint>(delayInSamples), numSamples);

	// Apply linear interpolation gains and sum up the result in the dest buffer.
	T prevGain = delayInSamples - floor(delayInSamples);
	_prevChunk.applyGain(prevGain);
	_nextChunk.applyGain(static_cast<T>(1) - prevGain);
	
	dest.copyFrom(destChannel, destStartSample, _prevChunk, 0, 0, numSamples);
	dest.addFrom(destChannel, destStartSample, _nextChunk, 0, 0, numSamples);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

// Clear the DelayLine.

template <class T>
void DelayLine<T>::clear()
{
	_circularBuffer.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DELAYLINE_H_INCLUDED