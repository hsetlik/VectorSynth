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
    WaveTable(int size, double fMin, double fMax, double* input) : minFreq(fMin), maxFreq(fMax), length(size), table(new float [size])
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
            for(int i = 0; i < size; ++i)
                table[i] *= -1.0f;
        }
    }
    //construct empty table of zeroes
    WaveTable(int size=TABLESIZE, double fMin=0.0f, double fMax=0.0f) : minFreq(fMin), maxFreq(fMax), length(size), table(new float [size])
    {
        for(int i = 0; i < length; ++i)
        {
            table[i] = 0.0f;
        }
    }
    ~WaveTable()
    {
        delete [] table;
    }
    float& operator [] (int idx)
    {
        return table[idx];
    }
    void normalize(float amp=1.0f)
    {
        auto maxLevel = std::numeric_limits<float>::min();
        auto minLevel = std::numeric_limits<float>::max();
        for(int i = 0; i < length; ++i)
        {
            if(table[i] < minLevel)
                minLevel = table[i];
            if(table[i] > maxLevel)
                maxLevel = table[i];
        }
        for(int i = 0; i < length; ++i)
            table[i] /= (fabs(maxLevel) + fabs(minLevel)) / 2.0f;
    }
    void makeDerivative()
    {
        dTable = new float[length];
        auto dX = juce::MathConstants<double>::twoPi / length;
        float aSample, bSample, rise;
        for(int i = 0; i < length; ++i)
        {
            aSample = table[i];
            bSample = (i < length - 1) ? table[i + 1] : table[0];
            rise = bSample - aSample;
            dTable[i] = rise / dX;
        }
    }
    double minFreq;
    double maxFreq; //max frequency this table can use before aliasing
    int length; //number of samples
    float* table;
    float* dTable;
};

class WavetableFrame
{
public:
    WavetableFrame(std::vector<float> t=std::vector<float>(TABLESIZE, 0.0f));
    WavetableFrame(std::array<float, TABLESIZE> t);
    WavetableFrame(float* t, int length);
    ~WavetableFrame(){}
    void setSampleRate(double rate)
    {
        sampleRate = rate;
    }
    int createTables(double* waveReal, double* waveImag, int numSamples);
    float makeTable(double* waveReal, double* waveImag, int numSamples, double scale, double bottomFreq, double topFreq);
    float getSample() {return output;}
    WaveTable* tableForFreq(double frequency);
    std::vector<float> getBasicVector(int resolution);
    void clockSample(double frequency);
private:
    WaveTable* pTables;
    WaveTable* rawTable;
    double lastMinFreq;
    int tablesAdded;
    std::vector<float> data; //the initial input data from which all the tables are made
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
        currentPosition = 0.0f;
        numFrames = 0;
    }
    WavetableOsc(juce::File wavData);
    ~WavetableOsc() {}
    void replaceTables(juce::String nTables);
    void clockFrames(double frequency)
    {
        for(frameIndex = 0; frameIndex < numFrames; ++frameIndex)
        {
            frames[frameIndex].clockSample(frequency);
        }
    }
    void setSampleRate(double newRate)
    {
        sampleRate = newRate;
        for(int i = 0; i < numFrames; ++i)
        {
            frames[i].setSampleRate(sampleRate);
        }
    }
    void setPosition(float p)
    {
        targetPosition = p;
        maxSamplePosDelta = 3.0f / sampleRate;
        if(currentPosition > targetPosition)
            posDeltaSign = -1.0f;
        else
            posDeltaSign = 1.0f;
    }
    static float clamp(float input, float amplitude)
    {
        if(input < (-1.0f * amplitude))
            return -1.0f * amplitude;
        if(input > amplitude)
            return amplitude;
        return input;
    }
    float getSample(double freq)
    {
        clockFrames(freq);
        if(numFrames < 2)
            output = frames[0].getSample();
        else
        {
            if(fabs(targetPosition - currentPosition) > maxSamplePosDelta)
                currentPosition = currentPosition + (maxSamplePosDelta * posDeltaSign);
            else
                currentPosition = targetPosition;
            pFrame = currentPosition * (numFrames - 1);
            lowerIndex = floor(pFrame);
            skew = pFrame - lowerIndex;
            upperIndex = (lowerIndex == (numFrames - 1)) ? 0 : lowerIndex + 1;
            bSample = frames[lowerIndex].getSample();
            tSample = frames[upperIndex].getSample();
            output = bSample + ((tSample - bSample) * skew);
        }
        return clamp(output, 1.0f);
    }
    std::vector<std::vector<float>> getDataToGraph(int resolution);
    juce::StringArray waveNames;
    float currentPosition;
private:
    int frameIndex;
    int lowerIndex;
    int upperIndex;
    float targetPosition;
    float maxSamplePosDelta; //maximum change to the position value per sample period
    float posDeltaSign;
    float pFrame;
    int numFrames;
    float skew;
    float tSample;
    float bSample;
    float output;
    double sampleRate;
    WavetableFrame* frames;
};

class WavetableOscHolder
{
public:
    WavetableOscHolder(juce::File file) :
    osc(std::make_unique<WavetableOsc>(file))
    {
        printf("Oscillator created\n");
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
    juce::Array<juce::File> waveFiles;
    float getPosition() {return osc->currentPosition;}
private:
    std::unique_ptr<WavetableOsc> osc;
};

