/*
  ==============================================================================

    FilterDrawer.h
    Created: 16 Feb 2023 10:06:11am
    Author:  romai

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "FIlter.h"
#include "../modules/juce_dsp/juce_dsp.h"


struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {


    }
};
//==============================================================================
/*
*/
class FilterDrawer  : public juce::Component, 
    juce::AudioProcessorParameter::Listener,
    juce::Timer
{
public:
    FilterDrawer(ConvolutionReverbAudioProcessor& p, std::vector<juce::String> params);
    ~FilterDrawer() override;

    void parameterValueChanged(int parameterIndex, float newValue) override;

    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override { }

    virtual void timerCallback() override;


    void paint (juce::Graphics&) override;
    void resized() override;

private:


    juce::Rectangle<float> filterRect;

    ConvolutionReverbAudioProcessor& audioProcessor;

    juce::Atomic<bool> parametersChanged{ false }; 

    CustomRotarySlider  peakFreqSlider,
                        peakGainSlider,
                        peakQualitySlider,
                        lowCutFreqSlider,
                        highCutFreqSlider,
                        lowCutSlopeSlider,
                        highCutSlopeSlider;

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachement = APVTS::SliderAttachment;

    juce::Label peakFreqLabel,
                peakGainLabel,
                peakQualityLabel,
                lowCutFreqLabel,
                highCutFreqLabel,
                lowCutSlopeLabel,
                highCutSlopeLabel;


    Attachement peakFreqSliderAttachement,
        peakGainSliderAttachement,
        peakQualitySliderAttachement,
        lowCutFreqSliderAttachement,
        highCutFreqSliderAttachement,
        lowCutSlopeSliderAttachement,
        highCutSlopeSliderAttachement;

    std::vector<juce::Component*> getComps();

    FIlter::MonoChain monoChain;

    std::vector<juce::String> apvts_para;

    juce::Rectangle<float> rectHidding;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterDrawer)
};
