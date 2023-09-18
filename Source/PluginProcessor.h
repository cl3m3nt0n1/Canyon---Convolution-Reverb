
#pragma once

#include <JuceHeader.h>
#include "Filter/FIlter.h"
#include "../modules/juce_dsp/juce_dsp.h"
#include "PresetManager/PresetManager.h"

//==============================================================================
/**
*/
class ConvolutionReverbAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    , public juce::AudioProcessorARAExtension
#endif
{
public:
    //==============================================================================
    ConvolutionReverbAudioProcessor();
    ~ConvolutionReverbAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts{ *this, nullptr,"Parameters", createParameterLayout() };

    FIlter filter;
    FIlter filterB;

    void setIRWaveForm(juce::AudioBuffer<float> waveForm);
    void setIRFile(juce::File file);
    juce::File getIRFile() { return IRFile; }   

    juce::AudioBuffer<float> getIRWaveForm() { return irWaveForm; }

    // Convolution
    int AB = 1;

    juce::dsp::Convolution convolution;
    juce::dsp::Convolution convolutionB;

    juce::dsp::ProcessSpec spec;

    // ajout de deux buffers

    void addBuffers(juce::AudioBuffer<float>& source, juce::AudioBuffer<float>& dest, float prop);
    PresetManager& getPresetManager() { return presetManager; }

private:
    //==============================================================================

    //Convolution
    juce::AudioBuffer<float> irWaveForm; //Buffer representing the audio waveform of the IR
    juce::File IRFile;

    juce::dsp::DryWetMixer<float> mixer;


    PresetManager presetManager;

  
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConvolutionReverbAudioProcessor)
};
