#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "melatonin_inspector/melatonin_inspector.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

//==============================================================================
class PluginEditor :    public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (PluginProcessor&);
    PluginEditor (PluginProcessor& p, juce::AudioProcessorValueTreeState& vts); // important, added
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    enum {
        paramControlHeight = 40,
        paramLabelWidth = 80,
        paramSliderWidth = 300
    };

private:
    //void sliderValueChanged (juce::Slider* slider) override;
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::Slider gainSlider;
    juce::Label gainLabel;
    std::unique_ptr<SliderAttachment> gainAttachment;

    juce::ToggleButton invertButton;
    std::unique_ptr<ButtonAttachment> invertAttachment;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
