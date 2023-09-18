

#pragma once

#include <JuceHeader.h>
#include "../Source/PluginProcessor.h"
#include "MyListBoxModel.h"

class IRDrawer : public juce::Component,
    public juce::FileDragAndDropTarget,
	juce::ChangeListener
    // public juce::Slider::Listener
{
public:
    IRDrawer(ConvolutionReverbAudioProcessor& processor);
    ~IRDrawer() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void loadFile();
    void loadFile(const juce::String& path);
    void loadFile(int row);

    juce::AudioBuffer<float>& getWaveForm() { return waveForm; }

    juce::dsp::Convolution convolutionD;
private:

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    ConvolutionReverbAudioProcessor& proc;

    

    // Made to manage the format of the audio sample (wav, flac, aif...)
    juce::AudioFormatManager formatManager;
    juce::AudioFormatReader* formatReader{ nullptr };

    juce::AudioBuffer<float> waveForm; //Buffer representing the audio waveform of the sample

    juce::Slider irSampleStartSlider;
    juce::Label irSampleStartLabel;
    juce::Slider irSampleStopSlider;
    juce::Label irSampleStopLabel;

    juce::TextButton openButton;
    juce::String fileIsOpen;

    std::vector<float> audioPoints;

    juce::Rectangle<float> sampleViewer;
    juce::Path IRPath;

    juce::Line<float> sampleStartLine;
    juce::Line<float> sampleStopLine;

    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;

    MyListBoxModel listBoxModel;
    juce::ComboBox irComboBox;

    /* TODO :
     * See the work on presetManager
     *
     * Add a previousIRButton, nextIRButton, displayListButton
     *
     */

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IRDrawer)
};
