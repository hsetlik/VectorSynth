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
#include "SynthVoice.h"
class DummyContainer : public juce::DragAndDropContainer, public juce::Component
{
public:
    DummyContainer(juce::AudioProcessorValueTreeState* t,  WavetableSynth* o) : panel(this, t, o)
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
