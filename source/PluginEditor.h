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
        paramWidth = 200,
        paramHeight = 500,
        buttonHeight = 50
    };

private:
    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::Slider gainSlider;
    juce::Label gainLabel;
    std::unique_ptr<SliderAttachment> gainAttachment;

    juce::ToggleButton invertButton;
    std::unique_ptr<ButtonAttachment> invertAttachment;

    juce::Slider cutoffFrequencySliderLow;
    std::unique_ptr<SliderAttachment> cutoffFrequencyAttachmentLow;
    juce::Label cutoffFrequencyLabelLow;

    juce::Slider cutoffFrequencySliderHigh;
    std::unique_ptr<SliderAttachment> cutoffFrequencyAttachmentHigh;
    juce::Label cutoffFrequencyLabelHigh;

    juce::Image MSBVerb;
    juce::Image IEEELogo;
    juce::ImageComponent MSBVerbComponent;
    juce::ImageComponent IEEELogoComponent;

    void openButtonClicked();
    juce::TextButton openButton;
   
    juce::AudioFormatManager formatManager; 
    std::unique_ptr<juce::FileChooser> chooser;

    juce::Label irName;

    PluginProcessor& processorRef;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};