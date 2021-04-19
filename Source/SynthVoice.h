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
    WavetableVoice(juce::Array<juce::File>& files);
    void pitchWheelMoved(int newPitchWheelVal) override {}
    //=============================================
    void controllerMoved(int controllerNumber, int controllerValue) override {}
    //===============================================
    void startNote (int midiNoteNumber,
                    float velocity,
                    juce::SynthesiserSound *sound,
                    int currentPitchWheelPosition) override;
    //===============================================
    void stopNote (float velocity, bool allowTailOff) override;
    void aftertouchChanged (int newAftertouchValue) override {}
    //==============================================
    void channelPressureChanged (int newChannelPressureValue) override {}
    //===============================================
    void renderNextBlock (juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    
    double fundamental;
    WavetableOscHolder oscillator;
    DAHDSR envelope;
    juce::Array<juce::File> fileArray;
};
