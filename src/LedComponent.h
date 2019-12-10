#ifndef LEDCOMPONENT_H_INCLUDED
#define LEDCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

class MainContentComponent;

class LedComponent:
public Component,
private Timer
{
    
//==========================================================================
// ATTRIBUTES
    
public:

    bool isClipped;
    
private:
    
    int timerUpdateIntervalInMs = 30;
    MainContentComponent& owner;
    
    // led inertia mechanism
    uint32 ledFadeDuration = 1000; // time led stays on after clip, in ms
    uint32 nextUnclipTime = 0;
    
//==========================================================================
// METHODS
    
public:
    
LedComponent( MainContentComponent& _owner ): owner(_owner)
{
    isClipped = false;
    startTimer( timerUpdateIntervalInMs );
}

~LedComponent(){}
    
// paint callback associated to this component, draw source image, user head, etc.
void paint(Graphics& g)
{
    g.setColour(Colours::dimgrey);
    g.drawEllipse(3.0, 7.0, 8.0, 8.0, 2.0);
    
    // draw led if currently clipped
    if( isClipped )
    {
        g.setColour(Colours::red);
        // update next unclip time
        nextUnclipTime = Time::getMillisecondCounter() + ledFadeDuration;
    }
    else
    {
        // check if time to switch off led reached
        if( Time::getMillisecondCounter() >= nextUnclipTime )
        {
            // reset clip state
            isClipped = false;
        }
    }
    
    g.fillEllipse(3.7, 7.6, 7.0, 7.0);
}

// get owner clip state
void timerCallback(){ repaint(); }

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LedComponent)
    
};

#endif // DELAYLINE_H_INCLUDED
