#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include <vector>
#include <array>
#include <unordered_map>

#include "../JuceLibraryCode/JuceHeader.h"
#include "OSCHandler.h"
#include "AudioIOComponent.h"
#include "AudioRecorder.h"
#include "Ambi2binIRContainer.h"
#include "FIRFilter/FIRFilter.h"
#include "Utils.h"
#include "DelayLine.h"
#include "SourceImagesHandler.h"
#include "AuralisationComponent.h"
#include "LoggingComponent.h"

#include "CustomLookAndFeel.h"

#include "../ff_meters/ff_meters.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class MainComponent :
	public AudioAppComponent,
	public ChangeListener
{
	public:

		MainComponent();
		~MainComponent();
    
		// AudioAppComponent overrides.
		void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
		void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
		void releaseResources() override;
    
		void processAmbisonicBuffer( AudioBuffer<float> *const audioBufferToFill );
		void fillNextAudioBlock( AudioBuffer<float> *const audioBufferToFill );
		void recordAmbisonicBuffer();
		void recordIr();
    
		void paint (Graphics& g) override;
		void resized() override;
    
		// Output clipping
		bool isClipped = false;
    
		void enableReverbTail(bool enable);
		void enableDirectToBinaural(bool enable);
		void saveRIR();
		void clearSourceImage();
		void updateNumFrequencyBands(int value);
		void updateSourceDirectivity(String value);
		void updateDirectPathGain(double value);
		void updateEarlyReflectionsGain(double value);
		void updateReverbTailGain(double value);
		void updateCrossfadeFactor(double value);
		String getLogs(bool enable);
		void enableRecordAmbisonicToDisk(bool enable);
		void saveOscState();

		AudioDeviceSelectorComponent audioSetupComponent;

private:
		
    void changeListenerCallback (ChangeBroadcaster* source) override;
    void updateOnOscReceive();
    float clipOutput(float input);
    
    // Miscellaneaous.
    double localSampleRate;
    int localSamplesPerBlockExpected;
    OSCHandler oscHandler; // receive OSC messages, ready them for other components
    bool isRecordingIr = false;
    
    // GUI elements.
    Image logoImage;
		AuralisationComponent auralisationComponent;
		LoggingComponent loggingComponent;
		CustomLookAndFeel customLookAndFeel;

		ff::LevelMeter levelMeterComponent;
		ff::LevelMeterSource levelMeterSource;
		ff::LevelMeterLookAndFeel levelMeterLnf;

    std::unordered_map<Button*, std::string> buttonMap;
    std::unordered_map<ComboBox*, Array< std::string > > comboBoxMap;
    std::unordered_map<Slider*, Array< double > > sliderMap;
    std::unordered_map<Label*, std::string> labelMap;
    std::unordered_map<ToggleButton*, std::string> toggleMap;

    // Audio components.

    // Buffers
    AudioBuffer<float> workingBuffer; // working buffer
    AudioBuffer<float> recordingBufferOutput; // recording buffer
    AudioBuffer<float> recordingBufferAmbisonicOutput; // recording buffer
    AudioBuffer<float> recordingBufferInput; // recording buffer
    
    // Audio player (GUI + audio reader + adc input)
    AudioIOComponent audioIOComponent;
    
    // Audio stream recorder
    AudioRecorder audioRecorder;
    
    // Delay line
    DelayLine delayLine;
    bool requireDelayLineSizeUpdate = false;
   
    // Sources images
    SourceImagesHandler sourceImagesHandler;
    bool sourceImageHandlerNeedsUpdate = false;
    
    // Ambisonic to binaural decoding
    AudioBuffer<float> ambisonicBuffer;
    AudioBuffer<float> ambisonicRecordBuffer;
    AudioBuffer<float> ambisonicBuffer2ndEar;
    Ambi2binIRContainer ambi2binContainer;
    FIRFilter ambi2binFilters[2*N_AMBI_CH]; // holds current ABIR (room reverb) filters
    
    // Frequency band
    int numFreqBands = 0;
    bool updateNumFreqBandrequired = false;
   
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif  // MAINCOMPONENT_H_INCLUDED