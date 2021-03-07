/*
  ==============================================================================

    SourcePanel.h
    Created: 7 Mar 2021 10:04:34am
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DAHDSRGraph.h"

class SoundSourcePanel : public juce::Component
{
public:
    SoundSourcePanel(juce::AudioProcessorValueTreeState* t) : pTree(t)
    {
        
    }
    ~SoundSourcePanel() {}
private:
    juce::AudioProcessorValueTreeState* pTree;
};
