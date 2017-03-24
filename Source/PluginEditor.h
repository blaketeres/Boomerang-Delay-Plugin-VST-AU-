/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/


class DelaySlider : public Slider
{
public:
    String getTextFromValue(double value) override;
};

class OtherSlider : public Slider
{
public:
    String getTextFromValue(double value) override;
    double getValueFromText(const String& s) override;
};

class MixSlider : public Slider
{
public:
    String getTextFromValue(double value) override;
};

class ModulationSlider : public Slider
{
public:
    String getTextFromValue(double value) override;
};





class CustomLookAndFeel : public LookAndFeel_V3
{
public:
    
    Image knob = ImageCache::getFromMemory(BinaryData::KNOB_png, BinaryData::KNOB_pngSize);
    Image toggleOff = ImageCache::getFromMemory(BinaryData::Toggle_png, BinaryData::Toggle_pngSize);
    Image toggleOn = ImageCache::getFromMemory(BinaryData::ToggleOn_png, BinaryData::ToggleOn_pngSize);
    
    Typeface::Ptr labelFont = Typeface::createSystemTypefaceFor(BinaryData::Futura_ttc, BinaryData::Futura_ttcSize);
    Font myLabelFont = Font(labelFont);
    
    DropShadowEffect shadow;
    DropShadow shadowProperties;
    
    void drawRotarySlider(Graphics& g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPos,
                          const float rotaryStartAngle,
                          const float rotaryEndAngle,
                          Slider& slider) override;
    
    void drawToggleButton(juce::Graphics &, juce::ToggleButton &, bool isMouseOverButton, bool isButtonDown) override;
    
    void drawLabel(Graphics &, Label &) override;
};



class BoomerangAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener, public Button::Listener
{
public:
    BoomerangAudioProcessorEditor (BoomerangAudioProcessor&);
    ~BoomerangAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(Slider* slider) override;
    void buttonClicked(Button* button) override;
    
    int calculateDelaySync(int delayTimeValue, int bpm);
    void calculateDelayTimeInSamples(int delayTimeValue, int bpm);
    
    void modifyStereoOffset(double stereoOffsetValue, int& stereoOffsetLeft, int& stereoOffsetRight);
    
    int positiveModulo(int i, int n);
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BoomerangAudioProcessor& processor;
    
    CustomLookAndFeel myLookAndFeel;
    
    OtherSlider feedback;
    DelaySlider delayTime;
    OtherSlider stereoOffset;
    ModulationSlider modulation;
    MixSlider mix;
    ToggleButton sync;
    
    Colour myColour = Colour(249, 249, 249);
    
    Image background = ImageCache::getFromMemory(BinaryData::Background2_png, BinaryData::Background2_pngSize);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BoomerangAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
