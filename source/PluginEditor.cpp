#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processorRef (p), valueTreeState(vts)
{
    gainLabel.setText ("Gain", juce::dontSendNotification);
    addAndMakeVisible (gainLabel);

    addAndMakeVisible (gainSlider);
    gainAttachment.reset (new SliderAttachment (valueTreeState, "gain", gainSlider));

    invertButton.setButtonText ("Invert Phase");
    addAndMakeVisible (invertButton);
    invertAttachment.reset (new ButtonAttachment (valueTreeState, "invertPhase", invertButton));

    setSize (paramSliderWidth + paramLabelWidth, juce::jmax (100, paramControlHeight * 2));

}

PluginEditor::~PluginEditor()
{
}


void PluginEditor::paint (juce::Graphics& g)
{

    /*g.fillAll (juce::Colours::white);
    g.setColour (juce::Colours::black);
    g.setFont (16.0f);*/
    //g.drawFittedText ("Gain", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    //gainSlider.setBounds (40, 30, 20, getHeight() - 60);

    auto r = getLocalBounds();

    auto gainRect = r.removeFromTop (paramControlHeight);
    gainLabel.setBounds (gainRect.removeFromLeft (paramLabelWidth));
    gainSlider.setBounds (gainRect);

    invertButton.setBounds (r.removeFromTop (paramControlHeight));


}
