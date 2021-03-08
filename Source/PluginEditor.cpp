/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
std::vector<std::vector<float>> makeDummyVector(int size)
{
    std::vector<std::vector<float>> output;
    for(int i = 0; i < 6; ++i)
    {
        std::vector<float> v(size, 0.0f);
        output.push_back(v);
    }
    return output;
}

//==============================================================================
WavetableSynthesizerAudioProcessorEditor::WavetableSynthesizerAudioProcessorEditor (WavetableSynthesizerAudioProcessor& p)
    : AudioProcessorEditor (&p), container(&p.tree), audioProcessor (p)
{
    this->setResizable(true, true);
    addAndMakeVisible(&container);
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
    
    container.setBounds(0, 0, getWidth(), getHeight());
}
