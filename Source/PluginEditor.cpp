/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
BoomerangAudioProcessorEditor::BoomerangAudioProcessorEditor (BoomerangAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    CustomLookAndFeel::setDefaultLookAndFeel(&myLookAndFeel);
    
    setSize(800, 350);
    
    // Add and control slider parameters
    feedback.setSliderStyle(Slider::RotaryVerticalDrag);
    feedback.setRange(0.0, 1.0, 0.0);
    feedback.setValue(0.3);
    feedback.setDoubleClickReturnValue(true, 0.3);
    feedback.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 40);
    feedback.addListener(this);
    processor.feedbackValue = feedback.getValue();
    
    delayTime.setSliderStyle(Slider::RotaryVerticalDrag);
    delayTime.setRange(-16, -1, 1);
    delayTime.setValue(-11);
    delayTime.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 40);
    delayTime.snapValue(delayTime.getValue(), Slider::absoluteDrag);
    delayTime.setTextBoxIsEditable(false);
    delayTime.addListener(this);
    calculateDelayTimeInSamples(delayTime.getValue(), processor.bpm);
    
    stereoOffset.setSliderStyle(Slider::RotaryVerticalDrag);
    stereoOffset.setRange(-1.0, 1.0, 0.0);
    stereoOffset.setValue(0.0);
    stereoOffset.setDoubleClickReturnValue(true, 0.0);
    stereoOffset.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 40);
    stereoOffset.addListener(this);
    processor.stereoOffsetValue = stereoOffset.getValue();
    
    modulation.setSliderStyle(Slider::RotaryVerticalDrag);
    modulation.setRange(0.0, 1.0, 0.0);
    modulation.setValue(0.0);
    modulation.setDoubleClickReturnValue(true, 0.0);
    modulation.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 40);
    modulation.addListener(this);
    processor.modulationValue = modulation.getValue();
    
    mix.setSliderStyle(Slider::RotaryVerticalDrag);
    mix.setRange(0.0, 1.0, 0.0);
    mix.setValue(0.5);
    mix.setDoubleClickReturnValue(true, 0.5);
    mix.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 40);
    mix.addListener(this);
    mix.setTextBoxIsEditable(false);
    processor.wetVolume = mix.getValue();
    processor.dryVolume = mix.getMaximum() - processor.wetVolume;
    
    sync.setToggleState(true, sendNotification);
    sync.setClickingTogglesState(true);
    sync.addListener(this);
    processor.sync = true;
    
    addAndMakeVisible(&feedback);
    addAndMakeVisible(&delayTime);
    addAndMakeVisible(&stereoOffset);
    addAndMakeVisible(&modulation);
    addAndMakeVisible(&mix);
    addAndMakeVisible(&sync);
}

BoomerangAudioProcessorEditor::~BoomerangAudioProcessorEditor()
{
}

//==============================================================================
void BoomerangAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(myColour);
    g.drawImage(background, 0, 0, 800, 300, 0, 0, 800, 300);
}

void BoomerangAudioProcessorEditor::resized()
{
    feedback.setBounds(67, 139, 90, 150);
    delayTime.setBounds(200, 139, 90, 150);
    stereoOffset.setBounds(333, 139, 90, 150);
    modulation.setBounds(466, 139, 90, 150);
    mix.setBounds(635, 139, 90, 150);
    sync.setSize(80, 58);
    sync.setCentrePosition(256, 310);
}

void BoomerangAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &feedback)
    {
        processor.feedbackValue = feedback.getValue();
    }
    if (slider == &delayTime)
    {
        calculateDelayTimeInSamples(delayTime.getValue(), processor.bpm);
    }
    if (slider == &stereoOffset)
    {
        processor.stereoOffsetValue = stereoOffset.getValue();
        modifyStereoOffset(stereoOffset.getValue(), processor.stereoOffsetLeft, processor.stereoOffsetRight);
        calculateDelayTimeInSamples(delayTime.getValue(), processor.bpm);
    }
    if (slider == &modulation)
    {
        processor.modulationValue = modulation.getValue();
    }
    if (slider == &mix)
    {
        processor.wetVolume = mix.getValue();
        processor.dryVolume = mix.getMaximum() - processor.wetVolume;
    }
}


void BoomerangAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == &sync)
    {
        if (processor.sync == true)
        {
            processor.sync = false;
            delayTime.setRange(1, 2000, 1);
            delayTime.setValue(1000);
            calculateDelayTimeInSamples(delayTime.getValue(), processor.bpm);
            delayTime.setDoubleClickReturnValue(true, 1000);
            delayTime.setTextBoxIsEditable(true);
        }
        else
        {
            processor.sync = true;
            delayTime.setRange(-16, -1, 1);
            delayTime.setValue(-11);
            calculateDelayTimeInSamples(delayTime.getValue(), processor.bpm);
            delayTime.setDoubleClickReturnValue(false, -11);
            delayTime.setTextValueSuffix("");
            delayTime.setTextBoxIsEditable(false);
        }
    }
}


void BoomerangAudioProcessorEditor::calculateDelayTimeInSamples(int delayTimeValue, int bpm)
{
    
    int delayTime_ms;
    
    if (processor.sync == true)
    {
        delayTime_ms = calculateDelaySync(delayTime.getValue(), processor.bpm);
    }
    else
    {
        delayTime_ms = delayTime.getValue();
    }
    
    processor.delayTimeInSamples = processor.getSampleRate() * (delayTime_ms / 1000.0);
    
    processor.delaySamplePositionLeft = positiveModulo((processor.samplePositionLeft - processor.delayTimeInSamples) +
                                                       processor.stereoOffsetLeft, processor.delayBufferStereo.getNumSamples());
    
    processor.delaySamplePositionRight = positiveModulo((processor.samplePositionRight - processor.delayTimeInSamples) +
                                                        processor.stereoOffsetRight, processor.delayBufferStereo.getNumSamples());
    
}



void BoomerangAudioProcessorEditor::modifyStereoOffset(double stereoOffsetValue, int& stereoOffsetLeft, int& stereoOffsetRight)
{
    int limit = (int)(processor.getSampleRate() / 10.0);
    
    stereoOffsetRight = limit * stereoOffsetValue;
    stereoOffsetLeft = limit * (-1 * stereoOffsetValue);
}



inline int BoomerangAudioProcessorEditor::positiveModulo(int i, int n)
{
    return (i % n + n) % n;
}


int BoomerangAudioProcessorEditor::calculateDelaySync(int delayTimeValue, int bpm)
{
    delayTimeValue = delayTimeValue + 16;
    int ms = 60000;
    int constant = 4;
    float fraction;
    float modifier;
    
    switch (delayTimeValue)
    {
        case 0:
            fraction = 1.0/1.0;
            modifier = 1.0;
            break;
        case 1:
            fraction = 1.0/2.0;
            modifier = 3.0/2.0;
            break;
        case 2:
            fraction = 1.0/2;
            modifier = 1.0;
            break;
        case 3:
            fraction = 1.0/2.0;
            modifier = 2.0/3.0;
            break;
        case 4:
            fraction = 1.0/4.0;
            modifier = 3.0/2.0;
            break;
        case 5:
            fraction = 1.0/4.0;
            modifier = 1.0;
            break;
        case 6:
            fraction = 1.0/4.0;
            modifier = 2.0/3.0;
            break;
        case 7:
            fraction = 1.0/8.0;
            modifier = 3.0/2.0;
            break;
        case 8:
            fraction = 1.0/8.0;
            modifier = 1.0;
            break;
        case 9:
            fraction = 1.0/8.0;
            modifier = 2.0/3.0;
            break;
        case 10:
            fraction = 1.0/16.0;
            modifier = 3.0/2.0;
            break;
        case 11:
            fraction = 1.0/16.0;
            modifier = 1.0;
            break;
        case 12:
            fraction = 1.0/16.0;
            modifier = 2.0/3.0;
            break;
        case 13:
            fraction = 1.0/32.0;
            modifier = 3.0/2.0;
            break;
        case 14:
            fraction = 1.0/32.0;
            modifier = 1.0;
            break;
        case 15:
            fraction = 1.0/32.0;
            modifier = 2.0/3.0;
            break;
    }
    
    int delayTimeValue_ms = ((ms / bpm) * constant);
    float q = fraction * modifier;
    delayTimeValue_ms = delayTimeValue_ms * q;
    delayTimeValue_ms = roundFloatToInt(delayTimeValue_ms);
    return delayTimeValue_ms;
};


String DelaySlider::getTextFromValue(double value)
{
    int x = int(value);
    std::string s;
    if (value < 0) {
        switch (x) {
            case -16: s = "1/1";
                break;
            case -15: s = "1/2.";
                break;
            case -14: s = "1/2";
                break;
            case -13: s = "1/2T";
                break;
            case -12: s = "1/4.";
                break;
            case -11: s = "1/4";
                break;
            case -10: s = "1/4T";
                break;
            case -9: s = "1/8.";
                break;
            case -8: s = "1/8";
                break;
            case -7: s = "1/8T";
                break;
            case -6: s = "1/16T.";
                break;
            case -5: s = "1/16";
                break;
            case -4: s = "1/16T";
                break;
            case -3: s = "1/32.";
                break;
            case -2: s = "1/32";
                break;
            case -1: s = "1/32T";
                break;
        }
    }
    else
    {
        s = std::to_string(x) + " ms";
        return s;
    }
    return s;
}


String OtherSlider::getTextFromValue(double value)
{
    std::string s;
    int r = value * 100;
    s = std::to_string(r);
    return s;
}

String ModulationSlider::getTextFromValue(double value)
{
    std::string s;
    int r = value * 100;
    s = std::to_string(r);
    return s;
}

double OtherSlider::getValueFromText(const String& s)
{
    double d = s.getDoubleValue();
    d = d / 100.00;
    return d;
}

String MixSlider::getTextFromValue(double value)
{
    std::string s;
    int d = value * 100;
    int w = 100 - d;
    std::string a = std::to_string(d);
    std::string b = std::to_string(w);
    s = a + "/" + b;
    return s;
}


void CustomLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
                                         const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    AffineTransform rotator;
    shadowProperties.drawForImage(g, knob);
    shadowProperties.colour = Colour::fromFloatRGBA(249, 249, 249, 0.5);
    shadow.setShadowProperties(shadowProperties);
    sliderPos *= 2.0;
    sliderPos -= 1.0;
    float rotationValue = sliderPos * 2.6;
    g.drawImageTransformed(knob, rotator.rotated(rotationValue, knob.getWidth()/2, knob.getHeight()/2));
}

void CustomLookAndFeel::drawToggleButton(Graphics& g, ToggleButton& t, bool isMouseOverButton, bool isButtonDown)
{
    if (t.getToggleStateValue() == false)
    {
        g.drawImage(toggleOff, 0, 0, 58, 42, 0, 0, 80, 58);
    }
    else
    {
        g.drawImage(toggleOn, 0, 0, 58, 42, 0, 0, 80, 58);
    }
}

void CustomLookAndFeel::drawLabel(Graphics& g, Label& l)
{
    g.setFont(myLabelFont);
    myLabelFont.setHeight(15);
    g.drawText(l.getTextValue().toString(), 0, -14, 58, 30, Justification::horizontallyCentred | Justification::bottom);
}










