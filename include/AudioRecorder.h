#include "../JuceLibraryCode/JuceHeader.h"
#include "DelayLine.h"
#include "Utils.h"

class AudioRecorder
{
	public:
    
		AudioRecorder();
		~AudioRecorder();

		void startRecording();
		void stopRecording();
		bool isRecording() const;
		void recordBuffer(const float** inputChannelData, int numInputChannels, int numSamples);
		void prepareToPlay(const unsigned int samplesPerBlockExpected, const double sampleRate);
    
private:

    // Ring buffer used to write audio data in main audio loop, awaiting writing to disk.
    DelayLine delayLine;
    
    // The thread that will write our audio data to disk.
    TimeSliceThread backgroundThread { "Audio Recorder Thread" };
    
    // The FIFO used to buffer the incoming data.
    std::unique_ptr<AudioFormatWriter::ThreadedWriter> threadedWriter;
    
    CriticalSection writerLock;
    std::atomic<AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };
    
    double localSampleRate = 0;
    unsigned int localNumChannel = N_AMBI_CH;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioRecorder)
};
