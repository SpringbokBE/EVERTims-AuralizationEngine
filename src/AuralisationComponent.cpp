#include "AuralisationComponent.h"

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"
#include "Utils.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

AuralisationComponent::AuralisationComponent()
{
	// Initialise GUI elements.
	addAndMakeVisible(&labelAuralisation);
	labelAuralisation.setText("Auralisation", dontSendNotification);
	
	addAndMakeVisible(&labelDirectPathGain);
	labelDirectPathGain.setText("Direct path gain", dontSendNotification);
	labelDirectPathGain.setJustificationType(Justification::right);

	addAndMakeVisible(&labelEarlyReflectionsGain);
	labelEarlyReflectionsGain.setText("Early reflections gain", dontSendNotification);
	labelEarlyReflectionsGain.setJustificationType(Justification::right);
	
	addAndMakeVisible(&labelReverbTail);
	labelReverbTail.setText("Reverb tail gain", dontSendNotification);
	labelReverbTail.setJustificationType(Justification::right);

	addAndMakeVisible(&labelCrossfadeFactor);
	labelCrossfadeFactor.setText("Crossfade factor", dontSendNotification);
	labelCrossfadeFactor.setJustificationType(Justification::right);
	
	addAndMakeVisible(&labelNumFrequencyBands);
	labelNumFrequencyBands.setText("Frequency bands", dontSendNotification);

	addAndMakeVisible(&labelSourceDirectivity);
	labelSourceDirectivity.setText("Source directivity", dontSendNotification);

	addAndMakeVisible(&buttonSaveRIR);
	buttonSaveRIR.addListener(this);
	buttonSaveRIR.setButtonText("Save RIR");
	buttonSaveRIR.setEnabled(true);

	addAndMakeVisible(&buttonClearSourceImage);
	buttonClearSourceImage.addListener(this);
	buttonClearSourceImage.setButtonText("Clear source");
	buttonClearSourceImage.setEnabled(true);

	addAndMakeVisible(&buttonReverbTail);
	buttonReverbTail.addListener(this);
	buttonReverbTail.setButtonText("Enable reverb tail");
	buttonReverbTail.setEnabled(true);
	buttonReverbTail.setToggleState(true, dontSendNotification);
	
	addAndMakeVisible(&buttonDirectToBinaural);
	buttonDirectToBinaural.addListener(this);
	buttonDirectToBinaural.setButtonText("Direct to binaural");
	buttonDirectToBinaural.setEnabled(true);
	buttonDirectToBinaural.setToggleState(false, dontSendNotification);

	addAndMakeVisible(&comboNumFrequencyBands);
	comboNumFrequencyBands.addListener(this);
	comboNumFrequencyBands.setEditableText(false);
	comboNumFrequencyBands.setJustificationType(Justification::right);
	comboNumFrequencyBands.addItemList({"3", "10"}, 1);
	comboNumFrequencyBands.setSelectedId(1);

	addAndMakeVisible(&comboSourceDirectivity);
	comboSourceDirectivity.addListener(this);
	comboSourceDirectivity.setEditableText(false);
	comboSourceDirectivity.setJustificationType(Justification::right);
	comboSourceDirectivity.addItemList({ "omni", "directional" }, 1);
	comboSourceDirectivity.setSelectedId(1);

	addAndMakeVisible(&sliderDirectPathGain);
	sliderDirectPathGain.addListener(this);
	sliderDirectPathGain.setRange(0.0, 2.0);
	sliderDirectPathGain.setValue(1.0);
	sliderDirectPathGain.setSliderStyle(Slider::LinearHorizontal);
	sliderDirectPathGain.setTextBoxStyle(Slider::TextBoxRight, true, 70, 20);
	
	addAndMakeVisible(&sliderEarlyReflectionsGain);
	sliderEarlyReflectionsGain.addListener(this);
	sliderEarlyReflectionsGain.setRange(0.0, 2.0);
	sliderEarlyReflectionsGain.setValue(1.0);
	sliderEarlyReflectionsGain.setSliderStyle(Slider::LinearHorizontal);
	sliderEarlyReflectionsGain.setTextBoxStyle(Slider::TextBoxRight, true, 70, 20);
	
	addAndMakeVisible(&sliderReverbTailGain);
	sliderReverbTailGain.addListener(this);
	sliderReverbTailGain.setRange(0.0, 2.0);
	sliderReverbTailGain.setValue(1.0);
	sliderReverbTailGain.setSliderStyle(Slider::LinearHorizontal);
	sliderReverbTailGain.setTextBoxStyle(Slider::TextBoxRight, true, 70, 20);

	addAndMakeVisible(&sliderCrossfadeFactor);
	sliderCrossfadeFactor.addListener(this);
	sliderCrossfadeFactor.setRange(0.01, 0.2);
	sliderCrossfadeFactor.setValue(0.1);
	sliderCrossfadeFactor.setSliderStyle(Slider::LinearHorizontal);
	sliderCrossfadeFactor.setTextBoxStyle(Slider::TextBoxRight, true, 70, 20);
	sliderCrossfadeFactor.setSkewFactor(0.7);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AuralisationComponent::buttonClicked(Button* button)
{
	auto parent = dynamic_cast<MainComponent*>(getParentComponent());

	if (button == &buttonSaveRIR)
	{
		parent->saveRIR();
	}
	else if (button == &buttonClearSourceImage)
	{
		parent->clearSourceImage();

	}
	else if (button == &buttonReverbTail)
	{
		bool enable = button->getToggleState();
		sliderReverbTailGain.setEnabled(enable);
		parent->enableReverbTail(enable);
	}
	else if (button == &buttonDirectToBinaural)
	{
		bool enable =button->getToggleState();
		parent->enableDirectToBinaural(enable);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AuralisationComponent::comboBoxChanged(ComboBox* comboBox)
{
	auto parent = dynamic_cast<MainComponent*>(getParentComponent());

	if (comboBox == &comboNumFrequencyBands)
	{
		int value = comboBox->getText().getIntValue();
		parent->updateNumFrequencyBands(value);
	}
	else if (comboBox == &comboSourceDirectivity)
	{
		String value = comboBox->getText();
		parent->updateSourceDirectivity(value);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AuralisationComponent::sliderValueChanged(Slider* slider)
{
	auto parent = dynamic_cast<MainComponent*>(getParentComponent());

	if (slider == &sliderDirectPathGain)
	{
		parent->updateDirectPathGain(slider->getValue());
	}
	else if (slider == &sliderEarlyReflectionsGain)
	{
		parent->updateEarlyReflectionsGain(slider->getValue());
	}
	else if (slider == &sliderReverbTailGain)
	{
		parent->updateReverbTailGain(slider->getValue());
	}
	else if(slider == &sliderCrossfadeFactor)
	{
		parent->updateCrossfadeFactor(slider->getValue());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AuralisationComponent::paint(Graphics& g)
{
	g.setColour(Colours::white);
	g.drawRoundedRectangle(10, 10, getWidth() - 20, getHeight() - 20,
		getParentComponent()->getHeight() / 100, 2.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AuralisationComponent::resized()
{
	int h = (getHeight() - 40) / 5;
	int w = (getWidth() - 40) / 20;

	Font labelFont = labelAuralisation.getFont();
	labelAuralisation.setBounds(30, 5, 1.2 * labelFont.getStringWidth(labelAuralisation.getText()), labelFont.getHeight());
	
	labelDirectPathGain.setBounds(20, 20, 4 * w, h);
	labelEarlyReflectionsGain.setBounds(20, 20 + h, 4 * w, h);
	labelReverbTail.setBounds(20, 20 + 2 * h, 4 * w, h);
	labelCrossfadeFactor.setBounds(20, 20 + 3 * h, 4 * w, h);
	buttonReverbTail.setBounds(20, 20 + 4 * h, 4 * w, h);
	sliderDirectPathGain.setBounds(20 + 4 * w, 20, 16 * w, h);
	sliderEarlyReflectionsGain.setBounds(20 + 4 * w, 20 + h, 16 * w, h);
	sliderReverbTailGain.setBounds(20 + 4 * w, 20 + 2 * h, 16 * w, h);
	sliderCrossfadeFactor.setBounds(20 + 4 * w, 20 + 3 * h, 16 * w, h);
	buttonDirectToBinaural.setBounds(20 + 4 * w, 20 + 4 * h, 4 * w, h);
	labelNumFrequencyBands.setBounds(20 + 8 * w, 20 + 4 * h, 4 * w, h / 2);
	labelSourceDirectivity.setBounds(20 + 8 * w, 20 + 4.5 * h, 4 * w, h / 2);
	comboNumFrequencyBands.setBounds(20 + 12 * w, 20 + 4 * h, 2 * w, h / 2);
	comboSourceDirectivity.setBounds(20 + 12 * w, 20 + 4.5 * h, 2 * w, h / 2);
	buttonSaveRIR.setBounds(pad(20 + 14 * w, 20 + 4 * h, 3 * w, h));
	buttonClearSourceImage.setBounds(pad(20 + 17 * w, 20 + 4 * h, 3 * w, h));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////