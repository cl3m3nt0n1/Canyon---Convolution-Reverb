/*
  ==============================================================================

    FIlter.cpp
    Created: 19 Jan 2023 2:39:52pm
    Author:  romai

  ==============================================================================
*/

#include "FIlter.h"
#include <JuceHeader.h>
#include "../modules/juce_dsp/juce_dsp.h"

FIlter::FIlter()
{

}

FIlter::~FIlter()
{

}



void FIlter::prepareToPlay(double sampleRate, int samplesPerBlock)
{

    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;

    spec.numChannels;

    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);



}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts, std::vector<juce::String> params) 
{
    ChainSettings settings;

    /*
    settings.lowCutFreq = apvts.getRawParameterValue("LowCut Freq")->load();
    settings.highCutFreq = apvts.getRawParameterValue("HighCut Freq")->load();
    settings.peakFreq = apvts.getRawParameterValue("Peak Freq")->load();
    settings.peakGainInDecibels = apvts.getRawParameterValue("Peak Gain")->load();
    settings.peakQuality = apvts.getRawParameterValue("Peak Quality")->load();
    settings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("LowCut Slope")->load());
    settings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue("HighCut Slope")->load());
    */

    settings.lowCutFreq = apvts.getRawParameterValue(params[0])->load();
    settings.highCutFreq = apvts.getRawParameterValue(params[1])->load();
    settings.peakFreq = apvts.getRawParameterValue(params[2])->load();
    settings.peakGainInDecibels = apvts.getRawParameterValue(params[3])->load();
    settings.peakQuality = apvts.getRawParameterValue(params[4])->load();
    settings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue(params[5])->load());
    settings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue(params[6])->load());

    return settings;

}

Coefficients makePeakFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return  juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
        chainSettings.peakFreq,
        chainSettings.peakQuality,
        juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibels));
}

void FIlter::updatePeakFilter(const ChainSettings& chainSettings, double sampleRate)
{

    auto peakCoefficients = makePeakFilter(chainSettings, sampleRate);
    updateCoefficients(leftChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
}


void FIlter::updateCoefficients(Coefficients& old, const Coefficients& replacements) 
{
    *old = *replacements;
}


void FIlter::updateFilters(juce::AudioProcessorValueTreeState& apvts, double sampleRate, std::vector<juce::String> params)
{
    auto chainSettings = getChainSettings(apvts, params);

    updateLowCutFilters(chainSettings, sampleRate);
    updatePeakFilter(chainSettings, sampleRate);
    updateHighCutFilters(chainSettings, sampleRate);

}

void FIlter::updateHighCutFilters(const ChainSettings& chainSettings, double sampleRate)
{
    auto highCutCoefficients = makeHighCutFilter(chainSettings, sampleRate);

    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();

    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);
    updateCutFilter(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);

}


void FIlter::updateLowCutFilters(const ChainSettings& chainSettings, double sampleRate)
{
    auto cutCoefficients = makeLowCutFilter(chainSettings, sampleRate);



    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();

    updateCutFilter(leftLowCut, cutCoefficients, chainSettings.lowCutSlope);
    updateCutFilter(rightLowCut, cutCoefficients, chainSettings.lowCutSlope);

}

void FIlter::processBlock(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& apvts, double sampleRate, std::vector<juce::String> params)
{

    updateFilters(apvts, sampleRate, params);  

}





