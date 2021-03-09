/*
  ==============================================================================

    WavetableComponent.cpp
    Created: 7 Mar 2021 9:51:53pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "WavetableComponent.h"
WavetableDisplay::WavetableDisplay(std::vector<std::vector<float>> data, juce::Slider* s, float pos) :
valueSet(data),
position(pos),
highlight(Color::RGBColor(255, 236, 95)),
currentValues(128, 0.0f)
{
    s->addListener(this);
    background = Color::RGBColor(53, 62, 68);
    numTraces = 0;
    resolution = 128;
    for(auto d : valueSet)
    {
        ++numTraces;
        traces.add(new juce::Path());
    }
    colors.add(Color::shadesBetween(Color::RGBColor(199, 200, 202), Color::RGBColor(37, 49, 53), numTraces));
}

void WavetableDisplay::setPosition(float pos)
{
    workingColors = colors;
    position = pos;
    auto lowerIdx = floor(position * (numTraces - 1) * 0.99f);
    auto upperIdx = lowerIdx + 1;
    auto& upperData = valueSet[upperIdx];
    auto& lowerData = valueSet[lowerIdx];
    auto skew = (position * (numTraces - 1) * 0.99f) - (float)lowerIdx;
    workingColors.set(lowerIdx, highlight);
    auto aExp = 0.45f;
    auto bExp = 0.6f;
    auto trcColor = Color::complement(highlight);
    for(int i = 0; i < numTraces; ++i)
    {
        auto diff = fabs(i - (position * (numTraces - 1)));
        auto brt = 1.0f * pow(bExp, diff);
        auto alpha = 1.0f * pow(aExp, diff);
        auto col = Color::desaturated(trcColor.withAlpha(alpha).withBrightness(brt), 1.0f - brt);
        workingColors.set(i, col);
    }
    for(int i = 0; i < resolution; ++i)
    {
        currentValues[i] = lowerData[i] + ((upperData[i] - lowerData[i]) * skew);
    }
}

void WavetableDisplay::paint(juce::Graphics &g)
{
    setPosition(position);
    auto strokeType = juce::PathStrokeType(2.0f);
    g.fillAll(background);
    auto fBounds = getBounds().toFloat();
    auto y0 = fBounds.getHeight() / 2.0f;
    auto amplitude = y0 * 0.75f;
    auto dX = fBounds.getWidth() / resolution;
    if(numTraces >= 1)
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
            g.setColour(workingColors.atIndex(p));
            g.strokePath(*traces[p], strokeType);
        }
        juce::Path current;
        current.startNewSubPath(0.0f, fBounds.getBottom());
        current.lineTo(0.0f, y0);
        for(int i = 0; i < resolution; ++i)
        {
            current.lineTo(i * dX, y0 + (currentValues[i] * amplitude));
        }
        current.lineTo(fBounds.getRight(), fBounds.getBottom());
        current.closeSubPath();
        g.setColour(highlight);
        g.strokePath(current, strokeType);
    }
}

void WavetableDisplay::sliderValueChanged(juce::Slider *s)
{
    setPosition((float)s->getValue());
    repaint();
}
