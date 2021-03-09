/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WavetableSynthesizerAudioProcessorEditor::WavetableSynthesizerAudioProcessorEditor (WavetableSynthesizerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), display(audioProcessor.osc.getDataToGraph(128), &posSlider)
{
    addAndMakeVisible(&freqSlider);
    freqSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    freqSlider.setRange(20.0f, 20000.0f);
    freqAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.tree, "frequency", freqSlider));
    freqSlider.setValue(100.0f);
    
    
    addAndMakeVisible(&posSlider);
    posSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    posSlider.setRange(0.0f, 1.0f);
    posAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.tree, "wavetablePos", posSlider));
    posSlider.setValue(0.2f);
    
    addAndMakeVisible(&display);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);
}

WavetableSynthesizerAudioProcessorEditor::~WavetableSynthesizerAudioProcessorEditor()
{
}

//==============================================================================
void WavetableSynthesizerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
}

void WavetableSynthesizerAudioProcessorEditor::resized()
{
    auto dX = getWidth() / 12;
    auto dY = getHeight() / 12;
    display.setBounds(0, 0, 12 * dX, 9 * dY);
    freqSlider.setBounds(dX, dY * 9, dX * 5, dY * 3);
    posSlider.setBounds(dX * 6, dY * 9, dX * 5, dY * 3);
}
