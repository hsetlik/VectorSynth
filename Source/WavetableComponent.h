/*
  ==============================================================================

    WavetableComponent.h
    Created: 7 Mar 2021 9:51:53pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RgbColor.h"
#include "WavetableProcessor.h"

class WavetableDisplay : public juce::Component, public juce::Slider::Listener
{
public:
    WavetableDisplay(std::vector<std::vector<float>> data, juce::Slider* s, float pos = 0.0f);
    ~WavetableDisplay() {};
    void paint(juce::Graphics& g) override;
    void setPosition(float pos); //recalculate colors in here;
    void sliderValueChanged(juce::Slider* s) override;
private:
    int resolution;
    int numTraces;
    std::vector<std::vector<float>> valueSet;
    float position;
    juce::Colour background;
    juce::OwnedArray<juce::Path> traces;
    juce::Colour highlight;
    ColorSet colors;
    ColorSet workingColors;
    std::vector<float> currentValues;
};
