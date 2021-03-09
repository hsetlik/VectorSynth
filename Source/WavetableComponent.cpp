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
highlight(Color::RGBColor(255, 236, 95))
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
    auto centerIndex = floor(position * numTraces);
    workingColors.set(centerIndex, highlight);
    auto factor = 1.0f;
    auto exp = 0.25f;
    if(centerIndex + 1 < numTraces)
    {
        for(int i = centerIndex + 1; i < numTraces; ++i)
        {
            factor *= exp;
            workingColors.set(i, Color::blend(highlight, colors.atIndex(i), factor));
        }
    }
    if(centerIndex - 1 > 0)
    {
        factor = 1.0f;
        for(int i = centerIndex - 1; i >= 0; --i)
        {
            factor *= exp;
            workingColors.set(i, Color::blend(highlight, colors.atIndex(i), factor));
        }
    }
}

void WavetableDisplay::paint(juce::Graphics &g)
{
    setPosition(position);
    auto strokeType = juce::PathStrokeType(5.0f);
    g.fillAll(background);
    auto fBounds = getBounds().toFloat();
    auto y0 = fBounds.getHeight() / 2.0f;
    auto amplitude = y0 * 0.75f;
    int idx = 0;
    auto dX = fBounds.getWidth() / resolution;
    for(auto p : traces)
    {
        p->clear();
        float xPoint = fBounds.getX();
        float yPoint = fBounds.getBottom();
        p->startNewSubPath(xPoint, yPoint);
        p->lineTo(xPoint, y0);
        for(int point = 0; point < resolution; ++point)
        {
            xPoint += dX;
            yPoint = y0 + (amplitude * valueSet[idx][point]);
            p->lineTo(xPoint, yPoint);
        }
        p->lineTo(fBounds.getRight(), fBounds.getBottom());
        p->closeSubPath();
        g.setColour(workingColors.atIndex(idx));
        g.strokePath(*p, strokeType);
        ++idx;
    }
}

void WavetableDisplay::sliderValueChanged(juce::Slider *s)
{
    setPosition((float)s->getValue());
    repaint();
}
