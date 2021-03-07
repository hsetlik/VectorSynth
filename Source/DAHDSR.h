/*
  ==============================================================================

    DAHDSR.h
    Created: 6 Oct 2020 1:07:11pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
struct envDataSet {
    double delay, attack, hold, decay, sustain, release;
};

class DAHDSR
{
public:
    enum envPhase
    {
        delayPhase,
        attackPhase,
        holdPhase,
        decayPhase,
        sustainPhase,
        releasePhase,
        noteOff
    };
    //functions
    DAHDSR(int ind) : factor(1.0f), sampleRate(44100), index(ind)
    {
        trigger = false;
        samplesIntoPhase = 0;
        currentPhase = noteOff;
    }
    ~DAHDSR() {}
    void triggerOn()
    {
        trigger = true;
        samplesInPhase = floor(settings.delay * (sampleRate / 1000));
        samplesIntoPhase = 0;
        currentPhase = delayPhase;
    }
    void triggerOff()
    {
        trigger = false;
        currentPhase = releasePhase;
        samplesIntoPhase = 0;
        samplesInPhase = settings.release * (sampleRate / 1000);
        factor = exp((log(minLevel) - log(settings.sustain)) /samplesInPhase);
    }
    void setSampleRate(double value) {sampleRate = value;}
    float process(float input);
    envPhase getPhase() {return currentPhase;}
    double output;
    void setDelay(double val) {settings.delay = val;}
    void setAttack(double val) {settings.attack = val;}
    void setHold(double val) {settings.hold = val;}
    void setDecay(double val) {settings.decay = val;}
    void setSustain(double val) {settings.sustain = val;}
    void setRelease(double val) {settings.release = val;}
private:
    envDataSet settings;
    //data
    envPhase currentPhase;
    unsigned long long samplesIntoPhase;
    unsigned long long samplesInPhase;
    double factor;
    float minLevel = 0.00001f;
    double sampleRate;
    int index;
    bool trigger;
};
