/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PianoRoll.h"

//==============================================================================
/**
*/
class LowFatBassAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    LowFatBassAudioProcessorEditor (LowFatBassAudioProcessor&);
    ~LowFatBassAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Image backgroundImage;
    juce::DrawableRectangle rectangle;
    juce::Path path;
    juce::DrawablePath normal, over;

    LowFatBassAudioProcessor& audioProcessor;

    //PianoRoll pianoRoll;

    juce::OwnedArray<Component> components;
    std::unique_ptr<juce::BubbleMessageComponent> bubbleMessage;

    template <typename ComponentType>
    ComponentType* addToList(ComponentType* newComp);
    static void showBubbleMessage(Component& targetComponent, const juce::String& textToShow,
        std::unique_ptr<juce::BubbleMessageComponent>& bmc,
        bool isRunningComponentTransformDemo);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LowFatBassAudioProcessorEditor)
};
