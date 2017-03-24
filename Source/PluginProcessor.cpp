/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
BoomerangAudioProcessor::BoomerangAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

BoomerangAudioProcessor::~BoomerangAudioProcessor()
{
}

//==============================================================================
const String BoomerangAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BoomerangAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BoomerangAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double BoomerangAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BoomerangAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BoomerangAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BoomerangAudioProcessor::setCurrentProgram (int index)
{
}

const String BoomerangAudioProcessor::getProgramName (int index)
{
    return String();
}

void BoomerangAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void BoomerangAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialization that you need..
    
    delayBufferStereo.setSize(2, 6 * (int)sampleRate);
    delayBufferStereo.clear();
    
    //modulationBuffer.setSize(2, 6 * (int)sampleRate);
    //modulationBuffer.clear();
    
    wetBufferStereo.setSize(2, samplesPerBlock);
    wetBufferStereo.clear();
}

void BoomerangAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BoomerangAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BoomerangAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    
    // Quick check to clear unused channel inputs in the case of a mismatched channel setup
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Process block
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        delayChannel(delayBufferStereo, channelData, channel, buffer.getNumSamples(), *samplePositionStereo[channel], *delaySamplePositionStereo[channel]);
        
        // apply wet/dry mix
        buffer.applyGain(channel, 0, buffer.getNumSamples(), dryVolume);
        wetBufferStereo.applyGain(channel, 0, wetBufferStereo.getNumSamples(), wetVolume);
        buffer.addFrom(channel, 0, wetBufferStereo, channel, 0, buffer.getNumSamples());
    }
}

void BoomerangAudioProcessor::delayChannel(AudioSampleBuffer& delayBuffer, float* channelInput, int channel, const int channelDataBufferSize, int& samplePosition, int& delaySamplePosition)
{
    
    float* delayWrite = delayBuffer.getWritePointer(channel);
    const float* delayRead = delayBuffer.getReadPointer(channel);
    //const float* flangeRead = delayBuffer.getReadPointer(channel);
    
    float* wetBufferWrite = wetBufferStereo.getWritePointer(channel);
    
    int flangeIndex;
    
    // Iterate through circular buffer with read and write pointers and their offset
    for (int i = 0; i < channelDataBufferSize; ++i)
    {
        // If either the read or write pointer reaches an out of bounds index, wrap it back to the beginning
        if (samplePosition == delayBuffer.getNumSamples())
            samplePosition = 0;
        if (delaySamplePosition == delayBuffer.getNumSamples())
            delaySamplePosition = 0;
        
        flangeIndex = positiveModulo(delaySamplePosition - int((modulationValue * 60) + 20), delayBuffer.getNumSamples());
        
        // Read from delay buffer before writing
        wetBufferWrite[i] = delayRead[delaySamplePosition] + (delayRead[flangeIndex] * (modulationValue / 1.4));
        
        // Write the newest input sample with feedback value
        delayWrite[samplePosition] = channelInput[i] + (feedbackValue * wetBufferWrite[i]);
        
        // Increment pointer indexes
        delaySamplePosition += 1;
        samplePosition += 1;
    }
}

inline int BoomerangAudioProcessor::positiveModulo(int i, int n)
{
    return (i % n + n) % n;
}


//==============================================================================
bool BoomerangAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* BoomerangAudioProcessor::createEditor()
{
    return new BoomerangAudioProcessorEditor (*this);
}

//==============================================================================
void BoomerangAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BoomerangAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BoomerangAudioProcessor();
}
