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

//TODO: this class should take in a vector of wave data and create all the appropriate band-limited wavetables needed for 20-20k Hz
class WToscillator
{
public:
    WToscillator(std::vector<float> t);
    ~WToscillator() {}
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

