/*
  ==============================================================================

    WavetableProcessor.cpp
    Created: 1 Mar 2021 12:50:20pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "WavetableProcessor.h"

void fft(int N, double *ar, double *ai)
/*
 in-place complex fft
 
 After Cooley, Lewis, and Welch; from Rabiner & Gold (1975)
 
 program adapted from FORTRAN
 by K. Steiglitz  (ken@princeton.edu)
 Computer Science Dept.
 Princeton University 08544          */
{
    int i, j, k, L;            /* indexes */
    int M, TEMP, LE, LE1, ip;  /* M = log N */
    int NV2, NM1;
    double t;               /* temp */
    double Ur, Ui, Wr, Wi, Tr, Ti;
    double Ur_old;
    // if ((N > 1) && !(N & (N - 1)))   // make sure we have a power of 2
    NV2 = N >> 1;
    NM1 = N - 1;
    TEMP = N; /* get M = log N */
    M = 0;
    while (TEMP >>= 1)
        ++M;
    /* shuffle */
    j = 1;
    for (i = 1; i <= NM1; i++)
    {
        if(i<j)
        {             /* swap a[i] and a[j] */
            t = ar[j-1];
            ar[j-1] = ar[i-1];
            ar[i-1] = t;
            t = ai[j-1];
            ai[j-1] = ai[i-1];
            ai[i-1] = t;
        }
        k = NV2;             /* bit-reversed counter */
        while(k < j)
        {
            j -= k;
            k /= 2;
        }
        j += k;
    }
    LE = 1.0f;
    for (L = 1; L <= M; L++) {            // stage L
        LE1 = LE;                         // (LE1 = LE/2)
        LE *= 2;                          // (LE = 2^L)
        Ur = 1.0f;
        Ui = 0.0f;
        Wr = cos(M_PI/(float)LE1);
        Wi = -sin(M_PI/(float)LE1); // Cooley, Lewis, and Welch have "+" here //Nigel Redmon used -sin, I switched for a bit better sound
        for (j = 1; j <= LE1; j++)
        {
            for (i = j; i <= N; i += LE)
            { // butterfly
                ip = i+LE1;
                Tr = ar[ip-1] * Ur - ai[ip-1] * Ui;
                Ti = ar[ip-1] * Ui + ai[ip-1] * Ur;
                ar[ip-1] = ar[i-1] - Tr;
                ai[ip-1] = ai[i-1] - Ti;
                ar[i-1]  = ar[i-1] + Tr;
                ai[i-1]  = ai[i-1] + Ti;
            }
            Ur_old = Ur;
            Ur = Ur_old * Wr - Ui * Wi;
            Ui = Ur_old * Wi + Ui * Wr;
        }
    }
}

WavetableFrame::WavetableFrame(std::vector<float> t) : tablesAdded(0), data(t), position(0.0f), sampleRate(44100.0f)
{
    auto inc = float(data.size() * (1.0f / TABLESIZE));
    float bottomSample, topSample, difference, skew, sample;
    double* freqWaveReal = new double[TABLESIZE];
    double* freqWaveImag = new double[TABLESIZE];
    for(int i = 0; i < TABLESIZE; ++i)
    {
        auto pos = inc * i;
        bottomSample = data[floor(pos)];
        topSample = data[ceil(pos)];
        difference = topSample - bottomSample;
        skew = pos - floor(pos);
        sample = bottomSample + (skew * difference);
        //setting up FFT arrays
        freqWaveImag[i] = (double)sample;
        freqWaveReal[i] = 0.0f;
    }
    //FFT time!!
    fft(TABLESIZE, freqWaveReal, freqWaveImag);
    createTables(freqWaveReal, freqWaveImag, TABLESIZE);
    delete [] freqWaveReal;
    delete [] freqWaveImag;
}

int WavetableFrame::createTables(double *waveReal, double *waveImag, int numSamples)
{
    int idx;
    // zero DC offset and Nyquist (set first and last samples of each array to zero, in other words)
    waveReal[0] = waveImag[0] = 0.0f;
    waveReal[numSamples >> 1] = waveImag[numSamples >> 1] = 0.0f;
    // determine maxHarmonic, the highest non-zero harmonic in the wave
    int maxHarmonic = numSamples >> 1;
    const double minVal = 0.000001f;
    while((fabs(waveReal[maxHarmonic]) + fabs(waveImag[maxHarmonic]) < minVal) && maxHarmonic)
        --maxHarmonic;
    double topFreq = (double)(2.0f / 3.0f / maxHarmonic); //note:: topFreq is in units of phase fraction per sample, not Hz
    double *ar = new double [numSamples];
    double *ai = new double [numSamples];
    double scale = 0.0f;
    int numTables = 0;
    while (maxHarmonic) // cut the harmonics in half until the max is <= 0
    {
        // fill the table in with the needed harmonics
        for (idx = 0; idx < numSamples; idx++)
            ar[idx] = ai[idx] = 0.0f;
        for (idx = 1; idx <= maxHarmonic; idx++)
        {
            ar[idx] = waveReal[idx];
            ai[idx] = waveImag[idx];
            ar[numSamples - idx] = waveReal[numSamples - idx];
            ai[numSamples - idx] = waveImag[numSamples - idx];
        }
        // make the wavetable
        scale = makeTable(ar, ai, numSamples, scale, topFreq);
        numTables++;

        // prepare for next table, topFreq *= 2 because we're using 1 waveTable per octave
        topFreq *= 2.0f;
        maxHarmonic >>= 1;
    }
    delete [] ar;
    delete [] ai;
    return numTables;
}
float WavetableFrame::makeTable(double *waveReal, double *waveImag, int numSamples, double scale, double topFreq)
{
    //printf("Table #%d limit: %lf\n", tablesAdded, topFreq * sampleRate);
    if(tablesAdded <= NUMTABLES)
    {
        tables.add(new WaveTable(numSamples, topFreq, waveImag));
        fft(numSamples, waveReal, waveImag);
        if (scale == 0.0f)
        {
            // get maximum value to scale to -1 - 1
            double max = 0.0f;
            for (int idx = 0; idx < numSamples; idx++)
            {
                double temp = fabs(waveImag[idx]);
                if (max < temp)
                    max = temp;
            }
            scale = 1.0f / max * 0.999f;
        }
        for(int i = 0; i < numSamples; ++i)
        {
            tables.getLast()->table[i] = waveImag[i] * scale;
        }
        //printf("Table #%d scale: %f\n", tablesAdded, scale);
        ++tablesAdded;
    }
    return (float)scale;
}

//note: this function looks like a mistake and I don't remember writing it or why it works but somehow it's the only version that does
WaveTable* WavetableFrame::tableForFreq(double frequency)
{
    auto* out = tables[0];
    int i;
    for(i = 0; i < tablesAdded; ++i)
    {
        if(tables[i]->maxFreq < frequency && (i + 1) < tablesAdded)
            out = tables[i + i];
    }
    /*
     I'm r-word so I don't know why getting tables[i + i] doesn't break it,
     but breakpoint confirms that this if never evaluates true so whatever
    */
    if(out == NULL)
        out = tables.getLast();
    return out;
}

float WavetableFrame::getSample(double frequency)
{
    posDelta = (double)(frequency / sampleRate);
    auto table = tableForFreq(posDelta);
    position += posDelta;
    if(position > 1.0f)
    {
        position -= 1.0f;
    }
    bottomSampleIndex = floor(table->length * position);
    skew = (table->length * position) - bottomSampleIndex;
    sampleDiff = table->table[bottomSampleIndex + 1] - table->table[bottomSampleIndex];
    output = table->table[bottomSampleIndex] + (skew * sampleDiff);
    return output;
}

std::vector<float> WavetableFrame::getBasicVector(int resolution)
{
    std::vector<float> out;
    auto inc = floor(TABLESIZE / resolution);
    for(int sample = 0; sample < resolution; ++sample)
    {
        auto f = (float)tables[0]->table[inc * sample]; //for purposes of graphing, always use the first table with the most harmonic detail
        out.push_back(f);
    }
    return out;
}

WavetableOsc::WavetableOsc(juce::File wavData)
{
    sampleRate = 44100.0f;
    position = 0.0f;
    numFrames = 0;
    juce::AudioFormatManager manager;
    manager.registerBasicFormats();
    auto reader = manager.createReaderFor(wavData);
    printf("Loading table set: %s\n", wavData.getFileName().toRawUTF8());
    auto numSamples = reader->lengthInSamples;
    int sNumFrames = floor(numSamples / TABLESIZE);
    frames.ensureStorageAllocated(sNumFrames);
    long currentSample = 0;
    printf("Parsing %d frames from %lld samples...\n", sNumFrames, numSamples);
    auto buffer = juce::AudioBuffer<float>(1, TABLESIZE);
    buffer.clear();
    reader->read(&buffer, 0, TABLESIZE, currentSample, true, true);
    std::vector<float> vec;
    for(int i = 0; i < sNumFrames; ++i)
    {
        for(int sample = 0; sample < TABLESIZE; ++sample)
        {
            vec.push_back(buffer.getSample(0, sample));
        }
        addFrame(vec);
        vec.clear();
        buffer.clear();
        currentSample += TABLESIZE;
        printf("Loaded frame %d from sample %ld\n", i, currentSample);
        reader->read(&buffer, 0, TABLESIZE, currentSample, true, true);
    }
    delete reader;
}

std::vector<std::vector<float>> WavetableOsc::getDataToGraph(int resolution)
{
    std::vector<std::vector<float>> out;
    for(int frame = 0; frame < numFrames; ++frame)
    {
        out.push_back(frames[frame]->getBasicVector(resolution));
    }
    return out;
}
