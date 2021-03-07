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
    g.fillAll(juce::Colours::transparentBlack);
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
    g.setColour(colors.getByDesc("dullGray"));
    g.strokePath(track, strokeType);
    juce::Path thumb;
    
    g.setColour(colors.getByDesc("thumbYellow"));
    if(s.getValue() < 0.0f)
    {
        g.setColour(colors.getByDesc("thumbBlue"));
        thumb.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle + angle,  midAngle, true);
    }
    else
        thumb.addCentredArc(centerX, centerY, radius, radius, 0.0f, midAngle, rotaryStartAngle + angle, true);
    g.strokePath(thumb, strokeType);
}
