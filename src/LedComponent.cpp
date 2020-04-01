#include "LedComponent.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

LedComponent::LedComponent() :
	state(State::notClipped)
{
	isClipped = false;
	startTimer(timerUpdateIntervalInMs);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void LedComponent::paint(Graphics& g)
// Paint callback associated to this component, draw source image, user head, etc.
{
	int w1 = (getWidth() - 10) / 2;
	int w2 = (getWidth() - 8) / 2;

	g.setColour(Colours::dimgrey);
	g.drawEllipse(w1, w1, 10, 10, 2);

	if (isClipped)
	{
		isClipped = false;
		state = State::clipped;
		nextUnclipTime = Time::getMillisecondCounter() + ledFadeDuration;
	}
	else if (Time::getMillisecondCounter() >= nextUnclipTime)
	{
		state = State::notClipped;
	}

	if(state == State::clipped)
	{
		g.setColour(Colours::red);
		g.fillEllipse(w2, w2, 8, 8);
	}
	else if(state == State::notClipped)
	{
		g.setColour(Colours::dimgrey);
		g.fillEllipse(w2, w2, 8, 8);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void LedComponent::timerCallback()
// get owner clip state
{
	repaint();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////