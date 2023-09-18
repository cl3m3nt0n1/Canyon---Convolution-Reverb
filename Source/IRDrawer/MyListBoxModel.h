
#pragma once

#include <JuceHeader.h>


class MyListBoxModel : public juce::ListBoxModel {
public:

    MyListBoxModel(const juce::File& directoryToLoad)
    {
        if (!directoryToLoad.getParentDirectory().exists())
        {
            const auto result = directoryToLoad.getParentDirectory().createDirectory();
            if (result.failed())
            {
                DBG("Error Creating IR Directory : " + result.getErrorMessage());
                jassertfalse;
            }

        }
        else
            if (!directoryToLoad.exists())
            {
                const auto result = directoryToLoad.createDirectory();
                if (result.failed())
                {
                    DBG("Error Creating IR Directory : " + result.getErrorMessage());
                    jassertfalse;
                }

            }
        directoryToLoad.findChildFiles(files, juce::File::findFiles, true);

        for (const auto& file : files)
        {
            fileNames.add(file.getFileNameWithoutExtension());
        }
    }

    int getNumRows() override {
        return files.size(); // Return the number of rows in the list
    }

    

    void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override
    {
        auto selectedFile = getFile(row);

        //Load file.

    }

    juce::File getFile(int row) const
    {
        return files[row];
    }


    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override
    {
        g.setColour(rowIsSelected ? juce::Colours::lightblue : juce::Colours::white); // Set the background color of the row based on whether it is selected or not
        g.fillRect(0, 0, width, height); // Fill the row with the background color
        g.setColour(juce::Colours::black); // Set the color of the text
        g.setFont(height * 0.7f); // Set the font size
        g.drawText(fileNames[rowNumber], 5, 0, width - 10, height, juce::Justification::centredLeft); // Draw the text for the row
    }


    juce::StringArray getFileNames()
    {
        return fileNames;
    }

private:
    juce::Array<juce::File> files;
    juce::StringArray fileNames;
    
};
