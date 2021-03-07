/*
  ==============================================================================

    ModSourceComponent.h
    Created: 2 Mar 2021 6:03:33pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RgbColor.h"
static ColorSet allColors;

const juce::Colour sourceRimColor = Color::RGBColor(37, 49, 53);

class ModSourceComponent : public juce::Component
{
public:
    //default constructor with light blue center
    ModSourceComponent(juce::DragAndDropContainer* cont, juce::String desc) : sourceId(desc), parent(cont)
    {
        centerColor = Color::RGBColor(159, 207, 219);
    }
    //with color argument
    ModSourceComponent(juce::DragAndDropContainer* cont, juce::String desc, juce::Colour color) : centerColor(color), sourceId(desc), parent(cont)
    {
        
    }
    ~ModSourceComponent() {}
    void resized() override
    {
    }
    void paint(juce::Graphics& g) override
    {
        auto area = getLocalBounds().toFloat();
        auto center = getLocalBounds().reduced(6).toFloat();
        g.setColour(sourceRimColor);
        g.fillEllipse(area);
        g.setColour(centerColor);
        g.fillEllipse(center);
    }
    void mouseDown(const juce::MouseEvent& e) override
    {
        juce::DragAndDropContainer::findParentDragContainerFor(this)->startDragging(sourceId, this);
    }
    void setColor(juce::Colour n)
    {
        centerColor = n;
    }
    juce::Colour getColor()
    {
        return centerColor;
    }
    juce::String getId()
    {
        return sourceId;
    }
private:
    juce::Colour centerColor;
    juce::String sourceId;
    juce::DragAndDropContainer* parent;
};
