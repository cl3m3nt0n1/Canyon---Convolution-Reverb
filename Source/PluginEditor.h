
#pragma once

#include <JuceHeader.h>
#include "IRDrawer/IRDrawer.h"
#include "PluginProcessor.h"
#include "Filter/FilterDrawer.h"
#include "PresetManager/PresetManagerComponent.h"



//==============================================================================

class CustomLNF : public juce::LookAndFeel_V4 {

public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override;
};

//==============================================================================
class ConvolutionReverbAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Button::Listener
{
public:
    ConvolutionReverbAudioProcessorEditor(ConvolutionReverbAudioProcessor&);
    ~ConvolutionReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    void presetListChanged() const;

    void buttonClicked(juce::Button*) override;

    
    int AB = 0;
    //==============================================================================

private:
    // Vu meter
    juce::Rectangle<float> vuRect;
    juce::Path vuPath;

    //Custom look and feel
    CustomLNF NLNF;
    juce::Image background;

    ConvolutionReverbAudioProcessor& audioProcessor;
    //Couleurs customs
    juce::Array<juce::Colour> colours{ juce::Colour::fromRGB(217,217,217) };

    // Elements du Header
    //TODO
    // PresetManagerComponent presetManagerComponent;

    juce::ToggleButton abToggle;


    //Elements sans Rectangle
    juce::Slider dryWetSlider;
    juce::Label dryWetLabel;

    juce::Slider gainSlider;
    juce::Label gainLabel;

    //Elements du filtre

    FilterDrawer filterDrawer;
    std::vector<juce::String> paramsA{ "LowCut Freq", "HighCut Freq", "Peak Freq", "Peak Gain", "Peak Quality", "LowCut Slope", "HighCut Slope" };

    FilterDrawer filterDrawerB;
    std::vector<juce::String> paramsB{ "LowCut FreqB", "HighCut FreqB", "Peak FreqB", "Peak GainB", "Peak QualityB", "LowCut SlopeB", "HighCut SlopeB" };


    juce::Rectangle<float> filterRect;
    juce::Path rectPath;
    

    //Elements du blend
    juce::Slider blendSlider;

    juce::Rectangle<float> bottomRect;
    juce::Path bottomPath;

    juce::TextButton AB_button{"A | b"};
    

    juce::ColourGradient gradient1;

    juce::ColourGradient gradient2;


    // APVTS Connection 

    juce::Atomic<bool> parametersChanged{ false };

    using APVTS = juce::AudioProcessorValueTreeState;

    using Attachement = APVTS::SliderAttachment;


    Attachement dryWetSliderAttachement,
                blendSliderAttachement,
                gainSliderAttachement;
    

    IRDrawer drawer;
    IRDrawer drawerB;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConvolutionReverbAudioProcessorEditor)
};
