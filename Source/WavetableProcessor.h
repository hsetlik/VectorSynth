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
        double accum = 0.0f;
        for(int i = 0; i < length; ++i)
        {
            table[i] = input[i];
            if(i < length / 2)
                accum += table[i];
        }
        //the average sample value for the first half of the wave
        if(accum / (double)(length / 2.0f) < 0.0f) //if the wave starts on a negative phase, invert it so they always match
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
    WavetableOsc(std::vector<float> firstFrameData)
    {
        sampleRate = 44100.0f;
        position = 0.0f;
        numFrames = 0;
        addFrame(firstFrameData);
    }
    WavetableOsc(juce::File wavData);
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
    void setPosition(float p) {position = p;}
    float getSample(double freq)
    {
        if(numFrames < 2)
            output = frames[0]->getSample(freq);
        else
        {
            pFrame = position * (numFrames - 1);
            skew = pFrame - floor(pFrame);
            lowerIndex = floor(pFrame);
            upperIndex = (lowerIndex == (numFrames - 1)) ? 0 : lowerIndex + 1;
            bSample = frames[lowerIndex]->getSample(freq);
            tSample = frames[upperIndex]->getSample(freq);
            output = bSample + ((tSample - bSample) * skew);
        }
        return output;
    }
    std::vector<std::vector<float>> getDataToGraph(int resolution);
    juce::StringArray waveNames;
private:
    int lowerIndex;
    int upperIndex;
    float position;
    float pFrame;
    int numFrames;
    float skew;
    float tSample;
    float bSample;
    float output;
    double sampleRate;
    juce::OwnedArray<WavetableFrame, juce::CriticalSection> frames;
};

class WavetableOscHolder
{
public:
    WavetableOscHolder(juce::File file) :
    osc(std::make_unique<WavetableOsc>(file))
    {
        
    }
    void replaceFromFile(juce::File file)
    {
        osc.reset(new WavetableOsc(file));
    }
    void setPosition(float pos) {osc->setPosition(pos);}
    void setSampleRate(double rate) {osc->setSampleRate(rate);}
    float getSample(double freq)
    {
        return osc->getSample(freq);
    }
    std::vector<std::vector<float>> getDataToGraph(int resolution) {return osc->getDataToGraph(resolution);}
    juce::StringArray waveNames;
private:
    std::unique_ptr<WavetableOsc> osc;
};

