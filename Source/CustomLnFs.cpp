/*
  ==============================================================================

    CustomLnFs.cpp
    Created: 5 Mar 2021 7:43:39am
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "CustomLnFs.h"

void ModSystemLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& s)
{
    auto iBounds = juce::Rectangle<int> {x, y, width, height};
    auto fBounds = iBounds.toFloat();
    auto angle = fabs(rotaryStartAngle - rotaryEndAngle) * sliderPos;
    auto midAngle = (fabs(rotaryStartAngle - rotaryEndAngle)  /  2.0f) + rotaryStartAngle;
    auto strokeType = juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    auto centerX = fBounds.getX() + (fBounds.getWidth() / 2.0f);
    auto centerY = fBounds.getY() + (fBounds.getHeight() / 2.0f);
    auto radius = fBounds.getWidth() * 0.45f;
    juce::Path track;
    track.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(UXColor::lightGray);
    g.strokePath(track, strokeType);
    juce::Path thumb;
    
    g.setColour(UXColor::highlight);
    if(s.getValue() < 0.0f)
    {
        g.setColour(UXColor::thumbBlue);
        thumb.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle + angle,  midAngle, true);
    }
    else
        thumb.addCentredArc(centerX, centerY, radius, radius, 0.0f, midAngle, rotaryStartAngle + angle, true);
    g.strokePath(thumb, strokeType);
}

SynthSourceLookAndFeel::SynthSourceLookAndFeel()
{
    
}

void SynthSourceLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &s)
{
    auto iBounds = juce::Rectangle<int> {x, y, width, height};
    auto fBounds = iBounds.toFloat();
    g.setColour(UXColor::darkGray);
    g.fillEllipse(fBounds);
    g.setColour(UXColor::lightGray);
    auto angle = fabs(rotaryStartAngle - rotaryEndAngle) * sliderPos;
    auto centerX = fBounds.getX() + (fBounds.getWidth() / 2.0f);
    auto centerY = fBounds.getY() + (fBounds.getHeight() / 2.0f);
    auto radius = fBounds.getWidth() * 0.4f;
    auto strokeType = juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    juce::Path track;
    track.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.strokePath(track, strokeType);
    g.setColour(UXColor::highlight);
    auto iRadius = radius * 0.6f;
    juce::Path thumb;
    thumb.startNewSubPath(centerX, centerY - radius);
    thumb.lineTo(centerX, centerY - iRadius);
    thumb.closeSubPath();
    thumb.applyTransform(juce::AffineTransform::rotation(rotaryStartAngle + angle, centerX, centerY));
    g.strokePath(thumb, strokeType);
};
