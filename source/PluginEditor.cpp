#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processorRef (p), valueTreeState(vts)
{

    // Gain Slider and label
    addAndMakeVisible (gainSlider);
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    gainAttachment.reset(new SliderAttachment (valueTreeState, "gain", gainSlider));

    addAndMakeVisible (gainLabel);
    gainLabel.setText("Gain", juce::dontSendNotification);

    // Phase invert toggle 
    addAndMakeVisible (invertButton);
    invertButton.setButtonText ("Invert Phase");
    invertAttachment.reset (new ButtonAttachment (valueTreeState, "invertPhase", invertButton));

    // Cutoff frequency slider and label + attachment
    addAndMakeVisible(cutoffFrequencySlider);
    cutoffFrequencySlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    cutoffFrequencyAttachment
    .reset(new SliderAttachment(vts, "cutoff_frequency", cutoffFrequencySlider));

    addAndMakeVisible(cutoffFrequencyLabel);
    cutoffFrequencyLabel.setText("Cutoff Frequency", juce::dontSendNotification);

    // Highpass toggle button, label, attachment
    addAndMakeVisible(highpassButton);
    highpassButton.setButtonText ("Highpass");
    highpassAttachment.reset(new ButtonAttachment(vts, "highpass", highpassButton));

    addAndMakeVisible(highpassButtonLabel);
    highpassButtonLabel.setText("Highpass", juce::dontSendNotification);

    processorRef.root = juce::File::getSpecialLocation (juce::File::userDesktopDirectory);

    // Open file button for convolution reverb
    addAndMakeVisible (openButton);
    openButton.setButtonText ("Open IR File");
    openButton.onClick = [this] { openButtonClicked(); };

    formatManager.registerBasicFormats();

    addAndMakeVisible (irName);

    setSize (3 * paramWidth, paramHeight);

}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::openButtonClicked()
{
    DBG ("clicked");
    // choose a file
    chooser = std::make_unique<juce::FileChooser> ("Choose a WAV or AIFF File", processorRef.root, "*.wav;*.aiff;*.mp3", true, false, nullptr);


    // if user chooses a file
    chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& fc) {
            auto file = fc.getResult();
            DBG ("getting to here?");
            if (file.existsAsFile())
            {
                DBG ("successful, file exists");
                //processor.loadImpulseResponse (file);
                processorRef.savedFile = file;
                processorRef.root = file.getParentDirectory().getFullPathName(); // Change default open directory
                processorRef.convolution.reset();
                processorRef.convolution.loadImpulseResponse (file, juce::dsp::Convolution::Stereo::yes, juce::dsp::Convolution::Trim::yes, 0);
                irName.setText (file.getFileName(), juce::dontSendNotification);
            }
        });
}


void PluginEditor::paint (juce::Graphics& g)
{

    /*g.fillAll (juce::Colours::white);
    g.setColour (juce::Colours::black);
    g.setFont (16.0f);*/
    //g.drawFittedText ("Gain", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    //gainSlider.setBounds (40, 30, 20, getHeight() - 60);

    auto r = getLocalBounds();

    auto gainRect = r.removeFromLeft (paramWidth);
    invertButton.setBounds(gainRect.removeFromBottom(buttonHeight));
    gainLabel.setBounds (gainRect.removeFromBottom (buttonHeight));
    gainSlider.setBounds (gainRect);

    auto filterRect = r. removeFromLeft(paramWidth);
    highpassButton.setBounds(filterRect.removeFromBottom(buttonHeight));
    cutoffFrequencyLabel.setBounds(filterRect.removeFromBottom(buttonHeight));
    cutoffFrequencySlider.setBounds(filterRect);

    auto reverbRect = r.removeFromLeft (paramWidth);
    openButton.setBounds (reverbRect.removeFromBottom (200));
    irName.setBounds (reverbRect.removeFromBottom (40));



}
