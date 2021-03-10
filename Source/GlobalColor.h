/*
  ==============================================================================

    GlobalColor.h
    Created: 9 Mar 2021 6:02:29pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include "RgbColor.h"
namespace UXColor
{
static juce::Colour gBackground = Color::RGBColor(37, 49, 53);
static juce::Colour highlight = Color::RGBColor(255, 236, 95);
static juce::Colour darkBkgnd = juce::Colours::black.brighter(0.1f);
static juce::Colour darkGray = Color::RGBColor(55, 56, 68);
static juce::Colour grayShadeL = Color::RGBColor(169, 179, 193);
static juce::Colour grayShadeD = grayShadeL.darker(0.3f);
static std::vector<juce::Colour> modTargetShades = Color::shadesBetweenVec(grayShadeD, grayShadeL, 5);
static juce::Colour thumbYellow = Color::RGBColor(255, 236, 95);
static juce::Colour thumbBlue = Color::RGBColor(32, 139, 181);
static juce::Colour lightGray = Color::RGBColor(125, 126, 129);


};
