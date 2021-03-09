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
    
    juce::Slider freqSlider;
    juce::Slider posSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> posAttach;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WavetableSynthesizerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableSynthesizerAudioProcessorEditor)
};
