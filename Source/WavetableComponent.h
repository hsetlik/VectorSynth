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

struct graphDataset
{
    const int resolution = 128;
    const int max_frames = 30;
    graphDataset(std::vector<std::vector<float>> input) :
    values(std::make_unique<std::vector<std::vector<float>>>(max_frames, std::vector<float>(resolution, 0.0f)))
    {setData(input);}
    void setData(std::vector<std::vector<float>> input)
    {
        int oInc;
        int framesToMake;
        auto inputFrames = (int)input.size();
        if(inputFrames <= max_frames)
        {
            oInc = 1;
            framesToMake = inputFrames;
        }
        else
        {
            oInc = inputFrames / max_frames;
            framesToMake = max_frames;
        }
        values.reset(new std::vector<std::vector<float>>(framesToMake, std::vector<float>(resolution, 0.0f)));
        for(int i = 0; i < framesToMake; ++i)
        {
            auto& inVec = input[i];
            values->at(i).assign(inVec.begin(), inVec.end());
        }
        values->shrink_to_fit();
    }
    std::unique_ptr<std::vector<std::vector<float>>> values;
};

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
        fBounds = getBounds().toFloat();
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
    void setValues(std::vector<std::vector<float>> vals)
    {
        valueSet.clear();
        valueSet = vals;
        numTraces = (int)valueSet.size();
        if(traces.size() < numTraces)
        {
            traces.ensureStorageAllocated(numTraces);
            for(int i = 0; i < numTraces - traces.size(); ++i)
            {
                traces.add(new juce::Path());
                traces.getLast()->preallocateSpace(394);
            }
        }
        else if(traces.size() > numTraces)
        {
            for(int i = 0; i < traces.size() - numTraces; ++i)
            {
                traces.remove(traces.size() - 1);
            }
            traces.minimiseStorageOverheads();
        }
        setPosition(position);
        tracesNeedRepaint = true;
        repaint();
    }
private:
    std::unique_ptr<juce::Path> blank;
    juce::Rectangle<float> fBounds;
    bool fake3d;
    int resolution;
    bool tracesNeedRepaint;
    int numTraces;
    juce::Path current;
    juce::OwnedArray<juce::Path, juce::CriticalSection> traces;
    std::vector<std::vector<float>> valueSet;
    float position;
    juce::Colour background;
    juce::Colour highlight;
    ColorSet colors;
    ColorSet workingColors;
    std::vector<float> currentValues;
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
    WaveSelector(WavetableOsc* o, juce::ComboBox::Listener* list);
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
    WavetableOsc* osc;
    juce::ComboBox waveBox;
    ArrowButton lButton;
    ArrowButton rButton;
    juce::StringArray tableNames;
};

class SoundSourcePanel : public juce::Component, public juce::ComboBox::Listener
{
public:
    SoundSourcePanel(juce::DragAndDropContainer* c, juce::AudioProcessorValueTreeState* t, WavetableOsc* o);
    ~SoundSourcePanel() {}
    void resized() override;
    void comboBoxChanged(juce::ComboBox* c) override;
private:
    TablePositionSlider sPos;
    OscLevelSlider sLevel;
    DAHDSRPanel envPanel;
    WavetableDisplay waveGraph;
    WaveSelector selector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> posAttach;
    WavetableOsc* osc;
};


