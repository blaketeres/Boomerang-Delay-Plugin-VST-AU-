/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
*/
class BoomerangAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    BoomerangAudioProcessor();
    ~BoomerangAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void delayChannel(AudioSampleBuffer& delayBuffer, float* channelInput, int channel, const int channelDataBufferSize, int& samplePosition, int& delaySamplePosition);
    
    int positiveModulo(int i, int n);
    
    //void modulateChannel(AudioSampleBuffer& modulationBuffer, AudioSampleBuffer& wetStereoBuffer, int channel);
    
    AudioSampleBuffer delayBufferStereo;
    //AudioSampleBuffer modulationBuffer;
    AudioSampleBuffer wetBufferStereo;
    
    int delayTimeInSamples;
    
    int samplePositionLeft = 0;
    int samplePositionRight = 0;
    int delaySamplePositionLeft = 0;
    int delaySamplePositionRight = 0;
    int modulationPositionLeft = 0;
    int modulationPositionRight = 0;
    
    int* samplePositionStereo [2] = {&samplePositionLeft, &samplePositionRight};
    int* delaySamplePositionStereo [2] = {&delaySamplePositionLeft, &delaySamplePositionRight};
    
    int stereoOffsetLeft = 0;
    int stereoOffsetRight = 0;
    
    double feedbackValue;
    double stereoOffsetValue;
    double modulationValue;
    double dryVolume;
    double wetVolume;
    
    bool sync;
    
    int bpm = 120;
    
    AudioPlayHead* playHead;
    AudioPlayHead::CurrentPositionInfo currentPositionInfo;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BoomerangAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
