/*
  ==============================================================================

    FilterDrawer.cpp
    Created: 16 Feb 2023 10:06:11am
    Author:  romai

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FilterDrawer.h"
#include "../modules/juce_dsp/juce_dsp.h"

//==============================================================================
FilterDrawer::FilterDrawer(ConvolutionReverbAudioProcessor& p, std::vector<juce::String> apvts_params) : audioProcessor(p),
    lowCutFreqSliderAttachement(audioProcessor.apvts, apvts_params[0], lowCutFreqSlider),
    highCutFreqSliderAttachement(audioProcessor.apvts, apvts_params[1], highCutFreqSlider),
    peakGainSliderAttachement(audioProcessor.apvts, apvts_params[2], peakGainSlider),
    peakFreqSliderAttachement(audioProcessor.apvts, apvts_params[3], peakFreqSlider),
    peakQualitySliderAttachement(audioProcessor.apvts, apvts_params[4], peakQualitySlider),
    highCutSlopeSliderAttachement(audioProcessor.apvts, apvts_params[5], highCutSlopeSlider),
    lowCutSlopeSliderAttachement(audioProcessor.apvts, apvts_params[6], lowCutSlopeSlider)
{

    apvts_para = apvts_params;

    setSize(365, 315);
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible(lowCutFreqSlider);
    addAndMakeVisible(lowCutSlopeSlider);
    addAndMakeVisible(highCutSlopeSlider);
    addAndMakeVisible(highCutFreqSlider);
    addAndMakeVisible(peakGainSlider);
    addAndMakeVisible(peakFreqSlider);
    addAndMakeVisible(peakQualitySlider);

    peakFreqLabel.attachToComponent(&peakFreqSlider, false);
    peakFreqLabel.setText("Peak Freq", juce::NotificationType::dontSendNotification);
    peakFreqLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    peakFreqLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&peakFreqLabel);

    const auto& params = audioProcessor.getParameters();

    for (auto param : params)
    {
        param->addListener(this);
    }

    startTimerHz(60);

    parametersChanged.set(true);
    
}

FilterDrawer::~FilterDrawer()
{
    const auto& params = audioProcessor.getParameters();
    for (auto param : params)
    {

        param->removeListener(this);
    }
}

void FilterDrawer::parameterValueChanged(int parameterIndex, float newValue)
{

    parametersChanged.set(true);    
}

void FilterDrawer::timerCallback()
{
    if (parametersChanged.compareAndSetBool(false, true))
    {

   

        //update monochain
        auto chainSettings = getChainSettings(audioProcessor.apvts, apvts_para);
        auto LowCutCoefficients = makeLowCutFilter(chainSettings, audioProcessor.getSampleRate());
        auto HighCutCoefficients = makeHighCutFilter(chainSettings, audioProcessor.getSampleRate());
        
        audioProcessor.filter.updateCutFilter(monoChain.get<FIlter::ChainPositions::LowCut>(), LowCutCoefficients, chainSettings.lowCutSlope);
        audioProcessor.filter.updateCutFilter(monoChain.get<FIlter::ChainPositions::HighCut>(), HighCutCoefficients, chainSettings.highCutSlope);


        auto peakCoefficients = makePeakFilter(chainSettings, audioProcessor.getSampleRate());
        audioProcessor.filter.updateCoefficients(monoChain.get<FIlter::ChainPositions::Peak>().coefficients, peakCoefficients);
        //signal a repaint
        repaint();
    }
}

void FilterDrawer::paint (juce::Graphics& g)
{
   
   
    g.setColour(juce::Colours::black);
    g.fillRect(filterRect);
    g.drawRect(filterRect);

    //Dessin du filtre avec les param�tres du APVTS

    using namespace juce;

    auto w = filterRect.getWidth();

    auto& lowcut = monoChain.get<FIlter::ChainPositions::LowCut>();
    auto& peak = monoChain.get<FIlter::ChainPositions::Peak>();
    auto& highcut = monoChain.get<FIlter::ChainPositions::HighCut>();

    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> mags;

    mags.resize(w);

    for (int i = 0; i < w; ++i)
    {
        double mag = 1.f;
        auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);

        if (!monoChain.isBypassed<FIlter::ChainPositions::Peak>())
            mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!lowcut.isBypassed<0>())
            mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowcut.isBypassed<1>())
            mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowcut.isBypassed<2>())
            mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowcut.isBypassed<3>())
            mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!highcut.isBypassed<0>())
            mag *= highcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highcut.isBypassed<1>())
            mag *= highcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highcut.isBypassed<2>())
            mag *= highcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highcut.isBypassed<3>())
            mag *= highcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        auto magniGain = Decibels::gainToDecibels(mag);
        if (magniGain < -24.0f) {
            mags[i] = -24.0f;
        }
        else {
            mags[i] = magniGain;
        }
        

    }


    Path  responseCurve;


    const double outputMin = filterRect.getBottom();
    const double outputMax = filterRect.getY();

    auto map = [outputMin, outputMax](double input)
    {
        return jmap(input, -24.0, 24.0, outputMin, outputMax);
    };

    responseCurve.startNewSubPath(filterRect.getX(), map(mags.front()));

    for (size_t i = 1; i < mags.size(); ++i)
    {
        responseCurve.lineTo(filterRect.getX() + i, map(mags[i]));
    }



    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.f));

    g.setColour(juce::Colours::black);
    g.fillRect(rectHidding);
    g.drawRect(rectHidding);
}

void FilterDrawer::resized()
{
    //========
    lowCutFreqSlider.setBounds(20, 300, 60, 60);
    lowCutSlopeSlider.setBounds(20, 240, 60, 60);

    //========
    highCutFreqSlider.setBounds(280, 300, 60, 60);
    highCutSlopeSlider.setBounds(280, 240, 60, 60);

    //========
    peakGainSlider.setBounds(120, 300, 60, 60);
    peakFreqSlider.setBounds(150, 240, 60, 60);
    peakQualitySlider.setBounds(190, 300, 60, 60);

    peakFreqLabel.setBounds(215, 290, 70, 30);
    

    filterRect.setBounds(23, 70, 332, 155);

    rectHidding.setBounds(23, 224, 332, 2);

    repaint();
}

std::vector<juce::Component*> FilterDrawer::getComps()
{
    return std::vector<juce::Component*>();
}
