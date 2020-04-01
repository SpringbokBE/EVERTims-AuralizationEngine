#include "LoggingComponent.h"

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

LoggingComponent::LoggingComponent()
{
	// Initialise GUI elements.
	addAndMakeVisible(&labelLogging);
	labelLogging.setText("Logging", dontSendNotification);

	addAndMakeVisible(&ledClipping);
	ledClipping.setAlwaysOnTop(true);

	addAndMakeVisible(&labelClipping);
	labelClipping.setText("Clipping", dontSendNotification);

	addAndMakeVisible(&buttonEnableLogs);
	buttonEnableLogs.addListener(this);
	buttonEnableLogs.setButtonText("Enable logs");
	buttonEnableLogs.setEnabled(true); // Is this required?
	buttonEnableLogs.setToggleState(true, sendNotification);

	addAndMakeVisible(&buttonRecordAmbisonicToDisk);
	buttonRecordAmbisonicToDisk.addListener(this);
	buttonRecordAmbisonicToDisk.setButtonText("Record Ambisonic to disk");
	buttonRecordAmbisonicToDisk.setEnabled(true); // Is this required?
	buttonRecordAmbisonicToDisk.setToggleState(false, sendNotification);

	addAndMakeVisible(&buttonSaveOscState);
	buttonSaveOscState.addListener(this);
	buttonSaveOscState.setButtonText("Save OSC state");
	buttonSaveOscState.setEnabled(true); // Is this required?

	addAndMakeVisible(textLogging);
	textLogging.setMultiLine(true);
	textLogging.setReturnKeyStartsNewLine(true);
	textLogging.setReadOnly(true);
	textLogging.setScrollbarsShown(true);
	textLogging.setCaretVisible(false);
	textLogging.setPopupMenuEnabled(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void LoggingComponent::buttonClicked(Button* button)
{
	auto parent = dynamic_cast<MainComponent*>(getParentComponent());

	if (button == &buttonEnableLogs)
	{
		textLogging.setText(parent->getLogs(button->getToggleState()));
	}
	else if (button == &buttonRecordAmbisonicToDisk)
	{
		parent->enableRecordAmbisonicToDisk(button->getToggleState());
	}
	else if (button == &buttonSaveOscState)
	{
		parent->saveOscState();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void LoggingComponent::updateLoggingText(const String& text)
{
	if (buttonEnableLogs.getToggleState()) textLogging.setText(text);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void LoggingComponent::paint(Graphics& g)
{
	g.setColour(Colours::white);
	g.drawRoundedRectangle(10, 10, getWidth() - 20, getHeight() - 20,
		getParentComponent()->getHeight() / 100, 2.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void LoggingComponent::resized()
{
	int h = (getHeight() - 40) / 5;
	int w = (getWidth() - 40) / 4;

	Font labelFont = labelLogging.getFont();
	labelLogging.setBounds(30, 5, 1.2 * labelFont.getStringWidth(labelLogging.getText()), labelFont.getHeight());

	ledClipping.setBounds(20, 20, h, h);
	labelClipping.setBounds(20 + h, 20, w - h, h);
	buttonEnableLogs.setBounds(20 + w, 20, w, h);
	buttonRecordAmbisonicToDisk.setBounds(20 + 2 * w, 20, w, h);
	buttonSaveOscState.setBounds(pad(20 + 3 * w, 20, w, h, 10, 10));
	textLogging.setBounds(pad(20, 20 + h, 4 * w, 4 * h));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////