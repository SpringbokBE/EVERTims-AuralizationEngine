#include "AudioRecorder.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

AudioRecorder::AudioRecorder()
{
	backgroundThread.startThread();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

AudioRecorder::~AudioRecorder()
{
	stopRecording();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioRecorder::startRecording()
{
	auto parentDir = File::getSpecialLocation(File::userDocumentsDirectory);
	File file = parentDir.getNonexistentChildFile("JUCE Demo Audio Recording", ".wav");
	stopRecording();

	if (localSampleRate > 0)
	{
		// Create an OutputStream to write to our destination file...
		file.deleteFile();
		std::unique_ptr<FileOutputStream> fileStream(file.createOutputStream());

		if (fileStream.get() != nullptr)
		{
			// Now create a WAV writer object that writes to our output stream...
			WavAudioFormat wavFormat;
			auto* writer = wavFormat.createWriterFor(fileStream.get(), localSampleRate, localNumChannel, 16, {}, 0);

			if (writer != nullptr)
			{
				fileStream.release(); // (passes responsibility for deleting the stream to the writer object that is now using it)

				// Now we'll create one of these helper objects which will act as a FIFO buffer, and will
				// write the data to disk on our background thread.
				threadedWriter.reset(new AudioFormatWriter::ThreadedWriter(writer, backgroundThread, 32768));

				// And now, swap over our active writer pointer so that the audio callback will start using it..
				const ScopedLock sl(writerLock);
				activeWriter = threadedWriter.get();
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioRecorder::stopRecording()
{
	// First, clear this pointer to stop the audio callback from using our writer object..
	{
		const ScopedLock sl(writerLock);
		activeWriter = nullptr;
	}

	// Now we can delete the writer object. It's done in this order because the deletion could
	// take a little time while remaining data gets flushed to disk, so it's best to avoid blocking
	// the audio callback while this happens.
	threadedWriter.reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

bool AudioRecorder::isRecording() const
{
	return activeWriter.load() != nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioRecorder::recordBuffer(const float** inputChannelData, int numInputChannels, int numSamples)
{
	const ScopedLock sl(writerLock);

	if (activeWriter.load() != nullptr && numInputChannels >= localNumChannel)
	{
		activeWriter.load()->write(inputChannelData, numSamples);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void AudioRecorder::prepareToPlay(const unsigned int samplesPerBlockExpected, const double sampleRate) {

	localSampleRate = sampleRate;

	delayLine.prepareToPlay(samplesPerBlockExpected, sampleRate);
	delayLine.setSize(localNumChannel, 10 * sampleRate);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////