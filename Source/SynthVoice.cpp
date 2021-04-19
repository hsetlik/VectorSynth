/*
  ==============================================================================

    SynthVoice.cpp
    Created: 7 Mar 2021 10:06:17am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SynthVoice.h"
WavetableVoice::WavetableVoice(juce::Array<juce::File>& files, int indexId) :
oscillator(files[indexId]),
fileArray(files),
index(indexId)
{
    assignParameterStrings();
}
void WavetableVoice::assignParameterStrings()
{
    auto iStr = juce::String(index);
    pDelay.setTreeString("delayParam" + iStr);
    pDelay.setRange(DELAY_MIN, DELAY_MAX);
    pDelay.set(DELAY_DEFAULT);
    pAttack.setTreeString("attackParam" + iStr);
    pAttack.setRange(ATTACK_MIN, ATTACK_MAX);
    pAttack.set(ATTACK_DEFAULT);
    pHold.setTreeString("holdParam" + iStr);
    pHold.setRange(HOLD_MIN, HOLD_MAX);
    pHold.set(HOLD_DEFAULT);
    pDecay.setTreeString("decayParam" + iStr);
    pDecay.setRange(DECAY_MIN, DECAY_MAX);
    pDecay.set(DECAY_DEFAULT);
    pSustain.setTreeString("sustainParam" + iStr);
    pSustain.setRange(SUSTAIN_MIN, SUSTAIN_MAX);
    pSustain.set(SUSTAIN_DEFAULT);
    pRelease.setTreeString("releaseParam" + iStr);
    pRelease.setRange(RELEASE_MIN, RELEASE_MAX);
    pRelease.set(RELEASE_DEFAULT);
    
    pOscPosition.setTreeString("oscPositionParam" + iStr);
    pOscPosition.setRange(WTPOS_MIN, WTPOS_MAX);
    pOscPosition.set(WTPOS_DEFAULT);
}
void WavetableVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    fundamental = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    envelope.triggerOn();
}

void WavetableVoice::stopNote(float velocity, bool allowTailOff)
{
    envelope.triggerOff();
    
}
void WavetableVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    outputBuffer.clear();
    for(int i = startSample; i < (startSample + numSamples); ++i)
    {
        updateOsc();
        updateEnvelope();
        rawSample = oscillator.getSample(fundamental);
        envSample = envelope.process(rawSample);
        for(int chan = 0; chan < outputBuffer.getNumChannels(); ++chan)
        {
            outputBuffer.addSample(chan, i, envSample * 0.4f);
        }
    }
}

void WavetableVoice::setSampleRate(double newRate)
{
    oscillator.setSampleRate(newRate);
    envelope.setSampleRate(newRate);
}
