
#include <JuceHeader.h>
#include "PresetManagerComponent.h"



//==============================================================================
PresetManagerComponent::PresetManagerComponent(PresetManager& presetManager) : presetManager(presetManager)
{
     setSize(400,100);

    // presetSelectorArea.setCentre(180,180);

    saveButton.setButtonText("Save");
    saveButton.setEnabled(true);
    addAndMakeVisible(saveButton);

    deleteButton.setButtonText("Delete");
    deleteButton.setEnabled(true);
    addAndMakeVisible(deleteButton);

    previousPresetButton.setButtonText("<");
    previousPresetButton.setEnabled(true);
    addAndMakeVisible(previousPresetButton);

    nextPresetButton.setButtonText(">");
    nextPresetButton.setEnabled(true);
    addAndMakeVisible(nextPresetButton);

    presetSelector.setTextWhenNoChoicesAvailable("No Presets Loaded");
    presetSelector.setMouseCursor(juce::MouseCursor::PointingHandCursor);
	presetSelector.addListener(this);
    addAndMakeVisible(presetSelector);

    presetNames = presetManager.getAllPresets();
    const auto currentPeset = presetManager.getCurrentPresetName();
    presetSelector.addItemList(presetNames, 1);
    presetSelector.setSelectedItemIndex(presetNames.indexOf(currentPeset), juce::dontSendNotification);
    
}

PresetManagerComponent::~PresetManagerComponent()
{
    
}

void PresetManagerComponent::paint (juce::Graphics& g)
{
    g.setColour(juce::Colour::fromFloatRGBA(120,120,120,60));
    g.drawRect(presetSelectorArea);
    g.setColour(juce::Colour::fromFloatRGBA(0,0,0,60));
    g.fillRect(presetSelectorArea);

}

void PresetManagerComponent::resized()
{
    // presetSelectorArea.setBounds(getBoundsInParent().getTopLeft().getX(), getBoundsInParent().getTopLeft().getY(), getWidth(), 30);
	presetSelectorArea.setBounds(getBoundsInParent().getCentreX(),getBoundsInParent().getCentreY(),200,100);

	deleteButton.setBounds(presetSelectorArea.getBottomLeft().getX() + 10, presetSelectorArea.getHeight(), 50, 20);
    saveButton.setBounds(deleteButton.getWidth() + 10, deleteButton.getHeight(), 50, 20);
	/*previousPresetButton.setBounds(saveButton.getWidth() + 10, saveButton.getHeight(), 50, 20);
    nextPresetButton.setBounds(previousPresetButton.getWidth() + 10, previousPresetButton.getHeight(), 50, 20);

    presetSelector.setBounds(presetSelectorArea.getWidth() - 50, nextPresetButton.getHeight(), 100, 30);
    */

}

void PresetManagerComponent::buttonClicked(juce::Button* button)
{
    if(button == &saveButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Please enter the name of the preset you want to save",
            PresetManager::defaultDirectory,
            "*." + PresetManager::extension);

        fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser)
        {
	        const auto result = chooser.getResult();
            presetManager.savePreset(result.getFileNameWithoutExtension());
        });
    }
    if(button == &previousPresetButton)
    {
	    presetManager.loadPreviousPreset();
    }
    if(button == &nextPresetButton)
    {
        presetManager.loadNextPreset();
	}
    if(button == &deleteButton)
    {
	    
        int result = juce::AlertWindow::showOkCancelBox(juce::AlertWindow::AlertIconType::WarningIcon,
            										   "Delete preset",
            										   "Are you sure you want to delete this preset?",
            										   "Yes",
            										   "No",
            										   this);
    	if(result == 1)
        {
			presetManager.deletePreset(presetManager.getCurrentPresetName());
        }
        else 
            return;
			
	/*
	 * presetManager.deletePreset(presetManager.getCurrentPresetName());
	 */

    }
    
    
}

void PresetManagerComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if(comboBoxThatHasChanged == &presetSelector)
    {
	    presetManager.loadPreset(presetSelector.getItemText(presetSelector.getSelectedItemIndex()));
    }
}
