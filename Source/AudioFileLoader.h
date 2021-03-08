/*
  ==============================================================================

    AudioFileLoader.h
    Created: 7 Mar 2021 9:52:48pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WavetableProcessor.h"
class AudioWavetableHandler
{
public:
    AudioWavetableHandler();
    void oscFromFile(WavetableOsc* osc, juce::String); //switches out the table of an existing oscillator in place w data from a wav file
private:
    int numFiles;
    juce::File audioFolder;
    juce::StringArray tableNames;
    juce::Array<juce::File> wavFiles;
    juce::AudioFormatManager manager;
};
