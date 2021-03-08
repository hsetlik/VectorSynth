/*
  ==============================================================================

    EnvelopeComponent.h
    Created: 7 Mar 2021 8:12:47pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "SliderTypes.h"
#include "DAHDSRGraph.h"

class DAHDSRPanel : public juce::Component
{
public:
    DAHDSRPanel(juce::DragAndDropContainer* c);
    void resized() override;
private:
    EnvelopeDial sDelay;
    EnvelopeDial sAttack;
    EnvelopeDial sHold;
    EnvelopeDial sDecay;
    EnvelopeDial sSustain;
    EnvelopeDial sRelease;
    EnvelopeLabel lDelay;
    EnvelopeLabel lAttack;
    EnvelopeLabel lHold;
    EnvelopeLabel lDecay;
    EnvelopeLabel lSustain;
    EnvelopeLabel lRelease;
    DAHDSRGraph graph;
    ModSourceComponent envModSource;
};
