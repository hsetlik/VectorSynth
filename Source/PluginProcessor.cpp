/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
juce::AudioProcessorValueTreeState::ParameterLayout makeLayout(int idx)
{
    auto iStr = juce::String(idx);
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    auto posRange = juce::NormalisableRange<float>(WTPOS_MIN, WTPOS_MAX, 0.00001f);
    auto delayRange = juce::NormalisableRange<float>(DELAY_MIN, DELAY_MAX, 0.1f);
    delayRange.setSkewForCentre(DELAY_CENTER);
    auto attackRange = juce::NormalisableRange<float>(ATTACK_MIN, ATTACK_MAX, 0.1f);
    attackRange.setSkewForCentre(ATTACK_CENTER);
    auto holdRange = juce::NormalisableRange<float>(HOLD_MIN, HOLD_MAX, 0.1f);
    holdRange.setSkewForCentre(HOLD_CENTER);
    auto decayRange = juce::NormalisableRange<float>(DECAY_MIN, DECAY_MAX, 0.1f);
    decayRange.setSkewForCentre(DECAY_CENTER);
    auto sustainRange = juce::NormalisableRange<float>(SUSTAIN_MIN, SUSTAIN_MAX, 0.001f);
    auto releaseRange = juce::NormalisableRange<float>(RELEASE_MIN, RELEASE_MAX, 0.1f);
    releaseRange.setSkewForCentre(RELEASE_CENTER);
    
    auto numWaveFiles = (float)AudioWavetableHandler::getNumWavetables();
    auto waveChoiceId = "waveChoiceParam" + iStr;
    auto waveChoiceName = "Wavetabe on: " + iStr;
    
    auto waveChoiceRange = juce::NormalisableRange<float>(0.0f, numWaveFiles, 1.0f);
    
    auto posId = "oscPositionParam" + iStr;
    auto posName = "Wavetable Position " + iStr;
    
    auto delayId = "delayParam" + iStr;
    auto delayName = "Delay " + iStr;
    auto attackId = "attackParam" + iStr;
    auto attackName = "Attack " + iStr;
    auto holdId = "holdParam" + iStr;
    auto holdName = "Hold " + iStr;
    auto decayId = "decayParam" + iStr;
    auto decayName = "Decay" + iStr;
    auto sustainId = "sustainParam" + iStr;
    auto sustainName = "Sustain " + iStr;
    auto releaseId = "releaseParam" + iStr;
    auto releaseName = "Release " + iStr;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(posId, posName, posRange, WTPOS_DEFAULT));
    layout.add(std::make_unique<juce::AudioParameterFloat>(delayId, delayName, delayRange, DELAY_DEFAULT));
    layout.add(std::make_unique<juce::AudioParameterFloat>(attackId, attackName, attackRange, ATTACK_DEFAULT));
    layout.add(std::make_unique<juce::AudioParameterFloat>(holdId, holdName, holdRange, HOLD_DEFAULT));
    layout.add(std::make_unique<juce::AudioParameterFloat>(decayId, decayName, decayRange, DECAY_DEFAULT));
    layout.add(std::make_unique<juce::AudioParameterFloat>(sustainId, sustainName, sustainRange, SUSTAIN_DEFAULT));
    layout.add(std::make_unique<juce::AudioParameterFloat>(releaseId, releaseName, releaseRange, RELEASE_DEFAULT));
    layout.add(std::make_unique<juce::AudioParameterFloat>(waveChoiceId, waveChoiceName, waveChoiceRange, 0.0f));
    
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
                       ), tree(*this, nullptr, "AllParameters", makeLayout(0)),
synth(handler.wavFiles)
#endif
{
    synth.setWaveNames(handler.tableNames);
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
    synth.setCurrentPlaybackSampleRate(sampleRate);
    synth.setSampleRateRecursive(sampleRate);
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
    synth.updateParameters(&tree);
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
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
