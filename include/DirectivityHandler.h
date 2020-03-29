#ifndef DIRECTIVITYHANDLER_H_INCLUDED
#define DIRECTIVITYHANDLER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include <mysofa.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class DirectivityHandler
{
	public:
    
		DirectivityHandler(){};
		~DirectivityHandler();
  
		void loadFile(const String& filenameStr);
		Array<float> getGains(const double azim, const double elev);
		void printGains(const unsigned int bandId, const unsigned int step);
		struct MYSOFA_EASY* mysofa_open_noNorm(const char* filename, float samplerate, int* filterlength, int* err);
    
	private:
    
		const static int FILTER_LENGTH = 10; // num frequency bands expected
		int filter_length; // num freq bands in file
		float sampleRate = 48000; // dummy, just made it fit .sofa file to avoid resampling

		struct MYSOFA_EASY* sofaEasyStruct;
		bool isLoaded = false; // to know if need is to free struct at exit

		// To comply with libmysofa notations
		float leftIR[FILTER_LENGTH]; // gain real
		float rightIR[FILTER_LENGTH]; // gain imag
		float leftDelay; // dummy
		float rightDelay; // dummy

		Array<float> dirGains; // [gainReal0 gainReal1 .. gainRealFILTER_LENGTH-1 gainImag0 gainImag1 .. gainImagFILTER_LENGTH-1]

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirectivityHandler)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIRECTIVITYHANDLER_H_INCLUDED