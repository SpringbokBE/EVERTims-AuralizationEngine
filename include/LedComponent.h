#ifndef LEDCOMPONENT_H_INCLUDED
#define LEDCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
//#include "MainComponent.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class LedComponent :
	public Component,
	private Timer
{
	public:
    
		LedComponent();
		~LedComponent(){};
    
		void paint(Graphics& g);
		void timerCallback();

		bool isClipped;

	private:

		enum class State {clipped, notClipped};
		State state;

		int timerUpdateIntervalInMs = 30;

		// Led inertia mechanism
		uint32 ledFadeDuration = 1000; // time led stays on after clip, in ms
		uint32 nextUnclipTime = 0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LedComponent)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // LEDCOMPONENT_H_INCLUDED
