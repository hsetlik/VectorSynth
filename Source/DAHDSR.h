/*
  ==============================================================================

    DAHDSR.h
    Created: 6 Oct 2020 1:07:11pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <math.h>
// macros to define parameter limits
// these should be here in every header that defines an object which is associated with parameters
#define DELAY_MIN 0.0f
#define DELAY_MAX 20000.0f
#define DELAY_DEFAULT 0.0f
#define DELAY_CENTER 1000.0f

#define ATTACK_MIN 0.0f
#define ATTACK_MAX 20000.0f
#define ATTACK_DEFAULT 20.0f
#define ATTACK_CENTER 1000.0f

#define HOLD_MIN 0.0f
#define HOLD_MAX 20000.0f
#define HOLD_DEFAULT 0.0f
#define HOLD_CENTER 1000.0f

#define DECAY_MIN 0.0f
#define DECAY_MAX 20000.0f
#define DECAY_DEFAULT 40.0f
#define DECAY_CENTER 1000.0f

#define SUSTAIN_MIN 0.0f
#define SUSTAIN_MAX 1.0f
#define SUSTAIN_DEFAULT 0.6f

#define RELEASE_MIN 0.0f
#define RELEASE_MAX 20000.0f
#define RELEASE_DEFAULT 80.0f
#define RELEASE_CENTER 1000.0f

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
    juce::AudioProcessorValueTreeState* tree;
    //functions
    //! the index should be incremented for each DAHDSR you plan to attach to a given APVTS
    DAHDSR(int ind, juce::AudioProcessorValueTreeState* t) : tree(t), factor(1.0f), sampleRate(44100), index(ind)
    {
        trigger = false;
        samplesIntoPhase = 0;
        currentPhase = noteOff;
        output = 0.0f;
        auto iStr = juce::String(ind);
        delayId = "delayParam" + iStr;
        attackId = "attackParam" + iStr;
        holdId = "holdParam" + iStr;
        decayId = "decayParam" + iStr;
        sustainId = "sustainParam" + iStr;
        releaseId = "releaseParam" + iStr;
    }
    //!  Constructor with no suffix for strings (if you only need one envelope attached to the APVTS)
    DAHDSR(juce::AudioProcessorValueTreeState* t) : tree(t), factor(1.0f), sampleRate(44100), index(0)
    {
        trigger = false;
        samplesIntoPhase = 0;
        currentPhase = noteOff;
        output = 0.0f;
        delayId = "delayParam";
        attackId = "attackParam";
        holdId = "holdParam";
        decayId = "decayParam";
        sustainId = "sustainParam";
        releaseId = "releaseParam";
    }
    ~DAHDSR() {}
    static envPhase nextPhase(envPhase input)
    {
        if(input != noteOff)
            return (envPhase)(input + 1);
        else
            return noteOff;
    }
    void triggerOn()
    {
        trigger = true;
        enterPhase(delayPhase);
    }
    float valueOf(juce::String& str)
    {
        return *tree->getRawParameterValue(str);
    }
    std::atomic<float>* ptrValue(juce::String str)
    {
        return tree->getRawParameterValue(str);
    }
    void updateParams()
    {
        delayTime = valueOf(delayId);
        attackTime = valueOf(attackId);
        holdTime = valueOf(holdId);
        decayTime = valueOf(decayId);
        sustainLevel = valueOf(sustainId);
        releaseTime = valueOf(releaseId);
    }
    void triggerOff()
    {
        trigger = false;
        enterPhase(releasePhase);
    }
    void updatePhase()
    {
        if(samplesIntoPhase > samplesInPhase || samplesInPhase < 1)
        {
            enterPhase(nextPhase(currentPhase));
        }
    }
    void enterPhase(envPhase newPhase);
    float factorFor(float startLevel, float endLevel, float lengthMs);
    void setSampleRate(double value) {sampleRate = value;}
    float process(float input);
    envPhase getPhase() {return currentPhase;}
    bool isActive()
    {
        if(currentPhase == envPhase::noteOff)
            return false;
        return true;
    }
    float output;
private:
    //data
    static unsigned long long phaseSafe(unsigned long long input) //to avoid divide-by-zero errors
    {
        if(input > 0)
            return input;
        return 1;
    }
    envPhase currentPhase;
    unsigned long long samplesIntoPhase;
    unsigned long long samplesInPhase;
    float factor;
    float minLevel = 0.00001f;
    double sampleRate;
    int index;
    bool trigger;
    juce::String delayId;
    juce::String attackId;
    juce::String holdId;
    juce::String decayId;
    juce::String sustainId;
    juce::String releaseId;
private:
    float delayTime = DELAY_DEFAULT;
    float attackTime = ATTACK_DEFAULT;
    float holdTime = HOLD_DEFAULT;
    float decayTime = DECAY_DEFAULT;
    float sustainLevel = SUSTAIN_DEFAULT;
    float releaseTime = RELEASE_DEFAULT;
    
    float startLevel;
    float endLevel;
};
