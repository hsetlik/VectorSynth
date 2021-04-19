/*
  ==============================================================================

    SynthVoice.cpp
    Created: 7 Mar 2021 10:06:17am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SynthVoice.h"
WavetableVoice::WavetableVoice(juce::Array<juce::File>& files) :
oscillator(files[0]),
fileArray(files)
{
    
}

void WavetableVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    fundamental = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    envelope.triggerOn();
}

void WavetableVoice::stopNote(float velocity, bool allowTailOff)
{
    
}

void WavetableVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    
}
