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


/*
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
            for(int i = 0; i < numTraces - traces.size() + 1; ++i)
            {
                traces.add(new juce::Path());
                traces.getLast()->preallocateSpace(394);
            }
                
        }
        else if(traces.size() > numTraces)
        {
            traces.removeRange(numTraces, traces.size() - numTraces);
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
 */
struct WaveGraphDataset
{
    const int resolution = 128;
    const int max_frames = 32;
    int totalFrames;
    WaveGraphDataset(std::vector<std::vector<float>>& input) :
    rawData(std::make_unique<std::vector<std::vector<float>>>(max_frames, std::vector<float>(resolution, 0.0f)))
    {
        setData(input);
    }
    void setData(std::vector<std::vector<float>>& input)
    {
        int oIncrement;
        if((int)input.size() <= max_frames)
        {
            oIncrement = 1;
            totalFrames = (int)input.size();
        }
        else
        {
            oIncrement = (int)input.size() / max_frames;
            totalFrames = max_frames;
        }
        rawData.reset(new std::vector<std::vector<float>>(totalFrames, std::vector<float>(resolution, 0.0f)));
        int idx = 0;
        for(auto & frame : *rawData)
        {
            frame.assign(input[idx].begin(), input[idx].end());
        }
    }
    const std::vector<float>& operator [] (int index)
    {
        return rawData->at(index);
    }
    std::vector<juce::Path>* generatePaths(float width, float height)
    {
        auto* vec = new std::vector<juce::Path>(totalFrames, juce::Path());
        auto y0 = height / 2;
        auto amplitude = y0 * 0.65f;
        auto dX = (float)width / resolution;
        for(int i = 0; i < totalFrames; ++i)
        {
            auto& path = vec->at(i);
            path.preallocateSpace((3 +  resolution) * 3 + 1);
            path.startNewSubPath(0.0f, height);
            path.lineTo(0.0f, y0);
            for(int point = 0; point < resolution; ++point)
            {
                path.lineTo(dX * point, y0 + (rawData->at(i)[point] * amplitude));
            }
            path.lineTo(width, height);
            path.closeSubPath();
        }
        return vec;
        //this function concludes OK and returns expected
    }
    juce::Path* interpPath(float position, float width, float height)
    {
        auto* path = new juce::Path();
        path->preallocateSpace((3 +  resolution) * 3 + 1);
        auto fPos = position * totalFrames;
        auto bottomIdx = floor(fPos);
        auto topIdx = bottomIdx + 1;
        auto y0 = height / 2;
        auto amplitude = y0 * 0.65f;
        auto dX = (float)width / resolution;
        path->startNewSubPath(0.0f, height);
        path->lineTo(0.0f, y0);
        float sample, tSample, bSample, skew;
        for(int i = 0; i < resolution; ++i)
        {
            tSample = rawData->at(topIdx)[i];
            bSample = rawData->at(bottomIdx)[i];
            skew = fPos - bottomIdx;
            sample = bSample + ((tSample - bSample) * skew);
            path->lineTo(i * dX, y0 + (sample * amplitude));
        }
        path->lineTo(width, height);
        path->closeSubPath();
        return path;
    }
    std::unique_ptr<std::vector<std::vector<float>>> rawData;
};

class WavetableDisplay : public juce::Component, public juce::Slider::Listener
{
public:
    WavetableDisplay(std::vector<std::vector<float>> inData, juce::Slider* s) :
    graphData(inData),
    currentPath(std::make_unique<juce::Path>()),
    pathGroup(std::make_unique<std::vector<juce::Path>>(30, juce::Path())),
    stroke(3.0f)
    {
        setUpToDate = false;
        currentUpTpDate = false;
        fake3d = true;
        fPos = position * graphData.totalFrames;
        cExp = 0.75f;
    }
    void sliderValueChanged(juce::Slider* s) override
    {
        position = s->getValue();
        updatePosition();
    }
    void mouseDown(const juce::MouseEvent &e) override
    {
        fake3d = !fake3d;
    }
    void alterFor3d(juce::Path* p, float index)
    {
        auto fBounds = getBounds().toFloat();
        auto dX = fBounds.getWidth() / graphData.totalFrames / 4;
        auto dY = fBounds.getHeight() / graphData.totalFrames / 4;
        auto t = juce::AffineTransform::scale(0.55f, 0.55f).followedBy(juce::AffineTransform::shear(0.0f, 0.2f)).followedBy(juce::AffineTransform::translation((dX * index) + (fBounds.getWidth() / 8), -(dY * index * 0.7f) +  (fBounds.getHeight() / 5)));
        p->applyTransform(t);
    }
    void alterFor3d(juce::Path& p, float index)
    {
        auto fBounds = getBounds().toFloat();
        auto dX = fBounds.getWidth() / graphData.totalFrames / 4;
        auto dY = fBounds.getHeight() / graphData.totalFrames / 4;
        auto t = juce::AffineTransform::scale(0.55f, 0.55f).followedBy(juce::AffineTransform::shear(0.0f, 0.2f)).followedBy(juce::AffineTransform::translation((dX * index) + (fBounds.getWidth() / 8), -(dY * index * 0.7f) +  (fBounds.getHeight() / 5)));
        p.applyTransform(t);
    }
    juce::Colour colorForIndex(int index)
    {
        cCoeff = pow(fabs(index - fPos), cExp);
        return Color::blendHSB(UXColor::highlight, UXColor::lightGray, cCoeff);
    }
    void setValues(std::vector<std::vector<float>> nData);
    void updatePosition();
    void paint(juce::Graphics& g) override;
private:
    bool fake3d;
    bool setUpToDate, currentUpTpDate;
    float position;
    float cCoeff, delta, fPos, cExp;
    WaveGraphDataset graphData;
    std::unique_ptr<juce::Path> currentPath;
    std::unique_ptr<std::vector<juce::Path>> pathGroup;
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


