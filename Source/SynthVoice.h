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
#include "SynthParameter.h"

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
    WavetableVoice(juce::Array<juce::File>& files, int indexId);
    ~WavetableVoice() {}
    //======================================================
    void assignParameterStrings();
    void pitchWheelMoved(int newPitchWheelVal) override {}
    //=============================================
    void controllerMoved(int controllerNumber, int controllerValue) override {}
    //===============================================
    void startNote (int midiNoteNumber,
                    float velocity,
                    juce::SynthesiserSound *sound,
                    int currentPitchWheelPosition) override;
    //===============================================
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<WavetableSound*>(sound) != nullptr;
    }
    void stopNote (float velocity, bool allowTailOff) override;
    void aftertouchChanged (int newAftertouchValue) override {}
    void updateEnvelope()
    {
        envelope.setDelay(pDelay.get());
        envelope.setAttack(pAttack.get());
        envelope.setHold(pHold.get());
        envelope.setDecay(pDecay.get());
        envelope.setSustain(pSustain.get());
        envelope.setRelease(pRelease.get());
    }
    void updateParameters(juce::AudioProcessorValueTreeState* tree)
    {
        pDelay.updateFromTree(tree);
        pAttack.updateFromTree(tree);
        pHold.updateFromTree(tree);
        pDecay.updateFromTree(tree);
        pSustain.updateFromTree(tree);
        pRelease.updateFromTree(tree);
        pOscPosition.updateFromTree(tree);
    }
    void updateOsc()
    {
        oscillator.setPosition(pOscPosition.get());
    }
    //==============================================
    void channelPressureChanged (int newChannelPressureValue) override {}
    //===============================================
    void renderNextBlock (juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    void setSampleRate(double newRate);
    void setOscillator(int fileIndex)
    {
        oscillator.replaceFromFile(fileArray[fileIndex]);
    }
    void setOscillator(juce::File file)
    {
        oscillator.replaceFromFile(file);
    }
    double fundamental;
    WavetableOscHolder oscillator;
    DAHDSR envelope;
    juce::Array<juce::File> fileArray;
    //!  \c renderNextBlock() needs to recursively call each parameter's \c updateFromTree() function
    //! Paramaters should have their base value set once per buffer and should update their modulation on every sample
    SynthParam pDelay;
    SynthParam pAttack;
    SynthParam pHold;
    SynthParam pDecay;
    SynthParam pSustain;
    SynthParam pRelease;
    SynthParam pOscPosition;
private:
    int index;
    float rawSample;
    float envSample;
    
};
