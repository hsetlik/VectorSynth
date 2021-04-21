/*
  ==============================================================================

    SynthVoice.h
    Created: 7 Mar 2021 10:06:17am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WavetableProcessor.h"
#include "DAHDSR.h"
#define NUM_VOICES 3

class WavetableSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int /*midiNoteNumber*/) //just plays this sound for any midi note
    {
        return true;
    }
    bool appliesToChannel(int /*midiChannel*/) //plays the sound on both channels
    {
        return true;
    }
};

class WavetableVoice : public juce::SynthesiserVoice
{
public:
    WavetableVoice(juce::File defaultWave, juce::AudioProcessorValueTreeState* t);
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<WavetableSound*>(sound) != nullptr;
    }
    void stopNote (float velocity, bool allowTailOff) override;
    void startNote (int midiNoteNumber,
                    float velocity,
                    juce::SynthesiserSound *sound,
                    int currentPitchWheelPosition) override;
    void pitchWheelMoved(int newPitchWheelVal) override {}
    void setSampleRate(double newRate)
    {
        currentRate = newRate;
        setCurrentPlaybackSampleRate(newRate);
    }
    void setPosition(float newPos)
    {
        osc.setPosition(newPos);
    }
    void tickPosition()
    {
        osc.setPosition(*tree->getRawParameterValue(posId));
    }
    //=============================================
    void controllerMoved(int controllerNumber, int controllerValue) override {}
    //===============================================
    void aftertouchChanged (int newAftertouchValue) override {}
    //==============================================
    void channelPressureChanged (int newChannelPressureValue) override{}
    //===============================================
    void renderNextBlock (juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    double fundamental;
    double lastFundamental;
    float phase;
    float phaseDelta;
    float oscPosition;
    WavetableOscHolder osc;
    juce::AudioProcessorValueTreeState* tree;
    juce::String posId;
    int sample;
    int channel;
    float lastVoiceOutput;
    double currentRate;
};

class WavetableSynth : public juce::Synthesiser
{
public:
    static juce::File getDefaultTable()
    {
        auto appFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
        appFolder.setAsCurrentWorkingDirectory();
        auto wFolder = appFolder.getChildFile("MyWavetables");
        auto files = wFolder.findChildFiles(juce::File::findFiles, true);
        return files[0];
    }
    WavetableSynth(juce::AudioProcessorValueTreeState* t);
    void setAllSampleRate(double newRate)
    {
        setCurrentPlaybackSampleRate(newRate);
        for(auto voice : WTvoices)
            voice->setSampleRate(newRate);
    }
    std::vector<std::vector<float>> getDataToGraph()
    {
        return WTvoices[0]->osc.getDataToGraph(128);
    }
    void replaceWave(int index);
    
    juce::StringArray getWaveNames();
    float getPosition();
private:
    std::vector<WavetableVoice*> WTvoices; //  vector of  WavetableVoice pointers so I don't need to dynamic cast continuously
    juce::File waveFolder; // this class stores the wavetable folder, component-side code that needs files or names should be constructed with a pointer to this
    juce::AudioProcessorValueTreeState* tree;
};
