#include "AudioIOComponent.h"
#include "MainComponent.h"
#include "Utils.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

AudioIOComponent::AudioIOComponent()
{
	// Initialise audio reader elements.
	formatManager.registerBasicFormats();
	transportSource.addChangeListener(this);

	// Initialise GUI elements.
	addAndMakeVisible(&labelInput);
	labelInput.setText("Input", dontSendNotification);

	addAndMakeVisible(&buttonOpenAudio);
	buttonOpenAudio.addListener(this);
	buttonOpenAudio.setButtonText("OPEN");
	buttonOpenAudio.setColour(TextButton::buttonColourId, Colours::darkgrey);

	addAndMakeVisible(&buttonPlayAudio);
	buttonPlayAudio.addListener(this);
	buttonPlayAudio.setButtonText("PLAY");
	buttonPlayAudio.setColour(TextButton::buttonColourId, Colours::seagreen);
	buttonPlayAudio.setEnabled(false);

	addAndMakeVisible(&buttonStopAudio);
	buttonStopAudio.addListener(this);
	buttonStopAudio.setButtonText("STOP");
	buttonStopAudio.setColour(TextButton::buttonColourId, Colours::indianred);
	buttonStopAudio.setEnabled(false);

	addAndMakeVisible(&buttonSetupAudio);
	buttonSetupAudio.addListener(this);
	buttonSetupAudio.setButtonText("SETUP");

	addAndMakeVisible(&sliderAudioGain);
	sliderAudioGain.setRange(0.0, 4.0);
	sliderAudioGain.setValue(1.0);
	sliderAudioGain.setSliderStyle(Slider::LinearHorizontal);
	sliderAudioGain.setTextBoxStyle(Slider::TextBoxRight, true, 70, 20);

	addAndMakeVisible(&sliderMicGain);
	sliderMicGain.setRange(0.0, 2.0);
	sliderMicGain.setValue(1.0);
	sliderMicGain.setSliderStyle(Slider::LinearHorizontal);
	sliderMicGain.setTextBoxStyle(Slider::TextBoxRight, true, 70, 20);

	addAndMakeVisible(&buttonLoopAudio);
	buttonLoopAudio.addListener(this);
	buttonLoopAudio.setButtonText("Loop audio");
	buttonLoopAudio.setEnabled(true); // Is this required?

	addAndMakeVisible(&buttonMicInput);
	buttonMicInput.addListener(this);
	buttonMicInput.setButtonText("Mic input");
	buttonMicInput.setEnabled(true); // Is this required?

	// Open audio file "impulse" at startup
	File audioFile = getFileFromString("sounds/impulse.wav");
	bool fileOpenedSucess = loadAudioFile(audioFile);
	buttonPlayAudio.setEnabled(fileOpenedSucess);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool AudioIOComponent::loadAudioFile(const File& file)
// load audio file to transport source
{
	bool fileOpenedSucess = false;

	AudioFormatReader* reader = formatManager.createReaderFor(file);

	if (reader != nullptr)
	{
		ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource(reader, true);
		transportSource.setSource(newSource, 0, nullptr, reader->sampleRate);
		readerSource = newSource.release();
		fileOpenedSucess = true;
	}

	return fileOpenedSucess;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool AudioIOComponent::openAudioFile()
// open audio file from GUI
{
	bool fileOpenedSucess = false;

	FileChooser chooser("Select a Wave file to play...", File(), "*.wav");

	if (chooser.browseForFileToOpen())
	{
		File file(chooser.getResult());
		fileOpenedSucess = loadAudioFile(file);
	}

	return fileOpenedSucess;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioIOComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
// local equivalent of prepareToPlay
{
	transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
	adcBuffer.setSize(1, samplesPerBlockExpected);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioIOComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
// fill in bufferToFill with data from audio file or adc
{
	// clear buffer
	bufferToFill.clearActiveBufferRegion();

	// check if audiofile loaded
	if (readerSource != nullptr)
	{
		// fill buffer with audiofile data
		transportSource.getNextAudioBlock(bufferToFill);

		// stereo downmix to mono
		bufferToFill.buffer->applyGain(0.5f);
		bufferToFill.buffer->addFrom(0, 0, bufferToFill.buffer->getWritePointer(1), bufferToFill.buffer->getNumSamples());

		// apply gain
		bufferToFill.buffer->applyGain(sliderAudioGain.getValue());
	}

	// copy adc inputs (stored in adcBuffer) to output
	if (buttonMicInput.getToggleState())
	{
		// apply gain
		adcBuffer.applyGain(sliderMicGain.getValue());
		// add to output
		bufferToFill.buffer->addFrom(0, 0, adcBuffer, 0, 0, bufferToFill.buffer->getNumSamples());
	}

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioIOComponent::saveIR(const AudioBuffer<float>& source, double sampleRate, String fileName)
{
	const File file(File::getSpecialLocation(File::userDesktopDirectory)
		.getNonexistentChildFile(fileName, ".wav"));

	// Create an OutputStream to write to our destination file...
	file.deleteFile();
	ScopedPointer<FileOutputStream> fileStream(file.createOutputStream());

	if (fileStream != nullptr)
	{
		// Now create a WAV writer object that writes to our output stream...
		WavAudioFormat wavFormat;
		AudioFormatWriter* writer = wavFormat.createWriterFor(fileStream, sampleRate, source.getNumChannels(), 24, StringPairArray(), 0);

		if (writer != nullptr)
		{
			fileStream.release(); // (passes responsibility for deleting the stream to the writer object that is now using it)

			writer->writeFromAudioSampleBuffer(source, 0, source.getNumSamples());
			delete(writer);

		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioIOComponent::audioDeviceAboutToStart(AudioIODevice*)
{
	adcBuffer.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioIOComponent::audioDeviceStopped()
{
	adcBuffer.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioIOComponent::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels,
	float** outputChannelData, int numOutputChannels,
	int numberOfSamples)
{
	adcBuffer.setDataToReferTo(const_cast<float**> (inputChannelData), numInputChannels, numberOfSamples);

	/**
	 1) We need to clear the output buffers, in case they're full of junk..
	 2) I actually do not intend to output anything from that method, since
	 outputChannelData will be summed with MainComponent IO device manager output
	 anyway
	 */
	for (int i = 0; i < numOutputChannels; ++i)
	{
		if (outputChannelData[i] != nullptr)
		{
			FloatVectorOperations::clear(outputChannelData[i], numberOfSamples);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioIOComponent::buttonClicked(Button* button)
{
	if (button == &buttonOpenAudio)
	{
		bool fileOpenedSucess = openAudioFile();
		buttonPlayAudio.setEnabled(fileOpenedSucess);
	}

	if (button == &buttonPlayAudio)
	{
		if (readerSource != nullptr)
		{
			readerSource->setLooping(buttonLoopAudio.getToggleState());
		}
		changeState(Starting);
	}

	if (button == &buttonStopAudio)
	{
		changeState(Stopping);
	}

	if (button == &buttonLoopAudio)
	{
		if (readerSource != nullptr)
		{
			readerSource->setLooping(buttonLoopAudio.getToggleState());
		}
	}

	else if (button == &buttonSetupAudio)
	{
		auto parent = dynamic_cast<MainComponent*>(getParentComponent());

		parent->audioSetupComponent.setSize(400, 500);
		parent->audioSetupComponent.setCentrePosition(200, 250);
		DialogWindow::showDialog("Audio setup", &parent->audioSetupComponent, this, CustomLookAndFeel::backgroundColour, true);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioIOComponent::changeState(TransportState newState)
{
	if (audioPlayerState != newState)
	{
		audioPlayerState = newState;

		switch (audioPlayerState)
		{
		case Stopped:
			buttonStopAudio.setEnabled(false);
			buttonPlayAudio.setEnabled(true);
			transportSource.setPosition(0.0);
			break;

		case Loaded:
			break;

		case Starting:
			buttonPlayAudio.setEnabled(false);
			transportSource.start();
			break;

		case Playing:
			buttonStopAudio.setEnabled(true);
			break;

		case Stopping:
			transportSource.stop();
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioIOComponent::changeListenerCallback(ChangeBroadcaster* broadcaster)
{
	if (broadcaster == &transportSource)
	{
		if (broadcaster == &transportSource)
		{
			if (transportSource.isPlaying())
				changeState(Playing);
			else
				changeState(Stopped);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioIOComponent::paint(Graphics& g)
{
	g.setColour(Colours::white);
	g.drawRoundedRectangle(10, 10, getWidth() - 20, getHeight() - 20,
		getParentComponent()->getHeight() / 100, 2.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioIOComponent::resized()
{
	int h = (getHeight() - 40) / 3;
	int w = (getWidth() - 40) / 20;

	Font labelFont = labelInput.getFont();
	labelInput.setBounds(30, 5, 1.2 * labelFont.getStringWidth(labelInput.getText()), labelFont.getHeight());

	buttonOpenAudio.setBounds(pad(20, 20, 5 * w, h, 2));
	buttonPlayAudio.setBounds(pad(20 + 5 * w, 20, 5 * w, h, 2));
	buttonStopAudio.setBounds(pad(20 + 10 * w, 20, 5 * w, h, 2));
	buttonSetupAudio.setBounds(pad(20 + 15 * w, 20, 5 * w, h, 2));
	buttonLoopAudio.setBounds(20, 30 + h, 4 * w, h);
	buttonMicInput.setBounds(20, 30 + 2 * h, 4 * w, h);
	sliderAudioGain.setBounds(20 + 4 * w, 30 + h, 16 * w, h);
	sliderMicGain.setBounds(20 + 4 * w, 30 + 2 * h, 16 * w, h);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////