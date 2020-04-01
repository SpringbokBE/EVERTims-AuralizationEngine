#ifndef LOGGINGCOPONENT_H_INCLUDED
#define LOGGINGCOPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "LedComponent.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class LoggingComponent :
	public Component,
	public Button::Listener
{
public:

	LoggingComponent();
	~LoggingComponent() {};

	void updateLoggingText(const String& text);

	LedComponent ledClipping;

private:

	void buttonClicked(Button* button) override; // Button::Listener

	void paint(Graphics& g) override; // Component
	void resized() override; // Component

	Label labelLogging,
		labelClipping;

	ToggleButton buttonEnableLogs,
		buttonRecordAmbisonicToDisk;

	TextButton buttonSaveOscState;

	TextEditor textLogging;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // LOGGINGCOPONENT_H_INCLUDED