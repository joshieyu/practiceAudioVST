#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginProcessor::PluginProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
      parameters(*this, nullptr, juce::Identifier("FilterPlugin"),
          {
            std::make_unique<juce::AudioParameterFloat>("gain",            // parameterID
                                                        "Gain",            // parameter name
                                                        0.0f,              // minimum value
                                                        1.0f,              // maximum value
                                                        0.5f),             // default value
            std::make_unique<juce::AudioParameterBool> ("invertPhase",      // parameterID
                                                        "Invert Phase",     // parameter name
                                                        false),              // default value
            std::make_unique<juce::AudioParameterFloat>("cutoff_frequency_high", 
                                                        "Cutoff Frequency High", 
                                                        juce::NormalisableRange{20.f, 20000.f, 0.1f, 0.2f, false},
                                                        500.f),
            std::make_unique<juce::AudioParameterFloat>("cutoff_frequency_low", 
                                                        "Cutoff Frequency Low", 
                                                        juce::NormalisableRange{20.f, 20000.f, 0.1f, 0.2f, false},
                                                        500.f)
          })
{
    phaseParameter = parameters.getRawParameterValue ("invertPhase");
    gainParameter = parameters.getRawParameterValue ("gain");
    cutoffFrequencyParameterLow = parameters.getRawParameterValue("cutoff_frequency_low");
    cutoffFrequencyParameterHigh = parameters.getRawParameterValue("cutoff_frequency_high");
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
    previousGain = *gainParameter * phase;

    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumInputChannels();

    convolution.reset();
    convolution.prepare (spec);

    lowFilter.setSamplingRate(static_cast<float>(sampleRate));
    highFilter.setSamplingRate(static_cast<float>(sampleRate));
    
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::dsp::AudioBlock<float> block { buffer };

    auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
    auto currentGain = *gainParameter * phase;

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    convolution.process (juce::dsp::ProcessContextReplacing<float> (block));
    const auto cutoffFrequencyLow = cutoffFrequencyParameterLow->load();
    const auto cutoffFrequencyHigh = cutoffFrequencyParameterHigh->load();

    lowFilter.setCutoffFrequency(cutoffFrequencyLow);
    highFilter.setCutoffFrequency(cutoffFrequencyHigh);
    lowFilter.setHighpass(false);
    highFilter.setHighpass(true);

    lowFilter.processBlock(buffer, midiMessages);
    highFilter.processBlock(buffer, midiMessages);

    if (juce::approximatelyEqual(currentGain, previousGain))
    {
        buffer.applyGain (currentGain);
    }
    else
    {
        buffer.applyGainRamp (0, buffer.getNumChannels(), previousGain, currentGain);
        previousGain = currentGain;
    }
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this, parameters);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
