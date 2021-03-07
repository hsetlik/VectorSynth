/*
  ==============================================================================

    SliderTypes.h
    Created: 7 Mar 2021 12:46:05pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RgbColor.h"
#include "CustomLnFs.h"

enum EnvSliderType
{
    DelaySlider,
    AttackSlider,
    HoldSlider,
    DecaySlider,
    SustainSlider,
    ReleaseSlider
};

class EnvelopeLabel : public juce::Label, public juce::Slider::Listener
{
public:
    EnvelopeLabel(juce::Slider* s, std::string suff) : source(s), suffix(suff)
    {
        s->addListener(this);
        auto vcrFont = juce::Font("WW Digital", 8.0f, 0);
        setEditable(true);
        getLookAndFeel().setDefaultSansSerifTypeface(getLookAndFeel().getTypefaceForFont(vcrFont));
        setJustificationType(juce::Justification::centred);
        setMinimumHorizontalScale(0.25);
        juce::String tempText;
        auto valueToDraw = source->getValue();
        auto fullString = juce::String(valueToDraw);
        if(valueToDraw < 100.0f)
            tempText = fullString.substring(0, 2);
        else if(valueToDraw < 1000.0f)
            tempText = fullString.substring(0, 3);
        else
            tempText = fullString.substring(0, 4);
        auto labelText = tempText +  suffix;
        if(suffix == " ")
            labelText = tempText;
        setText(labelText, juce::dontSendNotification);
    }
    void sliderValueChanged(juce::Slider* s) override
    {
        juce::String tempText;
        juce::String labelText;
        auto valueToDraw = source->getValue();
        auto fullString = juce::String(valueToDraw);
        if(suffix == " ")
        {
            labelText = fullString;
        }
        else
        {
            if(valueToDraw < 100.0f)
                tempText = fullString.substring(0, 3);
            else if(valueToDraw < 1000.0f)
                tempText = fullString.substring(0, 4);
            else
                tempText = fullString.substring(0, 5);
            labelText = tempText +  suffix;
        }
        setText(labelText, juce::dontSendNotification);
    }
    void textWasEdited() override
    {
        auto str = getText(true);
        auto val = str.getDoubleValue();
        source->setValue(val);
    }
    juce::Slider* source;
    std::string suffix;
};

class EnvelopeDial : public juce::Component
{
public:
    EnvelopeDial(EnvSliderType t, int index) : label(&slider, " "), type(t), srcIndex(index)
    {
        addAndMakeVisible(&slider);
        slider.setSliderStyle(juce::Slider::Rotary);
        slider.setLookAndFeel(&lnf);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
        addAndMakeVisible(&label);
        double rStart, rEnd, vDefault;
        switch(type)
        {
            case DelaySlider:{
                rStart = 0.0f; rEnd = 20000.0f; vDefault = 0.0f;
                break;
            }
            case AttackSlider:{
                rStart = 0.0f; rEnd = 20000.0f; vDefault = 25.0f;
                break;
            }
            case HoldSlider:{
                rStart = 0.0f; rEnd = 20000.0f; vDefault = 0.0f;
                break;
            }
            case DecaySlider:{
                rStart = 0.0f; rEnd = 20000.0f; vDefault = 100.0f;
                break;
            }
            case SustainSlider:{
                rStart = 0.0f; rEnd = 1.0f; vDefault = 0.6f;
                break;
            }
            case ReleaseSlider:{
                rStart = 0.0f; rEnd = 20000.0f; vDefault = 150.0f;
                break;
            }
        }
        slider.setRange(rStart, rEnd);
        slider.setValue(vDefault);
    }
    void resized() override //note: this component should always be drawn with a 3:4 aspect ratio
    {
        auto n = getHeight() / 4;
        slider.setBounds(0, 0, 3 * n, 3 * n);
        label.setBounds(0, 3 * n, 3 * n, n);
    }
private:
    SynthSourceLookAndFeel lnf;
    juce::Slider slider;
    EnvelopeLabel label;
    EnvSliderType type;
    int srcIndex;
};
