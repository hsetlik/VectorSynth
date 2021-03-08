/*
  ==============================================================================

    DragDropContainer.h
    Created: 2 Mar 2021 6:10:17pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "EnvelopeComponent.h"
class DummyContainer : public juce::DragAndDropContainer, public juce::Component
{
public:
    DummyContainer(juce::AudioProcessorValueTreeState* t)
    {
        slider1.setRange(20.0f, 20000.0f);
        slider1.setValue(150.0f);
        slider1.setSliderStyle(juce::Slider::Rotary);
        slider2.setRange(0.0f, 1.0f);
        slider2.setValue(0.0f);
        slider2.setSliderStyle(juce::Slider::Rotary);
        
        addAndMakeVisible(&slider1);
        addAndMakeVisible(&slider2);
        attachment1.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(*t, "frequency", slider1));
        attachment2.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(*t, "wavetablePos", slider2));
    }
    ~DummyContainer() {}
    void resized() override
    {
        auto n = getWidth() / 2;
        slider1.setBounds(0, 0, n, n);
        slider2.setBounds(n, 0, n, n);
    }
    void paint(juce::Graphics& g) override
    {
        
    }
private:
    juce::Slider slider1;
    juce::Slider slider2;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment1;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment2;
};
