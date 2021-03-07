/*
  ==============================================================================

    DragDropContainer.h
    Created: 2 Mar 2021 6:10:17pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ModTargetComponent.h"
class DummyContainer : public juce::DragAndDropContainer, public juce::Component
{
public:
    DummyContainer() : source1(this, "source1"), source2(this, "source2", juce::Colours::lightgreen), source3(this, "source3", juce::Colours::lightgreen), source4(this, "source4", juce::Colours::lightgreen), source5(this, "source5", juce::Colours::lightgreen), target(this)
    {
        addAndMakeVisible(source1);
       
        addAndMakeVisible(source2);
        addAndMakeVisible(source3);
        addAndMakeVisible(source4);
        addAndMakeVisible(source5);
        addAndMakeVisible(target);
        allColors.add(Color::monochromeFrom(Color::RGBColor(169, 179, 193)));
        source1.setColor(allColors.getByDesc("ColorL0"));
        source2.setColor(allColors.getByDesc("ColorL1"));
        source3.setColor(allColors.getByDesc("ColorL2"));
        source4.setColor(allColors.getByDesc("ColorL3"));
        source5.setColor(allColors.getByDesc("ColorL4"));
        
    }
    ~DummyContainer() {}
    void resized() override
    {
        auto n = getWidth() / 8;
        source1.setBounds(n, n, n  / 2, n / 2);
        source2.setBounds(n, 2 * n, n / 2, n / 2);
        source3.setBounds(n, 3 * n, n / 2, n / 2);
        source4.setBounds(n, 4 * n, n / 2, n / 2);
        source5.setBounds(n, 5 * n, n / 2, n / 2);
        target.setBounds(4 * n, n, 3 * n, 3 * n);
    }
    void paint(juce::Graphics& g) override
    {
        
    }
private:
    ModSourceComponent source1;
    ModSourceComponent source2;
    ModSourceComponent source3;
    ModSourceComponent source4;
    ModSourceComponent source5;
    ModTargetComponent target;
};
