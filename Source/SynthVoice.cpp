/*
  ==============================================================================

    SynthVoice.cpp
    Created: 7 Mar 2021 10:06:17am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SynthVoice.h"
WavetableVoice::WavetableVoice(juce::AudioProcessorValueTreeState* t, WavetableOscHolder* o) :
    fundamental(440.0f),
    phase(0.0f),
    phaseDelta(0.0f),
    pOsc(o),
    env(t),
    tree(t),
    posId("oscPositionParam")
{
    
}
void WavetableVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    fundamental = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    env.triggerOn();
    phaseDelta = (float)fundamental / currentRate;
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
    //updateParams();
    for(sample = startSample; sample < (startSample + numSamples); ++sample)
    {
        phase += phaseDelta;
        if(phase > 1.0f)
            phase -= 1.0f;
        lastVoiceOutput = pOsc->getSample(phase, fundamental);
        /*
        if(fundamental != 0.00f)
            printf("sample\n");
         */
        for(channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            outputBuffer.addSample(channel, sample, lastVoiceOutput * 0.5f);
        }
    }
}

WavetableSynth::WavetableSynth(juce::AudioProcessorValueTreeState* t) : osc(getDefaultTable()), tree(t)
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
        addVoice(new WavetableVoice(tree, &osc));
        auto pVoice = dynamic_cast<WavetableVoice*>(voices.getLast());
        WTvoices.push_back(pVoice);
    }
    addSound(new WavetableSound());
}

void WavetableSynth::replaceWave(int index)
{
    auto files = waveFolder.findChildFiles(juce::File::findFiles, true);
    auto file = files[index];
    osc.replaceFromFile(file);
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
    return osc.getPosition();
}


