#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    gainSlider.setSliderStyle (juce::Slider::LinearBarVertical);
    gainSlider.setRange (0.0, 1.0, 0.1);
    gainSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    gainSlider.setTextValueSuffix (" Gain");
    gainSlider.setValue(0.5);
    gainSlider.addListener (this);

    addAndMakeVisible (&gainSlider);

}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::sliderValueChanged(juce::Slider* slider)
{
    processorRef.gain->operator=(gainSlider.getValue());
    //PluginProcessor.parameters.getParameter
}

void PluginEditor::paint (juce::Graphics& g)
{

    g.fillAll (juce::Colours::white);
    g.setColour (juce::Colours::black);
    g.setFont (16.0f);
    g.drawFittedText ("Gain", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    gainSlider.setBounds (40, 30, 20, getHeight() - 60);
}
