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
const int TABLESIZE = 2048;
const int NUMTABLES = 32;
const int fftOrder = 10;
struct WaveTable
{
    WaveTable(int size, double freq, double* input) : maxFreq(freq), length(size)
    {
        table = new double[length];
        for(int i = 0; i < length; ++i)
        {
            table[i] = input[i];
        }
    }
    double maxFreq; //max frequency this table can use before aliasing
    int length; //number of samples
    double* table; //c-array of samples
};

class WTframe
{
public:
    WTframe(std::vector<float> t);
    ~WTframe() {}
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
    WavetableOsc(std::vector<float> inData);
    ~WavetableOsc() {}
    void addFrame(std::vector<float> inData);
    void addFrame(float* input, int size);
    float getSample(double freq);
    void setPosition(float index) //works such that the position can be set either by the exact index or by a proportion of the total length
    {
        if(index <= 1.0f)
            framePos = numFrames * index;
        else
            framePos = (double) index;
        if(!frameInterp)
            framePos = floor(framePos);
    }
    const int MAX_FRAMES = 256;
private:
    bool frameInterp; //determines whether the oscillator should interpolate between frames or only take values directly from one frame
    int numFrames;
    double framePos;
    float bottomSample;
    float topSample;
    float skew;
    juce::OwnedArray<WTframe> frames;
};

