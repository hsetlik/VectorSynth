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
#include "WavetableComponent.h"
class DummyContainer : public juce::DragAndDropContainer, public juce::Component
{
public:
    DummyContainer(juce::AudioProcessorValueTreeState* t, std::vector<std::vector<float>> v) : panel(this, v, t)
    {
        addAndMakeVisible(&panel);
    }
    ~DummyContainer() {}
    void resized() override
    {
        panel.setBounds(0, 0, getWidth(), getHeight());
    }
private:
    SoundSourcePanel panel;
};
