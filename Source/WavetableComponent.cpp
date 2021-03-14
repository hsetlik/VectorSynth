/*
  ==============================================================================

    WavetableComponent.cpp
    Created: 7 Mar 2021 9:51:53pm
    Author:  Hayden Setlik

  ==============================================================================
*/

#include "WavetableComponent.h"
GraphPathGroup::GraphPathGroup(std::vector<std::vector<float>> input, float width, float height) :
paths(std::make_unique<std::vector<juce::Path>>(max_paths, juce::Path())),
cWidth(width),
cHeight(height)
{
    pathSize = (resolution + 3) * 3 + 1;
    resetData(input);
}

void GraphPathGroup::resetData(std::vector<std::vector<float>> input)
{
    int numPaths;
    int oIncrement;
    if((int)input.size() <= max_paths)
    {
        numPaths = (int)input.size();
        oIncrement = 1;
    }
    else
    {
        numPaths = max_paths;
        oIncrement = (int)(input.size() / max_paths);
    }
    paths.reset(new std::vector<juce::Path>(numPaths, makePresizedPath(pathSize)));
    auto y0 = cHeight / 2.0f;
    auto amplitude = y0 * 0.65f;
    auto dX = cWidth / resolution;
    int oIdx = 0;
    for(auto& path : *paths)
    {
        rawData.push_back(input[oIdx * oIncrement]);
        path.startNewSubPath(0.0f, cHeight);
        path.lineTo(0.0f, y0);
        for(int i = 0; i < resolution; ++i)
        {
            path.lineTo(i * dX, input[oIdx * oIncrement][i] * amplitude);
        }
        path.lineTo(cWidth, cHeight);
        path.closeSubPath();
        ++oIdx;
    }
    paths->shrink_to_fit();
}


WavetableDisplay::WavetableDisplay(std::vector<std::vector<float>> data, juce::Slider* s, float pos) :
fake3d(true),
position(pos),
highlight(UXColor::highlight),
pathGroup(data, (float)getWidth(), (float)getHeight()),
stroke(3.0f)
{
    s->addListener(this);
    background = UXColor::darkBkgnd;
    resolution = 128;
    current.preallocateSpace(394);
    colors.add(Color::shadesBetween(Color::RGBColor(199, 200, 202), UXColor::grayShadeD, pathGroup.pathCount()));
}

void WavetableDisplay::setPosition(float pos)
{
    position = pos;
    workingColors = colors;
    auto aExp = 0.65f;
    auto tColorA = highlight.withBrightness(highlight.getBrightness() / 8).withSaturation(highlight.getSaturation() / 6);
    auto tColorB = highlight.withBrightness(highlight.getBrightness() * 0.8f);
    for(int i = 0; i < pathGroup.pathCount(); ++i)
    {
        auto diff = fabs(i - (position * (pathGroup.pathCount() - 1)));
        auto alpha = 1.0f * pow(aExp, diff);
        auto col = Color::blendHSB(tColorA, tColorB, alpha);
        workingColors.set(i, col);
    }
    auto y0 = fBounds.getHeight() / 2.0f;
    auto dX = fBounds.getWidth() / resolution;
    auto amplitude = y0 * 0.65f;
    auto cPos = position * pathGroup.pathCount();
    lowerIdx = floor(cPos);
    upperIdx = (ceil(cPos <= pathGroup.pathCount())) ? lowerIdx + 1 : lowerIdx;
    auto skew = cPos - lowerIdx;
    current.clear();
    current.preallocateSpace(pathGroup.pathSize);
    current.startNewSubPath(fBounds.getHeight(), 0.0f);
    current.lineTo(0.0f, y0);
    for(int i = 0; i < resolution; ++i)
    {
        topSample = pathGroup.rawData[upperIdx][i];
        bottomSample = pathGroup.rawData[lowerIdx][i];
        cSample = bottomSample + ((topSample - bottomSample) * skew);
        current.lineTo(dX * i, y0 + (cSample * amplitude));
    }
    current.lineTo(fBounds.getWidth(), fBounds.getHeight());
    current.closeSubPath();
}

void WavetableDisplay::paint(juce::Graphics &g)
{
    if(fake3d)
    {
        for(int i = 0; i < position * pathGroup.pathCount(); ++i)
        {
            g.setColour(workingColors.atIndex(i));
            pathGroup.alterFor3d((float)i);
            g.strokePath(pathGroup[i], stroke);
        }
        g.setColour(highlight);
        pathGroup.alterFor3d(&current, position * pathGroup.pathCount());
        g.strokePath(current, stroke);
        for(int i = ceil(position * pathGroup.pathCount()); i < pathGroup.pathCount(); ++i)
        {
            g.setColour(workingColors.atIndex(i));
            pathGroup.alterFor3d((float)i);
            g.strokePath(pathGroup[i], stroke);
        }
    }
    
}

void WavetableDisplay::sliderValueChanged(juce::Slider *s)
{
    setPosition((float)s->getValue());
    repaint();
}

WaveSelector::WaveSelector(WavetableOscHolder* o, juce::ComboBox::Listener* list) : osc(o), lButton(true, this), rButton(false, this), tableNames(osc->waveNames)
{
    addAndMakeVisible(&waveBox);
    addAndMakeVisible(&rButton);
    addAndMakeVisible(&lButton);
    waveBox.addItemList(tableNames, 1);
    waveBox.setSelectedItemIndex(1);
    waveBox.addListener(list);
}

SoundSourcePanel::SoundSourcePanel(juce::DragAndDropContainer* c, juce::AudioProcessorValueTreeState* t, WavetableOscHolder* o) :
sPos(c, 0),
sLevel(c, 0),
envPanel(c),
waveGraph(o->getDataToGraph(128), &sPos.mTarget),
selector(o, this),
osc(o)
{
    addAndMakeVisible(sPos);
    addAndMakeVisible(sLevel);
    addAndMakeVisible(envPanel);
    addAndMakeVisible(waveGraph);
    addAndMakeVisible(selector);
    
    freqAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(*t, "frequency", sLevel.mTarget));
    posAttach.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(*t, "wavetablePos", sPos.mTarget));
}

void SoundSourcePanel::resized()
{
    auto dX = getWidth() / 12;
    auto dY = getHeight() / 12;
    waveGraph.setBounds(dX / 5, dY / 5, 4.5 * dX, 4.5 * dY);
    selector.setBounds(dX * 5, dY / 3, dX * 6.5, dY / 2);
    envPanel.setBounds(dX / 5, 5 * dY, 8 * dX, 7 * dY);
    sLevel.setBounds(8 * dX, 2 * dY, 2.5f * dY, 2.5f * dY);
    sPos.setBounds(8 * dX, 5 * dY, 2.5f * dY, 2.5f * dY);
}

void SoundSourcePanel::comboBoxChanged(juce::ComboBox *c)
{
    if(wavFiles.size() > 0)
    {
        auto idx = c->getSelectedItemIndex();
        osc->replaceFromFile(wavFiles[idx]);
    }
}

