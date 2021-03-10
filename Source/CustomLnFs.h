/*
  ==============================================================================

    CustomLnFs.h
    Created: 5 Mar 2021 7:43:39am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RgbColor.h"
#include "GlobalColor.h"
class ModSystemLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModSystemLookAndFeel()
    {
    
    }
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& s) override;
};

class SynthSourceLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SynthSourceLookAndFeel();
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& s) override;
};
