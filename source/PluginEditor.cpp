#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processorRef (p), valueTreeState(vts)
{
    // Color palette
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colour(0xff444137));
    getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff444137));
    getLookAndFeel().setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff9f937d));
    getLookAndFeel().setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xff444137));
    getLookAndFeel().setColour(juce::Label::textColourId, juce::Colour(0xff444137));
    getLookAndFeel().setColour(juce::ToggleButton::textColourId, juce::Colour(0xff444137));
    getLookAndFeel().setColour(juce::ToggleButton::tickColourId, juce::Colour(0xff444137));
    getLookAndFeel().setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(0xff444137));
    getLookAndFeel().setColour(juce::TextButton::buttonColourId, juce::Colour(0xff444137));
    getLookAndFeel().setColour(juce::TextButton::textColourOnId, juce::Colour(0xffE0D7C6));

    // Gain Slider and label
    addAndMakeVisible (gainSlider);
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    gainAttachment.reset(new SliderAttachment (valueTreeState, "gain", gainSlider));

    addAndMakeVisible (gainLabel);
    gainLabel.setText("Gain", juce::dontSendNotification);

    // Phase invert toggle 
    addAndMakeVisible (invertButton);
    invertButton.setButtonText ("Invert Phase");
    invertAttachment.reset (new ButtonAttachment (valueTreeState, "invertPhase", invertButton));

    // Cutoff low frequency slider and label + attachment
    addAndMakeVisible(cutoffFrequencySliderLow);
    cutoffFrequencySliderLow.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    cutoffFrequencyAttachmentLow.reset(new SliderAttachment(vts, "cutoff_frequency_low", cutoffFrequencySliderLow));

    addAndMakeVisible(cutoffFrequencyLabelLow);
    cutoffFrequencyLabelLow.setText("Cutoff Frequency Low", juce::dontSendNotification);

    // Cutoff high frequency slider and label + attachment
    addAndMakeVisible(cutoffFrequencySliderHigh);
    cutoffFrequencySliderHigh.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    cutoffFrequencyAttachmentHigh.reset(new SliderAttachment(vts, "cutoff_frequency_high", cutoffFrequencySliderHigh));

    addAndMakeVisible(cutoffFrequencyLabelHigh);
    cutoffFrequencyLabelHigh.setText("Cutoff Frequency High", juce::dontSendNotification);

    processorRef.root = juce::File::getSpecialLocation (juce::File::userDesktopDirectory);

    // Open file button for convolution reverb
    addAndMakeVisible (openButton);
    openButton.setButtonText ("Open IR File");
    openButton.onClick = [this] { openButtonClicked(); };

    formatManager.registerBasicFormats();

    addAndMakeVisible (irName);

    setSize (5 * paramWidth, paramHeight);

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
    g.fillAll (juce::Colour(0xffE0D7C6));

    g.setColour(juce::Colour(0xff444137));
    g.setFont(15.0f);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    //gainSlider.setBounds (40, 30, 20, getHeight() - 60);

    auto r = getLocalBounds();

    // auto gainRect = r.removeFromLeft (paramWidth);
    // invertButton.setBounds(gainRect.removeFromBottom(buttonHeight));
    // gainLabel.setBounds (gainRect.removeFromBottom (buttonHeight));
    // gainSlider.setBounds (gainRect);

    // auto lowFilterRect = r. removeFromLeft(paramWidth);
    // cutoffFrequencyLabelLow.setBounds(lowFilterRect.removeFromBottom(buttonHeight));
    // cutoffFrequencySliderLow.setBounds(lowFilterRect);

    // auto highFilterRect = r. removeFromLeft(paramWidth);
    // cutoffFrequencyLabelHigh.setBounds(highFilterRect.removeFromBottom(buttonHeight));
    // cutoffFrequencySliderHigh.setBounds(highFilterRect);

    // auto reverbRect = r.removeFromLeft (paramWidth);
    // openButton.setBounds (reverbRect.removeFromBottom (200));
    // irName.setBounds (reverbRect.removeFromBottom (40));

    const auto openButtonX = getWidth() * JUCE_LIVE_CONSTANT(0.075);
    const auto openButtonY = getHeight() * JUCE_LIVE_CONSTANT(0.34);
    const auto openButtonWidth = getWidth() * JUCE_LIVE_CONSTANT(0.2);
    const auto openButtonHeight = getHeight() * JUCE_LIVE_CONSTANT(0.3);

    const auto irNameYOffset = JUCE_LIVE_CONSTANT(30);

    openButton.setBounds(openButtonX, openButtonY, openButtonWidth, openButtonHeight);
    irName.setBounds(openButtonX, openButtonY + irNameYOffset, openButtonWidth, openButtonHeight + irNameYOffset + 50);

    // auto knobRect = r.removeFromBottom (paramHeight);
    auto knobRect = r;
    
    auto gainRect = knobRect.removeFromRight (paramWidth);
    gainSlider.setBounds (gainRect);
    gainLabel.setBounds (gainRect.removeFromBottom (buttonHeight));

    auto highFilterRect = knobRect. removeFromRight(paramWidth);
    cutoffFrequencySliderHigh.setBounds(highFilterRect);
    cutoffFrequencyLabelHigh.setBounds(highFilterRect.removeFromBottom(buttonHeight));

    auto lowFilterRect = knobRect. removeFromRight(paramWidth);
    cutoffFrequencySliderLow.setBounds(lowFilterRect);
    cutoffFrequencyLabelLow.setBounds(lowFilterRect.removeFromBottom(buttonHeight));

    invertButton.setBounds(knobRect.removeFromRight(JUCE_LIVE_CONSTANT(80)));
}
