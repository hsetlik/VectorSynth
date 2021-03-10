/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class WavetableSynthesizerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    WavetableSynthesizerAudioProcessorEditor (WavetableSynthesizerAudioProcessor&);
    ~WavetableSynthesizerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
private:
    
    WavetableSynthesizerAudioProcessor& audioProcessor;
    DummyContainer container;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableSynthesizerAudioProcessorEditor)
};
