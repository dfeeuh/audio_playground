#include "MainComponent.h"
#include "Modules.hpp"

//==============================================================================
MainComponent::MainComponent()
{
    setOpaque(true);
    setSize (600, 400);

    unsigned nChannels = 2;
    unsigned sampleRate = 44100;
    unsigned frameSize = 512;

    sine = std::make_shared<SineWaveform>(nChannels);
    gain = std::make_shared<Gain>(nChannels, 0.1f);
    
    sine->connect(gain);

    audio = std::make_unique<AudioEngine>(nChannels, sampleRate, frameSize);
    if (audio != nullptr)
    {
        audio->connect(sine);

        if (audio->start() != 0)
        {
            juce::Logger::outputDebugString("Failed to start...");
        }   
    }

    // Add GUI elements here
    addAndMakeVisible (gainSlider);
    gainSlider.setRange (0.0, 1.0, 0.1);
    gainSlider.setPopupMenuEnabled (true);

    gainSlider.setSliderStyle (juce::Slider::LinearVertical);
    gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 20);

    juce::Rectangle<int> layoutArea { 20, 20, 580, 430 };
    auto sliderArea = layoutArea.removeFromTop (320);

    gainSlider.setBounds (sliderArea.removeFromLeft (70));
    gainSlider.setDoubleClickReturnValue (true, 50.0); // double-clicking this gainSlider will set it to 50.0
    gainSlider.setTextValueSuffix (" units");

    gainSlider.onValueChange = [this] { gain->update((AUDIO_FORMAT_TYPE)gainSlider.getValue()); };
    gainSlider.setValue (0);
    gain->update((AUDIO_FORMAT_TYPE)gainSlider.getValue());
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::lightgrey);

    // g.setFont (juce::Font (16.0f));
    // g.setColour (juce::Colours::white);
    // g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    // sliders.setBounds(getLocalBounds().reduced(4));
}
