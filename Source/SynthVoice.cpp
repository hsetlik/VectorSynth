/*
  ==============================================================================

    SynthVoice.cpp
    Created: 7 Mar 2021 10:06:17am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "SynthVoice.h"
WavetableVoice::WavetableVoice(juce::AudioProcessorValueTreeState* t, juce::File& defaultWave) :
    osc(defaultWave),
    env(t),
    tree(t)
{
    
}
