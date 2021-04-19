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
#define NUM_VOICES 6

class WavetableSynth : public juce::Synthesiser
{
public:
    WavetableSynth(juce::Array<juce::File>& files)
    {
        for(int i = 0; i < NUM_VOICES; ++i)
        {
            addVoice(new WavetableVoice(files, 0));
        }
        addSound(new WavetableSound());
    }
    void updateParameters(juce::AudioProcessorValueTreeState* tree)
    {
        for(auto v : voices)
        {
            auto* WTVoice = dynamic_cast<WavetableVoice*>(v);
            WTVoice->updateParameters(tree);
        }
    }
    std::vector<std::vector<float>> getDataToGraph(int resolution)
    {
        auto* voice = dynamic_cast<WavetableVoice*>(voices[0]);
        return voice->oscillator.getDataToGraph(resolution);
    }
    void setWaveNames(juce::StringArray waveNames);
    juce::StringArray getWaveNames();
    void setWaveFiles(juce::Array<juce::File> waveFiles);
    void updateOscs(juce::File newTable);
    void updateOscs(int index);
    void setSampleRateRecursive(double newRate);
};

