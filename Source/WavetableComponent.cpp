/*
  ==============================================================================

    WavetableComponent.cpp
    Created: 7 Mar 2021 9:51:53pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "WavetableComponent.h"
WavetableDisplay::WavetableDisplay(std::vector<std::vector<float>> data, juce::Slider* s, float pos) :
fake3d(true),
valueSet(data),
position(pos),
highlight(UXColor::highlight),
currentValues(128, 0.0f)
{
    sourceSlider = s;
    sourceSlider->addListener(this);
    background = UXColor::darkBkgnd;
    numTraces = 0;
    resolution = 128;
    for(auto d : valueSet)
    {
        ++numTraces;
        traces.add(new juce::Path());
    }
    colors.add(Color::shadesBetween(Color::RGBColor(199, 200, 202), UXColor::grayShadeD, numTraces));
}

void WavetableDisplay::setPosition(float pos)
{
    position = pos;
    workingColors = colors;
    auto lowerIdx = floor(position * (numTraces - 1) * 0.99f);
    auto upperIdx = lowerIdx + 1;
    auto& upperData = valueSet[upperIdx];
    auto& lowerData = valueSet[lowerIdx];
    auto skew = (position * (numTraces - 1) * 0.99f) - (float)lowerIdx;
    auto aExp = 0.65f;
    auto tColorA = highlight.withBrightness(highlight.getBrightness() / 8).withSaturation(highlight.getSaturation() / 6);
    auto tColorB = highlight.withBrightness(highlight.getBrightness() * 0.8f);
    for(int i = 0; i < numTraces; ++i)
    {
        auto diff = fabs(i - (position * (numTraces - 1)));
        auto alpha = 1.0f * pow(aExp, diff);
        auto col = Color::blendHSB(tColorA, tColorB, alpha);
        workingColors.set(i, col);
    }
    for(int i = 0; i < resolution; ++i)
    {
        currentValues[i] = lowerData[i] + ((upperData[i] - lowerData[i]) * skew);
    }
}

void WavetableDisplay::paint(juce::Graphics &g)
{
    int lowerIdx = floor(position * (numTraces - 1) * 0.99f);
    auto strokeType = juce::PathStrokeType(2.0f);
    g.setColour(background);
    auto fBounds = getBounds().toFloat();
    g.fillRect(fBounds.reduced(2.0f));
    auto y0 = fBounds.getHeight() / 2.0f;
    auto amplitude = y0 * 0.75f;
    auto dX = fBounds.getWidth() / resolution;
    if(numTraces >= 1)
    {
        if(fake3d)
        {
            for(int p = (numTraces - 1); p >= 0; --p)
            {
                traces[p]->clear();
                traces[p]->startNewSubPath(0.0f, fBounds.getBottom());
                traces[p]->lineTo(0.0f, y0);
                for(int i = 0; i < resolution; ++i)
                {
                    traces[p]->lineTo(dX * i, y0 + (valueSet[p][i] * amplitude));
                }
                traces[p]->lineTo(fBounds.getRight(), fBounds.getBottom());
                traces[p]->closeSubPath();
                alterFor3d(traces[p], (float)p);
                g.setColour(workingColors.atIndex(p));
                g.strokePath(*traces[p], strokeType);
                if(p == lowerIdx)
                {
                    juce::Path current;
                    current.startNewSubPath(0.0f, fBounds.getBottom());
                    current.lineTo(0.0f, y0);
                    for(int i = 0; i < resolution; ++i)
                    {
                        current.lineTo(i * dX, y0 + (currentValues[i] * amplitude));
                    }
                    current.lineTo(fBounds.getRight(), fBounds.getBottom());
                    current.closeSubPath();
                    alterFor3d(&current, position * (numTraces - 1));
                    g.setColour(highlight);
                    g.strokePath(current, strokeType);
                }
            }
        }
        else
        {
            juce::Path current;
            current.startNewSubPath(0.0f, fBounds.getBottom());
            current.lineTo(0.0f, y0);
            for(int i = 0; i < resolution; ++i)
            {
                current.lineTo(i * dX, y0 + (currentValues[i] * amplitude));
            }
            current.lineTo(fBounds.getRight(), fBounds.getBottom());
            current.closeSubPath();
            current.applyTransform(current.getTransformToScaleToFit(fBounds.reduced(20.0f), true));
            g.setColour(highlight);
            g.strokePath(current, strokeType);
        }
    }
}

void WavetableDisplay::sliderValueChanged(juce::Slider *s)
{
    setPosition((float)s->getValue());
    repaint();
}

WaveSelector::WaveSelector(WavetableSynth* o, juce::ComboBox::Listener* list) : lButton(true, this), rButton(false, this), tableNames(o->getWaveNames())
{
    addAndMakeVisible(&waveBox);
    addAndMakeVisible(&rButton);
    addAndMakeVisible(&lButton);
    waveBox.addItemList(tableNames, 1);
    waveBox.setSelectedItemIndex(1);
    waveBox.addListener(list);
}

SoundSourcePanel::SoundSourcePanel(juce::DragAndDropContainer* c, juce::AudioProcessorValueTreeState* t, WavetableSynth* o) :
sPos(c, 0),
envPanel(c, t),
synth(o),
pWaveDisplay(std::make_unique<WavetableDisplay>(o->getDataToGraph(), &sPos.mTarget)),
selector(o, this)
{
    addAndMakeVisible(sPos);
    addAndMakeVisible(envPanel);
    addAndMakeVisible(*pWaveDisplay);
    addAndMakeVisible(selector);
    
    posAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(*t, "oscPositionParam", sPos.mTarget));
}

void SoundSourcePanel::resized()
{
    auto dX = getWidth() / 12;
    auto dY = getHeight() / 12;
    pWaveDisplay->setBounds(dX / 5, dY / 5, 4.5 * dX, 4.5 * dY);
    selector.setBounds(dX * 5, dY / 3, dX * 6.5, dY / 2);
    envPanel.setBounds(dX / 5, 5 * dY, 8 * dX, 7 * dY);
    sPos.setBounds(8 * dX, 5 * dY, 2.5f * dY, 2.5f * dY);
}

void SoundSourcePanel::comboBoxChanged(juce::ComboBox *c)
{
    auto idx = c->getSelectedItemIndex();
    synth->replaceWave(idx);
    pWaveDisplay->removeListener();
    pWaveDisplay.reset(new WavetableDisplay(synth->getDataToGraph(), &sPos.mTarget));
    pWaveDisplay->setPosition(synth->getPosition());
    addAndMakeVisible(*pWaveDisplay);
    resized();
}

