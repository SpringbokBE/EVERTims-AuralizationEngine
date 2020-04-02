#include "CustomLookAndFeel.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

const Colour CustomLookAndFeel::backgroundColour = Colour(40, 44, 53);
const Colour CustomLookAndFeel::foregroundColour = Colours::whitesmoke;
const Colour CustomLookAndFeel::accentColour = Colours::transparentBlack;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

CustomLookAndFeel::CustomLookAndFeel()
{
	setupDefaultMeterColours();
	//setColour(Label::textColourId, foregroundColour);
	setColour(Label::backgroundColourId, backgroundColour);
	//setColour(TextButton::buttonColourId, accentColour);
	//setColour(ToggleButton::textColourId, foregroundColour);
	//setColour(ComboBox::backgroundColourId, accentColour);
	//setColour(ComboBox::buttonColourId, accentColour);
	//setColour(ComboBox::outlineColourId, accentColour);
	//setColour(ComboBox::textColourId, foregroundColour);
	//setColour(ComboBox::arrowColourId, foregroundColour);
	//setColour(Slider::textBoxBackgroundColourId, accentColour);
	//setColour(Slider::trackColourId, Colours::lightgrey);
	//setColour(Slider::thumbColourId, Colours::white);
	//setColour(Slider::textBoxTextColourId, foregroundColour);
	//setColour(Slider::textBoxOutlineColourId, accentColour);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
