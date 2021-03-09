/*
  ==============================================================================

    WavetableProcessor.h
    Created: 1 Mar 2021 12:50:20pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WavetableData.h"
const int TABLESIZE = 1024;
const int NUMTABLES = 32;
const int fftOrder = 10;
struct WaveTable
{
    WaveTable(int size, double freq, double* input) : maxFreq(freq), length(size)
    {
        double accum = 0.0f;
        for(int i = 0; i < length; ++i)
        {
            table[i] = input[i];
            if(i < length / 2)
                accum += table[i];
        }
        //the average sample value for the first half of the wave
        if(accum / (double)(length / 2.0f) > 0.0f) //if the wave starts on a negative phase, invert it so they always match
        {
            for(auto i : table)
                i *= -1.0f;
        }
    }
    double maxFreq; //max frequency this table can use before aliasing
    int length; //number of samples
    std::array<double, TABLESIZE> table;
};

class WavetableFrame
{
public:
    WavetableFrame(std::vector<float> t);
    ~WavetableFrame() {}
    void setSampleRate(double rate)
    {
        sampleRate = rate;
    }
    int createTables(double* waveReal, double* waveImag, int numSamples);
    float makeTable(double* waveReal, double* waveImag, int numSamples, double scale, double topFreq);
    float getSample(double frequency);
    WaveTable* tableForFreq(double frequency);
private:
    int tablesAdded;
    std::vector<float> data; //the initial input data from which all the tables are made
    juce::OwnedArray<WaveTable> tables; //array of tables
    float position;
    float posDelta;
    float output;
    double sampleRate;
    int bottomSampleIndex;
    float sampleDiff;
    float skew;
};

class WavetableOsc
{
public:
    WavetableOsc(std::vector<float> firstFrameData = saw512)
    {
        sampleRate = 44100.0f;
        position = 0.0f;
        numFrames = 0;
        addFrame(firstFrameData);
    }
    ~WavetableOsc() {}
    void addFrame(std::vector<float> d)
    {
        frames.add(new WavetableFrame(d));
        ++numFrames;
    }
    void setSampleRate(double newRate)
    {
        sampleRate = newRate;
        for(auto& i : frames)
            i->setSampleRate(sampleRate);
    }
    void setPosition(float p) {position = p;} //note:: position must always be between 0 and 1
    float getSample(double freq)
    {
        skew = std::fmod((position * numFrames), 1.0f);
        if(numFrames < 2)
            return frames[0]->getSample(freq);
        else
        {
            lowerIndex = floor(position * (float)(numFrames - 1));
            upperIndex = ceil(position * (float)(numFrames - 1));
            bSample = frames[lowerIndex]->getSample(freq);
            if(lowerIndex == upperIndex)
                return bSample;
            tSample = frames[upperIndex]->getSample(freq);
            return bSample + ((tSample - bSample) * skew);
        }
    }
private:
    int lowerIndex;
    int upperIndex;
    float position;
    int numFrames;
    float skew;
    float offset;
    float tSample;
    float bSample;
    double sampleRate;
    juce::OwnedArray<WavetableFrame, juce::CriticalSection> frames;
};
