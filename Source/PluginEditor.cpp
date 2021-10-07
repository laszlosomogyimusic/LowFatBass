/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LowFatBassAudioProcessorEditor::LowFatBassAudioProcessorEditor (LowFatBassAudioProcessor& p)
    : AudioProcessorEditor(&p), pianoRoll{ p }, audioProcessor(p)
{
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::bass_jpg, BinaryData::bass_jpgSize);

    p.loadFile("c://temp//lfb//001_E1.wav");

    path.addEllipse(10, 20, 10, 10);
    normal.setPath(path);
    normal.setFill(juce::Colours::lightblue);
    normal.setStrokeFill(juce::Colours::black);
    normal.setStrokeThickness(0.2f);

    over.setPath(path);
    over.setFill(juce::Colours::blue);
    over.setStrokeFill(juce::Colours::black);
    over.setStrokeThickness(0.2f);

    auto popupMessageCallback = [this]
    {
        if (auto* focused = Component::getCurrentlyFocusedComponent())
            showBubbleMessage(*focused,
                "This is a demo of the BubbleMessageComponent, which lets you pop up a message pointing "
                "at a component or somewhere on the screen.\n\n"
                "The message bubbles will disappear after a timeout period, or when the mouse is clicked.",
                this->bubbleMessage,
                true);
    };

    {
        // create an image-above-text button from these drawables..
        auto db = addToList(new juce::DrawableButton("Button 1", juce::DrawableButton::ImageAboveTextLabel));
        db->setImages(&normal, &over, &over);
        db->setBounds(260, 60, 50, 50);
        db->setTooltip("This is a DrawableButton with a label");
        db->onClick = popupMessageCallback;
    }

    addAndMakeVisible(pianoRoll);
    setSize (1024, 768);
}

LowFatBassAudioProcessorEditor::~LowFatBassAudioProcessorEditor()
{
}

//==============================================================================
void LowFatBassAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.drawImageAt(backgroundImage, 0, 50);
}

void LowFatBassAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    pianoRoll.setBoundsRelative(0.0f, 0.75f, 1.0f, 0.25f);
}

template <typename ComponentType>
ComponentType* LowFatBassAudioProcessorEditor::addToList(ComponentType* newComp)
{
    components.add(newComp);
    addAndMakeVisible(newComp);
    return newComp;
}

void LowFatBassAudioProcessorEditor::showBubbleMessage(Component& targetComponent, const juce::String& textToShow,
    std::unique_ptr<juce::BubbleMessageComponent>& bmc,
    bool isRunningComponentTransformDemo)
{
    bmc.reset(new juce::BubbleMessageComponent());

    if (isRunningComponentTransformDemo)
    {
        targetComponent.findParentComponentOfClass<LowFatBassAudioProcessorEditor>()->addChildComponent(bmc.get());
    }
    else if (juce::Desktop::canUseSemiTransparentWindows())
    {
        bmc->setAlwaysOnTop(true);
        bmc->addToDesktop(0);
    }
    else
    {
        targetComponent.getTopLevelComponent()->addChildComponent(bmc.get());
    }

    juce::AttributedString text(textToShow);
    text.setJustification(juce::Justification::centred);
    text.setColour(targetComponent.findColour(juce::TextButton::textColourOffId));

    bmc->showAt(&targetComponent, text, 2000, true, false);
}

