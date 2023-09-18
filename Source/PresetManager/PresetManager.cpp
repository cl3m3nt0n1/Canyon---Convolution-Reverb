
#include <JuceHeader.h>
#include "PresetManager.h"


const juce::File PresetManager::defaultDirectory{
	juce::File::getSpecialLocation(juce::File::SpecialLocationType::commonDocumentsDirectory)
			.getChildFile(ProjectInfo::companyName)
			.getChildFile(ProjectInfo::projectName)
	};

const juce::String PresetManager::extension{ "preset" };

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts) : valueTreeState(apvts)
{
    // Create a default Directory, if it doesn't exits
	if(!defaultDirectory.exists())
	{
		const auto result = defaultDirectory.createDirectory();
		if(result.failed())
		{
			DBG("Error creating presets :" + result.getErrorMessage());
			jassertfalse;
		}
	}
}

PresetManager::~PresetManager()
{
}

void PresetManager::savePreset(juce::String name)
{
	if(name.isEmpty())
		return;
	const auto xml = valueTreeState.copyState().createXml();
	const auto fileToWrite = defaultDirectory.getChildFile(name + '.' + extension);
	if(!xml->writeTo(fileToWrite))
	{
		DBG("Error saving preset : " + fileToWrite.getFullPathName());
		jassertfalse;
	}
	currentPreset = name;
}

void PresetManager::deletePreset(juce::String name) 
{
	// If no name was provided
	if(name.isEmpty())
		return;

	// If file doesn't exists
	const auto presetFile = defaultDirectory.getChildFile(name + '.' + extension);
	if(!presetFile.existsAsFile())
	{
		DBG("Preset File " + presetFile.getFullPathName() + "does not exists");
		jassertfalse;
	}

	// If file does exists
	if(!presetFile.deleteFile())
	{
		DBG("Error deleting preset : " + name + '.' + extension);
		jassertfalse;
		return;
	}
	currentPreset = "";
}

void PresetManager::loadPreset(juce::String name) 
{
	if (name.isEmpty())
			return;

	const auto presetFile = defaultDirectory.getChildFile(name + "." + extension);
	if (!presetFile.existsAsFile())
	{
		DBG("Preset file " + presetFile.getFullPathName() + " does not exist");
		jassertfalse;
		return;
	}

	juce::XmlDocument xml { presetFile };
	const juce::ValueTree valueTreeToLoad = juce::ValueTree::fromXml(*xml.getDocumentElement());

	valueTreeState.replaceState(valueTreeToLoad); //Loading state from XML file

	currentPreset = name;

}

void PresetManager::loadNextPreset()
{
	const auto allPresets = getAllPresets();
	if(allPresets.isEmpty())
		return;

	const auto currentIndex = allPresets.indexOf(currentPreset);
	const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;
	loadPreset(allPresets.getReference(nextIndex));

}

void PresetManager::loadPreviousPreset()
{
    const auto allPresets = getAllPresets();
	if(allPresets.isEmpty())
		return;

	const auto currentIndex = allPresets.indexOf(currentPreset);
	const auto previousIndex = currentIndex -1 <0 ? allPresets.size() - 1 : currentIndex - 1;
	loadPreset(allPresets.getReference(previousIndex));
}

juce::StringArray PresetManager::getAllPresets() 
{
	juce::StringArray presets;
	const auto fileArray = defaultDirectory.findChildFiles(
		juce::File::TypesOfFileToFind::findFiles, false,"*." + extension
	);

	for (const auto& file : fileArray)
	{
		presets.add(file.getFileNameWithoutExtension());
	}
	return presets;

}

juce::String PresetManager::getCurrentPresetName() 
{
	return currentPreset;
}

