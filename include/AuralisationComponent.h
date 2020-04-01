#ifndef AURALISATIONCOMPONENT_H_INCLUDED
#define AURALISATIONCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "CustomLookAndFeel.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class AuralisationComponent :
	public Component,
	public Button::Listener,
	public ComboBox::Listener,
	public Slider::Listener
{
public:

	AuralisationComponent();
	~AuralisationComponent() {};

private:

	void buttonClicked(Button* button) override; // Button::Listener
	void comboBoxChanged(ComboBox* comboBox) override; // ComboBox::Listener
	void sliderValueChanged(Slider* slider) override; // Slider::Listener

	void paint(Graphics& g) override; // Component
	void resized() override; // Component

	// GUI elements.
	Label labelAuralisation,
		labelDirectPathGain,
		labelEarlyReflectionsGain,
		labelReverbTail,
		labelCrossfadeFactor,
		labelNumFrequencyBands,
		labelSourceDirectivity;

	TextButton buttonSaveRIR,
		buttonClearSourceImage;

	ToggleButton buttonReverbTail,
		buttonDirectToBinaural;

	ComboBox comboNumFrequencyBands,
		comboSourceDirectivity;

	Slider sliderDirectPathGain,
		sliderEarlyReflectionsGain,
		sliderReverbTailGain,
		sliderCrossfadeFactor;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // AURALISATIONCOMPONENT_H_INCLUDED