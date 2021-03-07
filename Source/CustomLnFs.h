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
class ModSystemLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModSystemLookAndFeel()
    {
        colors.add(Color::RGBColor(125, 126, 129), "dullGray");
        colors.add(Color::RGBColor(255, 236, 95), "thumbYellow");
        colors.add(Color::RGBColor(32, 139, 181), "thumbBlue");
    }
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& s) override;
private:
    ColorSet colors;
};

class GeneralLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& s) override;
private:
    ColorSet colors;
};
