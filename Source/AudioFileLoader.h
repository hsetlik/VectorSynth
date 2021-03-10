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
    ~AudioWavetableHandler() {}
    juce::String nameAtIndex(int index)
    {
        return tableNames[index];
    }
    juce::File getWav(int index)
    {
        return wavFiles[index];
    }
    juce::File getWav(juce::String name)
    {
        return wavFiles[tableNames.indexOf(name)];
    }
    juce::StringArray tableNames;
private:
    int numFiles;
    juce::File audioFolder;
    juce::Array<juce::File> wavFiles;
};
