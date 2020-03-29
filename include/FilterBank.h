#ifndef FILTERBANK_H_INCLUDED
#define FILTERBANK_H_INCLUDED

#include <array>
#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utils.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class FilterBank
{
	public:
    
		FilterBank() {};
		~FilterBank() {};

		void prepareToPlay(const unsigned int samplesPerBlockExpected, const double sampleRate);
		void setNumFilters(const unsigned int numBands, const unsigned int numSourceImages);
		void _setNumFilters(const unsigned int numBands, const unsigned int numSourceImages);
		void decomposeBuffer(const AudioBuffer<float>& source, AudioBuffer<float>& destination, const unsigned int sourceImageId);
    
		int numOctaveBands = 0;
		int numIndptStream = 0;

	private:

		double localSampleRate;
		int localSamplesPerBlockExpected;

		int _numOctaveBands = 0;
		int _numIndptStream = 0;
		bool updateRequired = false;

		std::vector<std::array<IIRFilter, NUM_OCTAVE_BANDS - 1>> octaveFilterBanks;

		AudioBuffer<float> bufferFiltered;
		AudioBuffer<float> bufferRemains;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterBank)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // FILTERBANK_H_INCLUDED
