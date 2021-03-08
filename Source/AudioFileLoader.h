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
    ~AudioWavetableHandler() {delete reader;}
    void oscFromFile(WavetableOsc* osc, juce::String); //switches out the table of an existing oscillator in place w data from a wav file
    juce::String nameAtIndex(int index)
    {
        return tableNames[index];
    }
private:
    int numFiles;
    juce::AudioFormatReader* reader;
    juce::File audioFolder;
    juce::StringArray tableNames;
    juce::Array<juce::File> wavFiles;
    juce::AudioFormatManager manager;
};
