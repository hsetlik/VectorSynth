/*
  ==============================================================================

    AudioFileLoader.cpp
    Created: 7 Mar 2021 9:52:48pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "AudioFileLoader.h"


AudioWavetableHandler::AudioWavetableHandler()
{
    manager.registerBasicFormats();
    auto appFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    appFolder.setAsCurrentWorkingDirectory();
    audioFolder = appFolder.getChildFile("MyWavetables");
    if(audioFolder.exists() && audioFolder.isDirectory())
    {
        printf("wave folder exists\n");
        audioFolder.setAsCurrentWorkingDirectory();
    }
    else
    {
        audioFolder.createDirectory();
        audioFolder.setAsCurrentWorkingDirectory();
        printf("wave folder created\n");
    }
    printf("Folder is at: %s\n", audioFolder.getFullPathName().toRawUTF8());
    auto files = audioFolder.findChildFiles(juce::File::findFiles, true);
    numFiles = 0;
    if(files.size() > 0)
    {
        for(auto i : files)
        {
            auto name = i.getFileNameWithoutExtension();
            auto extension = i.getFileExtension();
            if(extension == ".wav" || extension == ".WAV")
            {
                wavFiles.add(i);
                tableNames.add(name);
                printf(".wav found: %s\n", i.getFileName().toRawUTF8());
            }
        }
    }
}

void AudioWavetableHandler::oscFromFile(WavetableOsc* osc, juce::String fileName)
{
    auto idx = tableNames.indexOf(fileName);
    auto file = wavFiles[idx]; //grip the appropriate wav file
    reader = manager.createReaderFor(file);
    printf("Loading table set: %s\n", fileName.toRawUTF8());
    auto numSamples = reader->lengthInSamples;
    auto numFrames = floor(numSamples / TABLESIZE);
    long currentSample = 0;
    auto buffer = juce::AudioBuffer<float>(1, TABLESIZE);
    buffer.clear();
    reader->read(&buffer, 0, TABLESIZE, currentSample, true, true);
    std::vector<float> vec;
    for(int i = 0; i < numFrames; ++i)
    {
        for(int sample = 0; sample < TABLESIZE; ++sample)
        {
            vec.push_back(buffer.getSample(0, sample));
        }
        if(i < osc->numFrames)
        {
            osc->frames.set(i, new WTframe(vec), true);
        }
        else
        {
            osc->addFrame(vec);
        }
        vec.clear();
        buffer.clear();
        currentSample += TABLESIZE;
        printf("Loaded Table %d from sample %ld\n", i, currentSample);
        reader->read(&buffer, 0, TABLESIZE, currentSample, true, true);
    }
    delete reader;
}
