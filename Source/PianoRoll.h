/*
  ==============================================================================

    PianoRoll.h
    Created: 29 Sep 2021 9:50:53am
    Author:  Laszlo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class PianoRoll  : public juce::Component,
                   private juce::MidiInputCallback,
                   private juce::MidiKeyboardStateListener
{
public:
    PianoRoll(LowFatBassAudioProcessor& p);
    ~PianoRoll() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
    void handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

private:
    juce::AudioDeviceManager deviceManager;         //to find which MIDI input devices are enabled
    juce::ComboBox midiInputList;                   //display the names of the MIDI devices
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;                         //this is used to de-register a previously selected MIDI input
    bool isAddingFromMidiInput = false;             //is MIDI data arriving from external source or mouse click on the on-screen keyboard

    juce::MidiKeyboardState keyboardState;          //keeps track of which MIDI keys are currently held down
    juce::MidiKeyboardComponent keyboardComponent;  //on-screen keyboard

    juce::TextEditor midiMessagesBox;
    double startTime;
    void logMessage(const juce::String& m);
    static juce::String getMidiMessageDescription(const juce::MidiMessage& m);


    LowFatBassAudioProcessor& processor;
    
    void setMidiInput(int index);

    // This is used to dispach an incoming message to the message thread
    class IncomingMessageCallback : public juce::CallbackMessage
    {
    public:
        IncomingMessageCallback(PianoRoll* o, const juce::MidiMessage& m, const juce::String& s)
            : owner(o), message(m), source(s)
        {}

        void messageCallback() override
        {
            if (owner != nullptr)
                owner->addMessageToList(message, source);
        }

        Component::SafePointer<PianoRoll> owner;
        juce::MidiMessage message;
        juce::String source;
    };

    void postMessageToList(const juce::MidiMessage& message, const juce::String& source);
    void addMessageToList(const juce::MidiMessage& message, const juce::String& source);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRoll)
};
