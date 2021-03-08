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
    DummyContainer() : panel(this)
    {
        addAndMakeVisible(&panel);
        
    }
    ~DummyContainer() {}
    void resized() override
    {
        panel.setBounds(0, 0, getWidth(), getHeight());
    }
    void paint(juce::Graphics& g) override
    {
        
    }
private:
    DAHDSRPanel panel;
};
