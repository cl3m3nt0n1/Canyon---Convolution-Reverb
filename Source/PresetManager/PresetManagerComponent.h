
#pragma once

#include <JuceHeader.h>
#include "PresetManager.h"


class PresetManagerComponent  : public juce::Component, juce::ComboBox::Listener, juce::Button::Listener
{
public:
    PresetManagerComponent(PresetManager& presetManager);
    ~PresetManagerComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    PresetManager& presetManager;
    juce::ComboBox presetSelector;
    juce::StringArray presetNames;

	juce::TextButton saveButton,
					 deleteButton,
					 previousPresetButton,
					 nextPresetButton;

	juce::Rectangle<float> presetSelectorArea;

    std::unique_ptr<juce::FileChooser> fileChooser;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManagerComponent)
};
