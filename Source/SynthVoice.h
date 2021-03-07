/*
  ==============================================================================

    SynthVoice.h
    Created: 7 Mar 2021 10:06:17am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
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
    void pitchWheelMoved(int newPitchWheelVal) {}
    //=============================================
    void controllerMoved(int controllerNumber, int controllerValue) {}
    //===============================================
    void aftertouchChanged (int newAftertouchValue) {}
    //==============================================
    void channelPressureChanged (int newChannelPressureValue) {}
    //===============================================
    void renderNextBlock (juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples);
};
