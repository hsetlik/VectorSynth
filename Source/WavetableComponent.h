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


struct GraphPathGroup
{
    const int resolution = 128;
    const int max_paths = 30;
    int pathSize;
    juce::Path makePresizedPath(int size)
    {
        auto* path = new juce::Path();
        path->preallocateSpace(size);
        return *path;
    }
    GraphPathGroup(std::vector<std::vector<float>> input, float width, float height);
    void resetData(std::vector<std::vector<float>> input);
    const juce::Path& operator [] (int index)
    {
        return paths->at(index);
    }
    void alterFor3d(float index)
    {
        auto dX = cWidth / pathCount() / 4;
        auto dY = cHeight / pathCount() / 4;
        auto t = juce::AffineTransform::scale(0.55f, 0.55f).followedBy(juce::AffineTransform::shear(0.0f, 0.2f)).followedBy(juce::AffineTransform::translation((dX * index) + (cWidth / 8), -(dY * index * 0.7f) +  (cWidth / 5)));
        paths->at((int)index).applyTransform(t);
    }
    void alterFor3d(juce::Path* p, float index)
    {
        auto dX = cWidth / pathCount() / 4;
        auto dY = cHeight / pathCount() / 4;
        auto t = juce::AffineTransform::scale(0.55f, 0.55f).followedBy(juce::AffineTransform::shear(0.0f, 0.2f)).followedBy(juce::AffineTransform::translation((dX * index) + (cWidth / 8), -(dY * index * 0.7f) +  (cWidth / 5)));
        p->applyTransform(t);
    }
    int pathCount() {return (int)paths->size();}
    std::unique_ptr<std::vector<juce::Path>> paths;
    std::vector<std::vector<float>> rawData;
    float cWidth, cHeight;
};

class WavetableDisplay : public juce::Component, public juce::Slider::Listener
{
public:
    WavetableDisplay(std::vector<std::vector<float>> data, juce::Slider* s, float pos = 0.0f);
    ~WavetableDisplay() {};
    void paint(juce::Graphics& g) override;
    void setPosition(float pos); //recalculate colors in here;
    void sliderValueChanged(juce::Slider* s) override;
    
    
    void mouseDown(const juce::MouseEvent &m) override
    {
        fake3d = !fake3d;
        setPosition(position);
        repaint();
    }
    void setValues(std::vector<std::vector<float>> vals)
    {
        pathGroup.cWidth = fBounds.getWidth();
        pathGroup.cHeight = fBounds.getHeight();
        pathGroup.resetData(vals);
    }
private:
    float topSample, bottomSample, cSample;
    int upperIdx, lowerIdx;
    juce::Rectangle<float> fBounds;
    bool fake3d;
    int resolution;
    bool tracesNeedRepaint;
    juce::Path current;
    float position;
    juce::Colour background;
    juce::Colour highlight;
    ColorSet colors;
    ColorSet workingColors;
    GraphPathGroup pathGroup;
    juce::PathStrokeType stroke;
};

class ArrowButton : public juce::ShapeButton
{
public:
    ArrowButton(bool p, juce::Button::Listener* l) :  juce::ShapeButton("modButton",  juce::Colours::black, juce::Colours::black, juce::Colours::black), pointsLeft(p)
    {
        addListener(l);
    }
    ~ArrowButton() {}
    void paintButton(juce::Graphics& g, bool mouseOver, bool isMouseDown) override
    {
        auto fBounds = getLocalBounds().toFloat();
        outer.clear();
        outer.addRoundedRectangle(fBounds, fBounds.getWidth() / 8.0f);
        g.setColour(UXColor::darkGray);
        g.fillPath(outer);
        inner.clear();
        inner.addRoundedRectangle(fBounds.reduced(fBounds.getWidth() / 15), fBounds.getWidth() / 8.0f);
        g.setColour(UXColor::lightGray);
        g.fillPath(inner);
        g.setColour(UXColor::darkGray);
        arrow.clear();
        auto t1 = juce::Point<float>(fBounds.getWidth() / 3, fBounds.getHeight() / 4);
        auto t2 = juce::Point<float>(fBounds.getWidth() / 3, 3 * (fBounds.getHeight() / 4));
        auto t3 = juce::Point<float>(2 * (fBounds.getWidth() / 3), fBounds.getHeight() / 2);
        arrow.addTriangle(t1, t2, t3);
        if(pointsLeft)
            arrow.applyTransform(juce::AffineTransform::rotation((float)M_PI, fBounds.getWidth() / 2, fBounds.getHeight() / 2));
        g.fillPath(arrow);
    }
private:
    bool pointsLeft;
    juce::Path outer;
    juce::Path inner;
    juce::Path arrow;
};

class WaveSelector : public juce::Component, public juce::Button::Listener
{
public:
    WaveSelector(WavetableOscHolder* o, juce::ComboBox::Listener* list);
    ~WaveSelector() {}
    void buttonClicked(juce::Button* b) override
    {
        auto idx = waveBox.getSelectedItemIndex();
        if(b == &lButton)
        {
            if(idx > 1)
                waveBox.setSelectedItemIndex(idx - 1);
        }
        else
        {
            if(idx < tableNames.size())
                waveBox.setSelectedItemIndex(idx + 1);
        }
    }
    void resized() override
    {
        auto n = getWidth() / 12;
        auto h = getHeight();
        lButton.setBounds(0, 0, n, h);
        rButton.setBounds(n, 0, n, h);
        waveBox.setBounds(2 * n, 0, 10 * n, h);
    }
private:
    WavetableOscHolder* osc;
    juce::ComboBox waveBox;
    ArrowButton lButton;
    ArrowButton rButton;
    juce::StringArray tableNames;
};

class SoundSourcePanel : public juce::Component, public juce::ComboBox::Listener
{
public:
    SoundSourcePanel(juce::DragAndDropContainer* c, juce::AudioProcessorValueTreeState* t, WavetableOscHolder* o);
    ~SoundSourcePanel() {}
    void resized() override;
    void comboBoxChanged(juce::ComboBox* c) override;
    void assignWavFiles(juce::Array<juce::File> arr)
    {
        wavFiles = arr;
    }
private:
    juce::Array<juce::File> wavFiles;
    TablePositionSlider sPos;
    OscLevelSlider sLevel;
    DAHDSRPanel envPanel;
    WavetableDisplay waveGraph;
    WaveSelector selector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> posAttach;
    WavetableOscHolder* osc;
};


