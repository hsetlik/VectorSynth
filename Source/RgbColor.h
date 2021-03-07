/*
  ==============================================================================

    RgbColor.h
    Created: 6 Feb 2021 4:10:08pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
struct ColorTriad
{
    juce::Colour left, right, center;
};

struct ColorData {
    juce::Colour color;
    juce::String desc;
};

struct ColorSet {
    std::vector<ColorData> colors;
    void add(juce::Colour c, juce::String d)
    {
        ColorData data;
        data.color = c;
        data.desc = d;
        if(colors.size() > 0)
        {
            int numNameMatches = 0;
            for(auto i : colors) //appends a number if the name is the same as any already in the set
            {
                if(i.desc.contains(data.desc))
                    ++numNameMatches;
            }
            if(numNameMatches > 0)
                data.desc += juce::String(numNameMatches);
        }
        colors.push_back(data);
    }
    void add(ColorData& d)
    {
        ColorData newCol = d;
        if(colors.size() > 0)
        {
            int numNameMatches = 0;
            for(auto i : colors)
            {
                if(i.desc.contains(newCol.desc))
                    ++numNameMatches;
            }
            if(numNameMatches > 0)
                newCol.desc += juce::String(numNameMatches);
        }
        colors.push_back(newCol);
    }
    void add(ColorSet s)
    {
        for(auto i : s.colors)
        {
            add(i);
        }
    }
    juce::Colour getByDesc(juce::String d)
    {
        for(auto i : colors)
        {
            if(i.desc == d)
                return i.color;
        }
        return juce::Colours::black;
    }
    
    juce::Colour getByDesc(const char* s)
    {
        auto d = juce::String(s);
        for(auto i : colors)
        {
            if(i.desc == d)
                return i.color;
        }
        return juce::Colours::black;
    }
};


struct Color
{
    //fourth argument sets alpha, fully opaque by default
    static juce::Colour RGBColor(int r, int g, int b, int a = 255)
    {
        return(juce::Colour(juce::uint8(r), juce::uint8(g), juce::uint8(b), juce::uint8(a)));
    }
    
    static juce::Colour blend(juce::Colour colorA, juce::Colour colorB, float ratio)
    {
        auto fRedA = colorA.getFloatRed();
        auto fGreenA = colorA.getFloatGreen();
        auto fBlueA = colorA.getFloatBlue();
        
        auto fRedB = colorB.getFloatRed();
        auto fGreenB = colorB.getFloatGreen();
        auto fBlueB = colorB.getFloatBlue();
        
        auto adjRedA = fRedA * (1.0f - ratio);
        auto adjGreenA = fGreenA * (1.0f - ratio);
        auto adjBlueA = fBlueA * (1.0f - ratio);
        
        auto adjRedB = fRedB * ratio;
        auto adjGreenB = fGreenB * ratio;
        auto adjBlueB = fBlueB * ratio;
        
        auto mixRed = (adjRedA + adjRedB) / 2.0f;
        auto mixGreen = (adjGreenA + adjGreenB) / 2.0f;
        auto mixBlue = (adjBlueA + adjBlueB) / 2.0f;
        
        int iRed = floor(mixRed * 255);
        int iGreen = floor(mixGreen * 255);
        int iBlue = floor(mixBlue * 255);
        
        return RGBColor(iRed, iGreen, iBlue);
    }
    
    static juce::Colour complement(juce::Colour start)
    {
        auto fHue = start.getHue();
        auto fSat = start.getSaturationHSL();
        auto fLgt = start.getLightness();
        
        auto dHueStart = fHue * 360.0;
        auto hue = 360.0 - dHueStart;
        return juce::Colour(hue, fSat, fLgt, 1.0f);
    }
    
    static ColorSet triadFrom(juce::Colour center)
    {
        ColorSet set;
        ColorTriad triad;
        triad.center = center;
        set.add(triad.center, "Center");
        auto fHue = center.getHue() * 360;
        auto rHue = fHue + 120;
        if(rHue > 360)
            rHue -= 360;
        else if(rHue < 0)
            rHue += 360;
        triad.right = juce::Colour(rHue, center.getSaturationHSL(), center.getLightness(), 1.0f);
        set.add(triad.right, "Right");
        auto lHue = fHue - 120;
        if(lHue > 360)
            lHue -= 360;
        else if(lHue < 0)
            lHue += 360;
        triad.left = juce::Colour(lHue, center.getSaturationHSL(), center.getLightness(), 1.0f);
        set.add(triad.left, "Left");
        return set;
    }
    
    static ColorSet monochromeFrom(juce::Colour center, bool skewSaturation=false, juce::String prefix="")
    {
        auto fHue = center.getHue();
        auto fSat = center.getSaturation();
        auto fLgt = center.getLightness();
        auto increment = (fLgt - 0.1f) / 5.0f; //note: center color must have a lightness value grater than 0.1 to start
        ColorSet set;
        for(int i = 0; i < 5; ++i)
        {
            if(skewSaturation)
                fSat = 0.15f + (0.12 * i);
           set.add(juce::Colour(fHue, fSat, (fLgt - (i * increment)), 1.0f), prefix + "ColorL" + juce::String(i));
        }
        return set;
    }
};
