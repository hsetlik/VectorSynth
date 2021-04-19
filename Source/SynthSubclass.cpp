/*
  ==============================================================================

    SynthGenerator.cpp
    Created: 7 Mar 2021 10:10:45am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SynthSubclass.h"
void WavetableSynth::setWaveNames(juce::StringArray names)
{
    for(auto v : voices)
    {
        auto* voice = dynamic_cast<WavetableVoice*>(v);
        voice->oscillator.waveNames = names;
    }
}

void WavetableSynth::setWaveFiles(juce::Array<juce::File> waveFiles)
{
    for(auto v : voices)
    {
        auto* voice = dynamic_cast<WavetableVoice*>(v);
        voice->oscillator.waveFiles = waveFiles;
    }
}

juce::StringArray WavetableSynth::getWaveNames()
{
    auto* voice = dynamic_cast<WavetableVoice*>(voices[0]);
    return voice->oscillator.waveNames;
}

void WavetableSynth::updateOscs(juce::File newTable)
{
    for(auto v : voices)
    {
        auto* voice = dynamic_cast<WavetableVoice*>(v);
        voice->setOscillator(newTable);
    }
}

void WavetableSynth::updateOscs(int index)
{
    for(auto v : voices)
    {
        auto* voice = dynamic_cast<WavetableVoice*>(v);
        voice->setOscillator(index);
    }
}

void WavetableSynth::setSampleRateRecursive(double newRate)
{
    for(auto v : voices)
    {
        auto* voice = dynamic_cast<WavetableVoice*>(v);
        voice->setSampleRate(newRate);
    }
}
