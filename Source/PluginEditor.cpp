/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
WavetableSynthesizerAudioProcessorEditor::WavetableSynthesizerAudioProcessorEditor (WavetableSynthesizerAudioProcessor& p)
    : AudioProcessorEditor (&p), dial(AttackSlider, 1), audioProcessor (p)
{
    this->setResizable(true, true);
    addAndMakeVisible(&freqSlider);
    addAndMakeVisible(&dial);
    freqSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    freqSlider.setRange(20.0f, 20000.0f);
    freqAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.tree, "frequency", freqSlider));
    freqSlider.setValue(100.0f);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
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
    auto n = getWidth() / 12;
    dial.setBounds(7 * n, 2 * n, 3 * n, 4 * n);
    freqSlider.setBounds(n * 2, n * 2, n * 5, n * 2);
}
