/*
  ==============================================================================

    SynthVoice.cpp
    Created: 7 Mar 2021 10:06:17am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SynthVoice.h"
WavetableVoice::WavetableVoice(juce::AudioProcessorValueTreeState* t, juce::File& defaultWave) :
    fundamental(440.0f),
    osc(defaultWave),
    env(t),
    tree(t),
    posId("oscPositionParam")
{
    
}
void WavetableVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    fundamental = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    env.triggerOn();
}
void WavetableVoice::stopNote(float velocity, bool allowTailOff)
{
    env.triggerOff();
    if(!allowTailOff || !env.isActive())
        clearCurrentNote();
}

void WavetableVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    outputBuffer.clear();
    updateParams();
    for(sample = startSample; sample < (startSample + numSamples); ++sample)
    {
        lastVoiceOutput = env.process(osc.getSample(fundamental));
        for(channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, sample, lastVoiceOutput * 0.6f);
        }
    }
}

WavetableSynth::WavetableSynth(juce::AudioProcessorValueTreeState* t) : tree(t)
{
    auto appFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    appFolder.setAsCurrentWorkingDirectory();
    waveFolder = appFolder.getChildFile("MyWavetables");
    if(waveFolder.exists() && waveFolder.isDirectory())
    {
        printf("wave folder exists\n");
        waveFolder.setAsCurrentWorkingDirectory();
    }
    else
    {
        waveFolder.createDirectory();
        waveFolder.setAsCurrentWorkingDirectory();
        printf("wave folder created\n");
    }
    printf("Folder is at: %s\n", waveFolder.getFullPathName().toRawUTF8());
    auto files = waveFolder.findChildFiles(juce::File::findFiles, true);
    auto defaultWave = files[0];
    for(int i = 0; i < NUM_VOICES; ++i)
    {
        addVoice(new WavetableVoice(tree, defaultWave));
        auto pVoice = dynamic_cast<WavetableVoice*>(voices.getLast());
        WTvoices.push_back(pVoice);
    }
    addSound(new WavetableSound());
}
