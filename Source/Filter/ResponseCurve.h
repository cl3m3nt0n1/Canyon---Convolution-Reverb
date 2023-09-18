/*
  ==============================================================================

    ResponseCurve.h
    Created: 20 Feb 2023 12:03:29pm
    Author:  romai

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../modules/juce_dsp/juce_dsp.h"
#include "../PluginProcessor.h"
#include "FIlter.h"

//==============================================================================
/*
*/
class ResponseCurve  : public juce::Component,
    juce::AudioProcessorParameter::Listener
{
public:
    ResponseCurve(ConvolutionReverbAudioProcessor& p);
    ~ResponseCurve() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:


    ConvolutionReverbAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResponseCurve)
};
