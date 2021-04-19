/*
  ==============================================================================

    SynthParameter.h
    Created: 18 Apr 2021 5:10:54pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ModSourceComponent.h"


//! Base class for any modulatable parameter
class SynthParam
{
public:
    SynthParam()
    {
    }
    void operator =(float newValue)
    {
        value = newValue;
    }
    void setRange(float _max, float _min)
    {
        max = _max;
        min = _min;
    }
    void setMax(float _max) {max = _max; }
    void setMin(float _min) {min = _min; }
    void set(float val)
    {
        value = val;
    }
    float get()
    {
        return value.load() + clockModulation();
    }
    float clockModulation();
    void addSource(ModSource* newSource)
    {
        modSources.push_back(newSource);
    }
    static float lerp(float lower, float upper, float r)
    {
        return lower + ((upper - lower) * r);
    }
private:
    std::atomic<float> max;
    std::atomic<float> min;
    float offset;
    float temp;
    float diff;
    std::atomic<float> value;
    std::vector<ModSource*> modSources;
};
