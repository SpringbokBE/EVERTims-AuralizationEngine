#ifndef BINAURALENCODER_H_INCLUDED
#define BINAURALENCODER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "FIRFilter/FIRFilter.h"

#define HRIR_LENGTH 200 // Length of loaded HRIR (in time samples)
#define AZIM_STEP 5.0f // HRIR spatial grid step
#define ELEV_STEP 5.0f // HRIR spatial grid step
#define N_AZIM_VALUES int(360 / AZIM_STEP) // total number of azimuth values in HRIR
#define N_ELEV_VALUES int(180 / ELEV_STEP) + 1 // total number of elevation values in HRIR

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class BinauralEncoder
{
	public:
    
		BinauralEncoder();
		~BinauralEncoder(){};

		void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
		void encodeBuffer(const AudioBuffer<float>& source, AudioBuffer<float>& destination);
		void updateCrossfade();
		void setPosition(double azim, double elev);
    
    float crossfadeStep = 0.1f;

	private:

		void loadHrir(const File& hrirFile);
    
		// Buffer to hold the HRIR for a given position
		using HrirBuffer = std::array<std::array<float, HRIR_LENGTH>, 2 >;

		// Holds all HRIR as loaded from HRIR
		std::map<int, std::array<HrirBuffer, N_ELEV_VALUES>> hrirDict;

		// Current HRIR data.
		HrirBuffer hrir;

		// HRIR FIR filters.
		FIRFilter hrirFir[2];
		FIRFilter hrirFirFuture[2];

		// Array holding index for HRIR linear interpolation.
		std::array<int, 2> azimId;
		std::array<int, 2> elevId;

		// Stereo output buffer.
		AudioBuffer<float> workingBuffer;

		// Miscelanneous.
		double localSampleRate;
		int localSamplesPerBlockExpected;
		float crossfadeGain = 0.f;
		bool crossfadeOver = true;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BinauralEncoder)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BINAURALENCODER_H_INCLUDED