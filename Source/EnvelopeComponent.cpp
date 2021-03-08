/*
  ==============================================================================

    EnvelopeComponent.cpp
    Created: 7 Mar 2021 8:12:47pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "EnvelopeComponent.h"

DAHDSRPanel::DAHDSRPanel(juce::DragAndDropContainer* c) :
sDelay(c, DelaySlider, 0),
sAttack(c, AttackSlider, 0),
sHold(c, HoldSlider, 0),
sDecay(c, DecaySlider, 0),
sSustain(c, SustainSlider, 0),
sRelease(c, ReleaseSlider, 5),
lDelay(&sDelay.mTarget, "ms"),
lAttack(&sAttack.mTarget, "ms"),
lHold(&sHold.mTarget, "ms"),
lDecay(&sDecay.mTarget, "ms"),
lSustain(&sSustain.mTarget, ""),
lRelease(&sRelease.mTarget, "ms"),
graph(&sDelay.mTarget, &sAttack.mTarget, &sHold.mTarget, &sDecay.mTarget, &sSustain.mTarget, &sRelease.mTarget),
envModSource(c, "dummySource")
{
    addAndMakeVisible(&sDelay);
    addAndMakeVisible(&sAttack);
    addAndMakeVisible(&sHold);
    addAndMakeVisible(&sDecay);
    addAndMakeVisible(&sSustain);
    addAndMakeVisible(&sRelease);
    
    addAndMakeVisible(&lDelay);
    addAndMakeVisible(&lAttack);
    addAndMakeVisible(&lHold);
    addAndMakeVisible(&lDecay);
    addAndMakeVisible(&lSustain);
    addAndMakeVisible(&lRelease);
    
    addAndMakeVisible(&graph);
    addAndMakeVisible(&envModSource);
}

void DAHDSRPanel::resized()
{
    auto n = getWidth() / 12; //note: this component should always have a 4:3 aspect ratio, therefore width = 9 * n
    graph.setBounds(0, 0, 7 * n, 5 * n);
    sDelay.setBounds(0, 5 * n, 2 * n, 2 * n);
    sAttack.setBounds(2 * n, 5 * n, 2 * n, 2 * n);
    sHold.setBounds(4 * n, 5 * n, 2 * n, 2 * n);
    sDecay.setBounds(6 * n, 5 * n, 2 * n, 2 * n);
    sSustain.setBounds(8 * n, 5 * n, 2 * n, 2 * n);
    sRelease.setBounds(10 * n, 5 * n, 2 * n, 2 * n);
    
    lDelay.setBounds(0, 7 * n, 2 * n, n);
    lAttack.setBounds(2 * n, 7 * n, 2 * n, n);
    lHold.setBounds(4 * n, 7 * n, 2 * n, n);
    lDecay.setBounds(6 * n, 7 * n, 2 * n, n);
    lSustain.setBounds(8 * n, 7 * n, 2 * n, n);
    lRelease.setBounds(10 * n, 7 * n, 2 * n, n);
    
    envModSource.setBounds(8 * n, n, 3 * n, 3 * n);
}
