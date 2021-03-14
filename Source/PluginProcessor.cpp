/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
juce::AudioProcessorValueTreeState::ParameterLayout makeLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    juce::NormalisableRange<float> freqRange(150.0f, 20000.0f, 20.0f, 0.5f);
    freqRange.setSkewForCentre(1500.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>("frequency", "Frequency", freqRange, 150.0f));
    juce::NormalisableRange<float> posRange(0.0f, 1.0f);
    layout.add(std::make_unique<juce::AudioParameterFloat>("wavetablePos", "wavetable position", posRange, 0.0f));
    return layout;
}

//==============================================================================
WavetableSynthesizerAudioProcessor::WavetableSynthesizerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), tree(*this, nullptr, "AllParameters", makeLayout()), osc(handler.getWav(1))
#endif
{
    osc.waveNames = handler.tableNames;
    osc.waveFiles = handler.wavFiles;
}
WavetableSynthesizerAudioProcessor::~WavetableSynthesizerAudioProcessor()
{
}

//==============================================================================
const juce::String WavetableSynthesizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WavetableSynthesizerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WavetableSynthesizerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WavetableSynthesizerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WavetableSynthesizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WavetableSynthesizerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WavetableSynthesizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WavetableSynthesizerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WavetableSynthesizerAudioProcessor::getProgramName (int index)
{
    return {};
}

void WavetableSynthesizerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WavetableSynthesizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    osc.setSampleRate(sampleRate);
}

void WavetableSynthesizerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WavetableSynthesizerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void WavetableSynthesizerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    frequency = (double)*tree.getRawParameterValue("frequency");
    position = *tree.getRawParameterValue("wavetablePos");
    osc.setPosition(position);
        for(int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            lastSample = osc.getSample(frequency);
            for(int channel = 0; channel < 2; ++channel)
            {
                buffer.addSample(channel, sample, lastSample * 0.25f);
            }
        }
}

//==============================================================================
bool WavetableSynthesizerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WavetableSynthesizerAudioProcessor::createEditor()
{
    return new WavetableSynthesizerAudioProcessorEditor (*this);
}

//==============================================================================
void WavetableSynthesizerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void WavetableSynthesizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WavetableSynthesizerAudioProcessor();
}
