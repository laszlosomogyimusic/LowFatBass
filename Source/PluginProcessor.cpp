/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LowFatBassAudioProcessor::LowFatBassAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    for (int i = 0; i < numVoices; i++)
    {
        sampler.addVoice(new juce::SamplerVoice());
    }
}

LowFatBassAudioProcessor::~LowFatBassAudioProcessor()
{
    delete formatReader;
}

//==============================================================================
const juce::String LowFatBassAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LowFatBassAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LowFatBassAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool LowFatBassAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double LowFatBassAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LowFatBassAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int LowFatBassAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LowFatBassAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String LowFatBassAudioProcessor::getProgramName (int index)
{
    return {};
}

void LowFatBassAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void LowFatBassAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    sampler.setCurrentPlaybackSampleRate(sampleRate);
}

void LowFatBassAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LowFatBassAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void LowFatBassAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{
    //    auto* channelData = buffer.getWritePointer (channel);

    //    // ..do something to the data...
    //}
    juce::MidiMessage m;
    juce::MidiBuffer::Iterator it{ midiMessages };

    int sample;

    while (it.getNextEvent(m, sample))
    {
        if (m.isNoteOn())
        {
            isNotePlayed = true;
        }
        else if (m.isNoteOff())
        {
            isNotePlayed = false;
        }
    }

    sampleCount = isNotePlayed ? sampleCount += buffer.getNumSamples() : 0;

    sampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

}

//==============================================================================
bool LowFatBassAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* LowFatBassAudioProcessor::createEditor()
{
    return new LowFatBassAudioProcessorEditor (*this);
}

//==============================================================================
void LowFatBassAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void LowFatBassAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void LowFatBassAudioProcessor::loadFile(const juce::String& path)
{
    sampler.clearSounds();
    auto file = juce::File(path);
    formatManager.registerBasicFormats();
    formatReader = formatManager.createReaderFor(file);

    auto sampleLength = static_cast<int>(formatReader->lengthInSamples);

    waveForm.setSize(1, sampleLength);
    formatReader->read(&waveForm, 0, sampleLength, 0, true, false);

    auto buffer = waveForm.getReadPointer(0);

    juce::BigInteger range;
    range.setRange(52, 1, true);

    juce::BigInteger e1;
    e1 = 52;

    sampler.addSound(new juce::SamplerSound("E1", *formatReader, range, 52, 0.0, 0.6, 60));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LowFatBassAudioProcessor();
}
