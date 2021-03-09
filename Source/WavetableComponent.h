/*
  ==============================================================================

    WavetableComponent.h
    Created: 7 Mar 2021 9:51:53pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "RgbColor.h"
#include "WavetableProcessor.h"
#include "EnvelopeComponent.h"

class WavetableDisplay : public juce::Component, public juce::Slider::Listener
{
public:
    WavetableDisplay(std::vector<std::vector<float>> data, juce::Slider* s, float pos = 0.0f);
    ~WavetableDisplay() {};
    void paint(juce::Graphics& g) override;
    void setPosition(float pos); //recalculate colors in here;
    void sliderValueChanged(juce::Slider* s) override;
    void alterFor3d(juce::Path* p, float index)
    {
        auto fBounds = getBounds().toFloat();
        auto dX = fBounds.getWidth() / numTraces / 4;
        auto dY = fBounds.getHeight() / numTraces / 4;
        auto t = juce::AffineTransform::scale(0.55f, 0.55f).followedBy(juce::AffineTransform::shear(0.0f, 0.2f)).followedBy(juce::AffineTransform::translation((dX * index) + (fBounds.getWidth() / 8), -(dY * index * 0.7f) +  (fBounds.getHeight() / 5)));
        p->applyTransform(t);
        
    }
    void mouseDown(const juce::MouseEvent &m) override
    {
        fake3d = !fake3d;
        setPosition(position);
        repaint();
    }
private:
    bool fake3d;
    int resolution;
    int numTraces;
    std::vector<std::vector<float>> valueSet;
    float position;
    juce::Colour background;
    juce::OwnedArray<juce::Path> traces;
    juce::Colour highlight;
    ColorSet colors;
    ColorSet workingColors;
    std::vector<float> currentValues;
};

class SoundSourcePanel : public juce::Component
{
public:
    SoundSourcePanel(juce::DragAndDropContainer* c);
private:
    
};


