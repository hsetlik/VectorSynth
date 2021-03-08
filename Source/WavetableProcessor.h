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
    std::vector<float> getBasicVector(int resolution);
private:
    int tablesAdded;
    std::vector<float> data; //the initial input data from which all the tables are made
    juce::OwnedArray<WaveTable, juce::CriticalSection> tables; //array of tables
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
    WavetableOsc(); //default constructor just creates one frame with the saw512 table
    WavetableOsc(std::vector<float> inData);
    ~WavetableOsc() {}
    void setSampleRate(double newRate)
    {
        for(auto i : frames)
            i->setSampleRate(newRate);
    }
    void addFrame(std::vector<float> inData);
    void addFrame(float* input, int size);
    float getSample(double freq);
    void setPosition(float index)
    {
        framePos = (double) index;
        if(!frameInterp)
            framePos = floor(index);
    }
    std::vector<std::vector<float>> getFrameVectors(int resolution);
    const int MAX_FRAMES = 256;
    juce::OwnedArray<WTframe, juce::CriticalSection> frames;
    int numFrames;
private:
    bool frameInterp; //determines whether the oscillator should interpolate between frames or only take values directly from one frame
    double framePos;
    float bottomSample;
    float topSample;
    float skew;
};

