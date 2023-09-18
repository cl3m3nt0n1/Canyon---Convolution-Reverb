
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Filter/FilterDrawer.h"



//==============================================================================
ConvolutionReverbAudioProcessor::ConvolutionReverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ), apvts(*this,nullptr,ProjectInfo::projectName,createParameterLayout()),presetManager(apvts)
#endif
{

}

ConvolutionReverbAudioProcessor::~ConvolutionReverbAudioProcessor()
{

}

//==============================================================================
const juce::String ConvolutionReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ConvolutionReverbAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ConvolutionReverbAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ConvolutionReverbAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double ConvolutionReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ConvolutionReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int ConvolutionReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ConvolutionReverbAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String ConvolutionReverbAudioProcessor::getProgramName(int index)
{
    return {};
}

void ConvolutionReverbAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void ConvolutionReverbAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    //audioFilter.prepareToPlay(samplesPerBlock, sampleRate);
    //audioFilter.updateFilters(apvts, getSampleRate());

    
   
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;

    convolution.reset();
    convolution.prepare(spec);

    convolutionB.reset();
    convolutionB.prepare(spec);
    
    mixer.prepare(spec);
}

void ConvolutionReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ConvolutionReverbAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void ConvolutionReverbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();




    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    

    //----------------------- FILTERS---------------------------------------
    
    std::vector<juce::String> paramsA{ "LowCut Freq", "HighCut Freq", "Peak Freq", "Peak Gain", "Peak Quality", "LowCut Slope", "HighCut Slope" };

    filter.processBlock(buffer, apvts, getSampleRate(), paramsA);

    std::vector<juce::String> paramsB{ "LowCut FreqB", "HighCut FreqB", "Peak FreqB", "Peak GainB", "Peak QualityB", "LowCut SlopeB", "HighCut SlopeB" };

    filterB.processBlock(buffer, apvts, getSampleRate(), paramsB);


    //dry samples
    juce::dsp::AudioBlock<float> block(buffer);


    float blendProp = apvts.getRawParameterValue("Blend")->load();

    juce::AudioBuffer<float> bufferA;
    bufferA.makeCopyOf(buffer);
    juce::dsp::AudioBlock<float> blockA(bufferA);

    juce::AudioBuffer<float> bufferB;
    bufferB.makeCopyOf(buffer);
    juce::dsp::AudioBlock<float> blockB(bufferB);
 

    mixer.pushDrySamples(block);


    //Filtres A
    auto leftBlock = blockA.getSingleChannelBlock(0);
    auto rightBlock = blockA.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    filter.leftChain.process(leftContext);
    filter.rightChain.process(rightContext);

    //Filtres B
    auto leftBlockB = blockB.getSingleChannelBlock(0);
    auto rightBlockB = blockB.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContextB(leftBlockB);
    juce::dsp::ProcessContextReplacing<float> rightContextB(rightBlockB);

    filterB.leftChain.process(leftContextB);
    filterB.rightChain.process(rightContextB);


    //addBuffers(bufferA, bufferB, blendProp);

    //buffer = bufferB;
    
    //------------------------------ CONVOLUTIONS -------------------------------
   /* juce::dsp::AudioBlock<float> blockConv(buffer);
    
    juce::AudioBuffer<float> bufferConvB;
    bufferConvB.makeCopyOf(buffer);
    juce::dsp::AudioBlock<float> blockConvB(bufferB);*/


    convolution.process(juce::dsp::ProcessContextReplacing<float> (blockA));
   
    convolutionB.process(juce::dsp::ProcessContextReplacing<float>(blockB));

    addBuffers(bufferA, bufferB, blendProp);

    buffer = bufferB;
   
    mixer.mixWetSamples(buffer);

    float dryWet = apvts.getRawParameterValue("Dry Wet")->load();

    mixer.setWetMixProportion(dryWet);

    float gain = apvts.getRawParameterValue("Gain")->load();

    buffer.applyGain(juce::Decibels::decibelsToGain(gain));
    
}

//==============================================================================
bool ConvolutionReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ConvolutionReverbAudioProcessor::createEditor()
{
    return new ConvolutionReverbAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void ConvolutionReverbAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);

}

void ConvolutionReverbAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);

    if (tree.isValid())
    {
        apvts.replaceState(tree);
        //Update filters and other things controlled by apvts
        //audioFilter.updateFilters(apvts, getSampleRate());
    }
}

void ConvolutionReverbAudioProcessor::setIRWaveForm(juce::AudioBuffer<float> waveForm)
{
    this->irWaveForm = waveForm;
}

void ConvolutionReverbAudioProcessor::setIRFile(juce::File file)
{
    this->IRFile = file;
}


juce::AudioProcessorValueTreeState::ParameterLayout
ConvolutionReverbAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("Dry Wet",
                                                            "Dry Wet",
                                                            juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.f),
                                                            1.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Gain",
                                                            "Gain",
                                                            juce::NormalisableRange<float>(-12.f, 12.f, 0.5f, 0.5f),
                                                            0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Blend",
                                                            "Blend",
                                                            juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.f),
                                                            1.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("HighCut Freq",
                                                            "HighCut Freq",
                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                            20000.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("HighCut FreqB",
                                                            "HighCut FreqB",
                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                            20000.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCut Freq",
                                                            "LowCut Freq",
                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                            20.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCut FreqB",
                                                            "LowCut FreqB",
                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                            20.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Freq",
                                                            "Peak Freq",
                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                            750.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak FreqB",
                                                            "Peak FreqB",
                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
                                                            750.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Gain",
                                                            "Peak Gain",
                                                            juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 0.25f),
                                                            0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak GainB",
                                                            "Peak GainB",
                                                            juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 0.25f),
                                                            0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak Quality",
                                                            "Peak Quality",
                                                            juce::NormalisableRange<float>(0.1f, 10.f, 0.1f, 0.5f),
                                                            1.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Peak QualityB",
                                                            "Peak QualityB",
                                                            juce::NormalisableRange<float>(0.1f, 10.f, 0.1f, 0.5f),
                                                            1.f));


    layout.add(std::make_unique<juce::AudioParameterFloat>("Sample Start",
                                                            "Sample Start",
                                                            juce::NormalisableRange<float>(0.f, 100.f, 1.f, 0.25f),
                                                            0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Sample Stop",
                                                            "Sample Stop",
                                                            juce::NormalisableRange<float>(0.f, 100.f, 1.f, 0.25f),
                                                            100.0f));

    

    juce::StringArray stringArray;

    for (int i = 0; i < 4; ++i) {

        juce::String str;
        str << (12 + i * 12);
        str << " db/Oct";
        stringArray.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut Slope", "LowCut Slope", stringArray, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut Slope", "HighCut Slope", stringArray, 0));

    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut SlopeB", "LowCut SlopeB", stringArray, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut SlopeB", "HighCut SlopeB", stringArray, 0));


    return layout;
}


void ConvolutionReverbAudioProcessor::addBuffers(juce::AudioBuffer<float>& source, juce::AudioBuffer<float>& dest, float prop) 
{

    for (int channel = 0; channel < source.getNumChannels(); ++channel) {
       
        const float* buffer1Ptr = source.getReadPointer(channel);
        const float* buffer2Ptr = dest.getReadPointer(channel);

        float* mixed = dest.getWritePointer(channel);

        for (int sample = 0; sample < dest.getNumSamples(); ++sample) {

            mixed[sample] = buffer1Ptr[sample]* (1 - prop) + buffer2Ptr[sample]*prop;
        }
    }

}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ConvolutionReverbAudioProcessor();
}

