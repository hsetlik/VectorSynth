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
    using pAttach = std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>;
    using sAttach = juce::AudioProcessorValueTreeState::SliderAttachment;
    DAHDSRPanel(juce::DragAndDropContainer* c, juce::AudioProcessorValueTreeState* tree);
    void resized() override;
    void paint(juce::Graphics& g) override;
private:
    EnvelopeDial sDelay;
    pAttach aDelay;
    EnvelopeDial sAttack;
    pAttach aAttack;
    EnvelopeDial sHold;
    pAttach aHold;
    EnvelopeDial sDecay;
    pAttach aDecay;
    EnvelopeDial sSustain;
    pAttach aSustain;
    EnvelopeDial sRelease;
    pAttach aRelease;
    EnvelopeLabel lDelay;
    EnvelopeLabel lAttack;
    EnvelopeLabel lHold;
    EnvelopeLabel lDecay;
    EnvelopeLabel lSustain;
    EnvelopeLabel lRelease;
    DAHDSRGraph graph;
    ModSourceComponent envModSource;
};
