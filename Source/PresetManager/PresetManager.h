#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class PresetManager
{
public:
    PresetManager(juce::AudioProcessorValueTreeState& state);
    ~PresetManager();

    static const juce::File defaultDirectory;
    static const juce::String extension;

	void savePreset(juce::String name);
	void deletePreset(juce::String name);
    void loadPreset(juce::String name);

    void loadNextPreset();
    void loadPreviousPreset();

	juce::StringArray getAllPresets();
	juce::String getCurrentPresetName();

private:
    
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::String currentPreset = "";



   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManager)
};
