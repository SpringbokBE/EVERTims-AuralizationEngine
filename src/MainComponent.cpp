
#include "MainComponent.h"
#include "Utils.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

MainComponent::MainComponent() :
	oscHandler(),
	audioIOComponent(),
	auralisationComponent(),
	loggingComponent(),
	levelMeterComponent(ff::LevelMeter::Compact),
	audioRecorder(),
	delayLine(),
	sourceImagesHandler(),
	ambi2binContainer(),
	audioSetupComponent(deviceManager, 0, 256, 0, 256, false, false, false, false)
{
		setLookAndFeel(&customLookAndFeel);

		levelMeterComponent.setMeterSource(&levelMeterSource);
		addAndMakeVisible(&levelMeterComponent);

		// Set window dimensions.
		setSize(650, 700);

    // Specify the required number of input and output channels.
    setAudioChannels (0, N_AMBI_CH);
    
    // Add to change listeners.
    oscHandler.addChangeListener(this);
   
    // Add audioIOComponent as addAudioCallback for adc input.
    deviceManager.addAudioCallback(&audioIOComponent);
    
    // Initialise GUI elements.
    
    // Add GUI sub-components.
		addAndMakeVisible(audioSetupComponent);
    addAndMakeVisible(audioIOComponent);
		addAndMakeVisible(auralisationComponent);
		addAndMakeVisible(loggingComponent);
    
    // Setup logo image.
    logoImage = ImageCache::getFromMemory(BinaryData::evertims_logo_512_png, BinaryData::evertims_logo_512_pngSize);
    logoImage = logoImage.rescaled(logoImage.getWidth()/2, logoImage.getHeight()/2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

MainComponent::~MainComponent()
{
    // Fix denied access at close when sound playing,
    // see https://forum.juce.com/t/tutorial-playing-sound-files-raises-an-exception-on-2nd-load/15738/2
    audioIOComponent.transportSource.setSource(nullptr);
		setLookAndFeel(nullptr);

		levelMeterComponent.setLookAndFeel(nullptr);

    shutdownAudio();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
// This function will be called when the audio device is started, or when
// its settings (i.e. sample rate, block size, etc) are changed.
// Called on the audio thread, not the GUI thread.
{
    
    // Audio file reader & adc input
    audioIOComponent.prepareToPlay (samplesPerBlockExpected, sampleRate);
    
    // Recorder
    audioRecorder.prepareToPlay (samplesPerBlockExpected, sampleRate);
    
    // Working buffer
    workingBuffer.setSize(1, samplesPerBlockExpected);
    // Ambisonic buffer holds 2 stereo channels (first) + ambisonic channels
    ambisonicBuffer.setSize(2 + N_AMBI_CH, samplesPerBlockExpected);
    // Because of stupid design choice of ambisonicBuffer, require this additional ambisonicRecordBuffer. to clean..
    ambisonicRecordBuffer.setSize(N_AMBI_CH, samplesPerBlockExpected);
    
    // Keep track of sample rate
    localSampleRate = sampleRate;
    localSamplesPerBlockExpected = samplesPerBlockExpected;
    
    // Initialise delay line.
    delayLine.prepareToPlay(samplesPerBlockExpected, sampleRate);
    delayLine.setSize(1, sampleRate); // arbitrary length of 1 sec
    sourceImagesHandler.prepareToPlay (samplesPerBlockExpected, sampleRate);
    
    // Initialise ambi 2 bin decoding: fill in data in ABIR filtered and ABIR filter themselves
    for( int i = 0; i < N_AMBI_CH; i++ )
    {
        ambi2binFilters[ 2*i ].init(samplesPerBlockExpected, AMBI2BIN_IR_LENGTH);
        ambi2binFilters[ 2*i ].setImpulseResponse(ambi2binContainer.ambi2binIrDict[i][0].data()); // [ch x ear x sampID]
        ambi2binFilters[2*i+1].init(samplesPerBlockExpected, AMBI2BIN_IR_LENGTH);
        ambi2binFilters[2*i+1].setImpulseResponse(ambi2binContainer.ambi2binIrDict[i][1].data()); // [ch x ear x sampID]
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
// Audio Processing (split in "processAmbisonicBuffer" and "fillNextAudioBlock" to enable
// IR recording: using the same methods as the main thread)
{
  // Check if update required
  if( updateNumFreqBandrequired ){
      sourceImagesHandler.setFilterBankSize(numFreqBands);
      updateNumFreqBandrequired = false;
      // Trigger general update: must re-dimension abs.coeffs and trigger update future->current, see in function
      sourceImagesHandler.updateFromOscHandler(oscHandler);
  }
    
  // Fill buffer with audiofile data
  audioIOComponent.getNextAudioBlock(bufferToFill);
    
  // Execute main audio processing
  if( !isRecordingIr )
  {
      processAmbisonicBuffer( bufferToFill.buffer );
      if( audioRecorder.isRecording() ){ recordAmbisonicBuffer(); }
      fillNextAudioBlock( bufferToFill.buffer );
  }
  // Simply clear output buffer
  else
  {
      bufferToFill.clearActiveBufferRegion();
  }
   
	levelMeterSource.measureBlock(*bufferToFill.buffer);

  // Check if source images need update (i.e. update called by OSC handler
  // while source images in the midst of a crossfade
  if( sourceImageHandlerNeedsUpdate && sourceImagesHandler.crossfadeOver )
  {
      sourceImagesHandler.updateFromOscHandler(oscHandler);
      requireDelayLineSizeUpdate = true;
      sourceImageHandlerNeedsUpdate = false;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::processAmbisonicBuffer( AudioBuffer<float> *const audioBufferToFill )
// Audio Processing: split from getNextAudioBlock to use it for recording IR
{
    workingBuffer.copyFrom(0, 0, audioBufferToFill->getWritePointer(0), workingBuffer.getNumSamples());
    
    //==========================================================================
    // SOURCE IMAGE PROCESSING
    
    if ( sourceImagesHandler.numSourceImages > 0 )
    {
        
        //==========================================================================
        // DELAY LINE
        
        // update delay line size if need be (TODO: MOVE THIS .SIZE() OUTSIDE OF AUDIO PROCESSING LOOP
        if ( requireDelayLineSizeUpdate )
        {
            // get maximum required delay line duration
            float maxDelay = sourceImagesHandler.getMaxDelayFuture();
            
            // get associated required delay line buffer length
            int updatedDelayLineLength = (int)( 1.5 * maxDelay * localSampleRate); // longest delay creates noisy sound if delay line is exactly 1* its duration
            
            // update delay line size
            delayLine.setSize(1, updatedDelayLineLength);
            
            // unflag update required
            requireDelayLineSizeUpdate = false;
        }
        
        // add current audio buffer to delay line
        delayLine.copyFrom(0, workingBuffer, 0, 0, workingBuffer.getNumSamples());
        
        // loop over sources images, apply delay + room coloration + spatialization
        sourceImagesHandler.getNextAudioBlock( & delayLine, ambisonicBuffer );
        
        // increment delay line write position
        delayLine.incrementWritePosition(workingBuffer.getNumSamples());
        
    }
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::fillNextAudioBlock( AudioBuffer<float> *const audioBufferToFill )
{
    
    //==========================================================================
    // SPATIALISATION: Ambisonic decoding + virtual speaker approach + binaural
    
    if ( sourceImagesHandler.numSourceImages > 0 )
    {
        // duplicate channel before filtering for two ears
        ambisonicBuffer2ndEar = ambisonicBuffer;

        // loop over Ambisonic channels
        for (int k = 0; k < N_AMBI_CH; k++)
        {
            //ambi2binFilters[ 2*k ].process(ambisonicBuffer.getWritePointer(k+2)); // left
            //ambi2binFilters[2*k+1].process(ambisonicBuffer2ndEar.getWritePointer(k+2)); // right
            
            // collapse left channel, collapse right channel
            //ambisonicBuffer.addFrom(0, 0, ambisonicBuffer.getWritePointer(2+k), workingBuffer.getNumSamples());
            //ambisonicBuffer2ndEar.addFrom(1, 0, ambisonicBuffer2ndEar.getWritePointer(2+k), workingBuffer.getNumSamples());
					audioBufferToFill->copyFrom(k, 0, ambisonicBuffer, k + 2, 0, workingBuffer.getNumSamples());
				}

        // final rewrite to output buffer
        //audioBufferToFill->copyFrom(0, 0, ambisonicBuffer, 0, 0, workingBuffer.getNumSamples());
        //audioBufferToFill->copyFrom(1, 0, ambisonicBuffer2ndEar, 1, 0, workingBuffer.getNumSamples());
    }
    
    //==========================================================================
    // if no source image, simply rewrite to output buffer (TODO: remove stupid double copy)
    else
    {
        audioBufferToFill->copyFrom(0, 0, workingBuffer, 0, 0, workingBuffer.getNumSamples());
        audioBufferToFill->copyFrom(1, 0, workingBuffer, 0, 0, workingBuffer.getNumSamples());
    }
    
    //==========================================================================
    // CLIP OUTPUT (DEBUG PRECAUTION)
    auto outL = audioBufferToFill->getWritePointer(0);
    auto outR = audioBufferToFill->getWritePointer(1);
    for (int i = 0; i < workingBuffer.getNumSamples(); i++)
    {
        outL[i] = clipOutput(outL[i]);
        outR[i] = clipOutput(outR[i]);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::recordAmbisonicBuffer()
// Record Ambisonic buffer to disk
{
    if ( sourceImagesHandler.numSourceImages > 0 )
    {
        // loop over Ambisonic channels to extract only ambisonic channels. I know, stupid. Needs cleaning
        for (int k = 0; k < N_AMBI_CH; k++)
        {
            ambisonicRecordBuffer.copyFrom(k, 0, ambisonicBuffer, k+2, 0, ambisonicBuffer.getNumSamples());
        }
    }
    // if no source image, data in ambisonicBuffer is meaningless: copy content of workingbuffer (raw input) rather
    else{
        ambisonicRecordBuffer.clear();
        ambisonicRecordBuffer.copyFrom(0, 0, workingBuffer, 0, 0, ambisonicBuffer.getNumSamples());
    }
    
    // write to disk
    audioRecorder.recordBuffer((const float **) ambisonicRecordBuffer.getArrayOfWritePointers(), ambisonicRecordBuffer.getNumChannels(), ambisonicRecordBuffer.getNumSamples());
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::recordIr()
// Record current Room Impulse Response to disk
{
    // estimate output buffer size (based on max delay time)
    auto maxDelaySourceImages = getMaxValue( oscHandler.getSourceImageDelays() );
    auto rt60 = oscHandler.getRT60Values();
    float maxDelayRt60 = getMaxValue(rt60);
    float maxDelay = fmax( maxDelaySourceImages, maxDelayRt60 );
    int maxDelayInSamp = ceil(maxDelay * localSampleRate);
    // output buffer at least 1 localSamplesPerBlockExpected long
    maxDelayInSamp = fmax( maxDelayInSamp, localSamplesPerBlockExpected);
    
    // get min delay
    int minDelayInSamp = ceil( localSampleRate * getMinValue( oscHandler.getSourceImageDelays() ) );
    
    // init
    recordingBufferInput.setSize(2, localSamplesPerBlockExpected);
    recordingBufferInput.clear();
    recordingBufferOutput.setSize(2, 2*maxDelayInSamp);
    recordingBufferOutput.clear();
    recordingBufferAmbisonicOutput.setSize(N_AMBI_CH, 2*maxDelayInSamp);
    recordingBufferAmbisonicOutput.clear();
    
    // prepare impulse response buffer
    recordingBufferInput.getWritePointer(0)[0] = 1.0f;
    
    // clear delay lines / fdn buffers of main thread
    delayLine.clear();
    sourceImagesHandler.reverbTail.clear();
    
    // pass impulse input into processing loop until IR faded below threshold
    float rms = 1.0f;
    int bufferId = 0;
    // record until rms below threshold or reached max delay. minDelay used here to make sure recording doesn't stop on first buffers for large
    // source-listener distances, where RMS is zero for the first few buffers until LOS image source reaches listener.
    while( ( rms >= 0.00001f || bufferId*localSamplesPerBlockExpected < minDelayInSamp ) && bufferId*localSamplesPerBlockExpected < maxDelayInSamp )
    {
        // clear impulse after first round
        if( bufferId >= 1 ){ recordingBufferInput.clear(); }
        
        // execute main audio processing: fill ambisonic buffer
        processAmbisonicBuffer( &recordingBufferInput );
        
        // add to output ambisonic buffer
        for( int k = 0; k < N_AMBI_CH; k++ )
        {
            recordingBufferAmbisonicOutput.addFrom(k, bufferId*localSamplesPerBlockExpected, ambisonicBuffer, k+2, 0, localSamplesPerBlockExpected);
        }
        
        // ambisonic to stereo
        fillNextAudioBlock( &recordingBufferInput );
        
        // add to output stereo buffer
        for( int k = 0; k < 2; k++ )
        {
            recordingBufferOutput.addFrom(k, bufferId*localSamplesPerBlockExpected, recordingBufferInput, k, 0, localSamplesPerBlockExpected);
        }
        
        // increment
        bufferId += 1;
        rms = recordingBufferInput.getRMSLevel(0, 0, localSamplesPerBlockExpected) + recordingBufferInput.getRMSLevel(0, 0, localSamplesPerBlockExpected);
        rms *= 0.5;
    }
    
    // resize output IR buffers to max meaningful sample length
    recordingBufferOutput.setSize(2, bufferId*localSamplesPerBlockExpected, true);
    recordingBufferAmbisonicOutput.setSize(N_AMBI_CH, bufferId*localSamplesPerBlockExpected, true);
    
    // save output
    audioIOComponent.saveIR(recordingBufferAmbisonicOutput, localSampleRate, String("Evertims_IR_Recording_ambi_") + String(AMBI_ORDER) + String("_order"));
    audioIOComponent.saveIR(recordingBufferOutput, localSampleRate, "Evertims_IR_Recording_binaural");
    
    // unlock main audio thread
    isRecordingIr = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::releaseResources()
// This will be called when the audio device stops, or when it is being
// restarted due to a setting change.
{
    
    audioIOComponent.transportSource.releaseResources();
    
    // clear all "delay line" like buffers
    delayLine.clear();
    sourceImagesHandler.reverbTail.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

float MainComponent::clipOutput(float input)
// CRUDE DEBUG PRECAUTION - clip output in [-1.0; 1.0]
{
    if (std::abs(input) > 1.0f)
    {
        loggingComponent.ledClipping.isClipped = true;
        return sign(input)*fmin(std::abs(input), 1.0f);
    }
		else
			return input;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::updateOnOscReceive()
// Method called when new OSC messages are available
{
    // update OSC handler internals (has to happend here, to ensure that they won't be updated while
    // running the sourceImagesHandler.updateFromOscHandler method that reads them OSC internals.
    oscHandler.updateInternals();
    
    // if sourceImagesHandler not in the midst of an update
    if( sourceImagesHandler.crossfadeOver && !sourceImageHandlerNeedsUpdate )
    {
        // update source images attributes based on latest received OSC info
        sourceImagesHandler.updateFromOscHandler(oscHandler);
        
        // now that everything is ready: set update flag, to resize delay line at next audio loop
        requireDelayLineSizeUpdate = true;
    }
    // otherwise, flag that an update is required
    else{ sourceImageHandlerNeedsUpdate = true; }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::paint (Graphics& g)
{
	g.fillAll(CustomLookAndFeel::backgroundColour);
    // background
    //g.fillAll(Colours::grey);
    // parameters box
    // g.setOpacity(1.0f);
    //g.setColour(Colours::white);
    //g.drawRect(10.f, 155.f, getWidth()-20.f, 150.f);
    //
    //// logo image
    //g.drawImageAt(logoImage, (int)( (getWidth()/2) - (logoImage.getWidth()/2) ), 380);
    //
    //// signature
    //g.setColour(Colours::white);
    //g.setFont(11.f);
    //g.drawFittedText("designed by D. Poirier-Quinot, M. Noisternig, and B. F.G. Katz (2017)", getWidth() - 335, getHeight()-15, 325, 15, Justification::right, 2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::resized()
{
	int h = getHeight() / 5;
	int w = getWidth() / 5;

	audioIOComponent.setBounds(0, 0, 3 * w, h);
	auralisationComponent.setBounds(0, h, 3 * w, 2 * h);
	loggingComponent.setBounds(0, 3 * h, 3 * w, 2 * h);
	levelMeterComponent.setBounds(3 * w, 0, 2 * w, 5 * h);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::changeListenerCallback (ChangeBroadcaster* broadcaster)
{
    if (broadcaster == &oscHandler)
    {
				loggingComponent.updateLoggingText(oscHandler.getMapContentForGUI());
        updateOnOscReceive();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

Component* createMainContentComponent()
{
	return new MainComponent();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::enableReverbTail(bool enable)
{
	sourceImagesHandler.enableReverbTail = enable;
	updateOnOscReceive(); // Enabling reverb requires an update of the delay line size.
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::enableDirectToBinaural(bool enable)
{
	sourceImagesHandler.enableDirectToBinaural = enable;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::saveRIR()
{
	if (sourceImagesHandler.numSourceImages > 0)
	{
		isRecordingIr = true;
		recordIr();
	}
	else
	{
		AlertWindow::showMessageBoxAsync(AlertWindow::NoIcon, "Room Impulse Response (RIR) not saved!",
		 	"No source images registered from Raytracing Client!\n(Empty RIR)", "OK");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::clearSourceImage()
{
	oscHandler.clear(false);
	changeListenerCallback(&oscHandler);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::updateNumFrequencyBands(int value)
{
	numFreqBands = value;
	updateNumFreqBandrequired = true; // Update flag for multi-threaded update.
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::updateSourceDirectivity(String value)
{
	String filename;
	filename << "directivity/" << value << ".sofa";
	
	sourceImagesHandler.directivityHandler.loadFile(filename);
	updateOnOscReceive();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::updateDirectPathGain(double value)
{
	sourceImagesHandler.directPathGain = value;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::updateEarlyReflectionsGain(double value)
{
	sourceImagesHandler.earlyGain = value;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::updateReverbTailGain(double value)
{
	sourceImagesHandler.reverbTailGain = value;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::updateCrossfadeFactor(double value)
{
	sourceImagesHandler.crossfadeStep = value;
	sourceImagesHandler.binauralEncoder.crossfadeStep = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

String MainComponent::getLogs(bool enable)
{
	if (enable) return String("");
	return oscHandler.getMapContentForGUI();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::enableRecordAmbisonicToDisk(bool enable)
{
	if (enable)
		audioRecorder.startRecording();
	else
		audioRecorder.stopRecording();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void MainComponent::saveOscState()
{
	String output = oscHandler.getMapContentForLog();
	saveStringToDesktop("EVERTims_state", output);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////