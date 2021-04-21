/*
  ==============================================================================

    SynthVoice.cpp
    Created: 7 Mar 2021 10:06:17am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SynthVoice.h"
WavetableVoice::WavetableVoice(juce::File defaultWave, juce::AudioProcessorValueTreeState* t, int voiceIdx) :
    fundamental(440.0f),
    osc(defaultWave),
    tree(t),
    posId("oscPositionParam"),
    voiceIndex(voiceIdx),
    noteOn(false)
{
    
}
void WavetableVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    fundamental = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    printf("%d\n", voiceIndex);
    noteOn = true;
    //env.triggerOn();
    
}
void WavetableVoice::stopNote(float velocity, bool allowTailOff)
{
    //env.triggerOff();
    noteOn = false;
    clearCurrentNote();
}

void WavetableVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    outputBuffer.clear();
    //updateParams();
    for(sample = startSample; sample < (startSample + numSamples); ++sample)
    {
        lastVoiceOutput = 0.0f;
        if(noteOn)
        {
            tickPosition();
            lastVoiceOutput = osc.getSample(fundamental);
        }
        for(channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, sample, lastVoiceOutput);
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
        addVoice(new WavetableVoice(defaultWave, tree, i));
        auto pVoice = dynamic_cast<WavetableVoice*>(voices.getLast());
        WTvoices.push_back(pVoice);
    }
    addSound(new WavetableSound());
}

void WavetableSynth::replaceWave(int index)
{
    auto files = waveFolder.findChildFiles(juce::File::findFiles, true);
    auto file = files[index];
    for(auto voice: WTvoices)
        voice->osc.replaceFromFile(file);
}

juce::StringArray WavetableSynth::getWaveNames()
{
    auto files = waveFolder.findChildFiles(juce::File::findFiles, true);
    juce::StringArray arr;
    for(auto file : files)
    {
        arr.add(file.getFileName());
    }
    return arr;
}

float WavetableSynth::getPosition()
{
    return WTvoices[0]->osc.getPosition();
}


