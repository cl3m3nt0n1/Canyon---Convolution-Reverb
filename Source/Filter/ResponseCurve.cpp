/*
  ==============================================================================

    ResponseCurve.cpp
    Created: 20 Feb 2023 12:03:29pm
    Author:  romai

  ==============================================================================
*/

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "ResponseCurve.h"
#include "../modules/juce_dsp/juce_dsp.h"

//==============================================================================
ResponseCurve::ResponseCurve(ConvolutionReverbAudioProcessor& p): audioProcessor(p)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

ResponseCurve::~ResponseCurve()
{
}

void ResponseCurve::paint (juce::Graphics& g)
{
    
}

void ResponseCurve::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
