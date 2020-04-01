#ifndef AUDIOIOCOMPONENT_H_INCLUDED
#define AUDIOIOCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class AudioIOComponent :
	public Component,
	public Button::Listener,
	public ChangeListener,
	public AudioIODeviceCallback
{
	public:

		AudioIOComponent();
		~AudioIOComponent(){};

		bool loadAudioFile(const File& file);
		bool openAudioFile();
		void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
		void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);
		void saveIR(const AudioBuffer<float>& source, double sampleRate, String fileName);

		AudioTransportSource transportSource;
    
	private:

		enum TransportState { Stopped, Loaded, Starting, Playing, Stopping };

		void audioDeviceAboutToStart(AudioIODevice*) override;
		void audioDeviceStopped() override;
		void audioDeviceIOCallback(const float ** inputChannelData, int numInputChannels,
																		 float ** outputChannelData, int numOutputChannels, int numberOfSamples) override;

		void paint(Graphics& g) override;
		void resized() override;

		void buttonClicked(Button* button) override;
		void changeState(TransportState newState);
		void changeListenerCallback(ChangeBroadcaster* broadcaster) override;

		// GUI elements.
		Label labelInput;

    TextButton buttonOpenAudio,
			buttonPlayAudio,
			buttonStopAudio,
			buttonSetupAudio;

    ToggleButton buttonLoopAudio,
			buttonMicInput;
    
		Slider sliderAudioGain,
			sliderMicGain;

    bool shouldLoopAudioFile = false;
    AudioFormatManager formatManager;
    ScopedPointer<AudioFormatReaderSource> readerSource;
    TransportState audioPlayerState;

    AudioBuffer<float> adcBuffer;
        

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioIOComponent)

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // AUDIOIOCOMPONENT_H_INCLUDED