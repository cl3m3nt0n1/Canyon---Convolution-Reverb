/*
  ==============================================================================

    FIlter.h
    Created: 19 Jan 2023 2:39:52pm
    Author:  romai

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../modules/juce_dsp/juce_dsp.h"


enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48
};


struct ChainSettings
{
    float peakFreq{ 0 }, peakGainInDecibels{ 0 }, peakQuality{ 1.f };
    float lowCutFreq{ 0 }, highCutFreq{ 0 };

    Slope lowCutSlope{ Slope::Slope_12 }, highCutSlope{ Slope::Slope_12 };
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts, std::vector<juce::String> params);

using Filter = juce::dsp::IIR::Filter<float>;

using Coefficients = Filter::CoefficientsPtr;

inline auto makeLowCutFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return  juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq,
        sampleRate,
        2 * (chainSettings.lowCutSlope + 1));
   
}

inline auto makeHighCutFilter(const ChainSettings& chainSettings, double sampleRate)
{
    return  juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq,
        sampleRate,
        2 * (chainSettings.highCutSlope + 1));

}

Coefficients makePeakFilter(const ChainSettings& chainSettings, double sampleRate);

//==============================================================================

class FIlter {

public:
    FIlter();
    ~FIlter();

    

    void prepareToPlay(double sampleRate, int samplesPerBlock);


    void processBlock(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& apvts, double sampleRate, std::vector<juce::String> params);

    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

    using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;

    MonoChain leftChain, rightChain;

    void updateCoefficients(Coefficients& old, const Coefficients& replacements);


   

    void updatePeakFilter(const ChainSettings& chainSettings, double sampleRate);

    template<int Index, typename ChainType, typename CoefficientType>
    void update(ChainType& chain, const CoefficientType& coefficients)
    {
        updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
        chain.template setBypassed<Index>(false);
    }
    enum ChainPositions {

        LowCut,
        Peak,
        HighCut
    };
    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& chain,
        const CoefficientType& coefficients,
        const Slope& Slope)
    {

        chain.template setBypassed<0>(true);
        chain.template setBypassed<1>(true);
        chain.template setBypassed<2>(true);
        chain.template setBypassed<3>(true);


        switch (Slope)
        {
        case Slope_48:
        {
            update<3>(chain, coefficients);
        }
        case Slope_36:
        {
            update<2>(chain, coefficients);
        }
        case Slope_24:
        {
            update<1>(chain, coefficients);
        }
        case Slope_12:
        {
            update<0>(chain, coefficients);
        }

        }
    }

    void updateFilters(juce::AudioProcessorValueTreeState& apvts, double sampleRate, std::vector<juce::String> params);
    void updateHighCutFilters(const ChainSettings& chainSettings, double sampleRate);
    void updateLowCutFilters(const ChainSettings& chainSettings, double sampleRate);

private:

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FIlter)
};