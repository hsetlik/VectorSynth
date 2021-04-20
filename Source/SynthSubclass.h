/*
  ==============================================================================

    SynthGenerator.h
    Created: 7 Mar 2021 10:10:45am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WavetableProcessor.h"
#include "DAHDSR.h"
#include "SynthVoice.h"


class WavetableSynth : public juce::Synthesiser
{
public:
    void noteOn (int midiChannel, int midiNoteNumber, float velocity) override;
    void noteOff (int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff) override;
    
};

