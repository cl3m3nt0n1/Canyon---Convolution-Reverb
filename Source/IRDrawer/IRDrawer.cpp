
#include <JuceHeader.h>
#include "IRDrawer.h"

IRDrawer::IRDrawer(ConvolutionReverbAudioProcessor& processor) :
	proc(processor),
	thumbnailCache(5),
	thumbnail(512, formatManager, thumbnailCache),
	listBoxModel(
		juce::File::getSpecialLocation(juce::File::commonDocumentsDirectory).getChildFile(ProjectInfo::companyName).
		getChildFile("IR Files"))	 
{
	formatManager.registerBasicFormats();

	irComboBox.addItemList(listBoxModel.getFileNames(), -1);
	irComboBox.onChange = [&]()
	{
		auto selectedItem = irComboBox.getSelectedItemIndex();
		loadFile(selectedItem);
		repaint();
	};

	addAndMakeVisible(irComboBox);
	// loadFile(irComboBox.getItemId(0));

	irSampleStartSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	irSampleStartSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	addAndMakeVisible(&irSampleStartSlider);

	irSampleStartLabel.attachToComponent(&irSampleStartSlider, false);
	irSampleStartLabel.setText("Start", juce::NotificationType::dontSendNotification);
	irSampleStartLabel.setColour(juce::Label::textColourId, juce::Colours::white);
	irSampleStartLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(&irSampleStartLabel);

	irSampleStopSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	irSampleStopSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	addAndMakeVisible(&irSampleStopSlider);

	irSampleStopLabel.attachToComponent(&irSampleStopSlider, false);
	irSampleStopLabel.setText("Stop", juce::NotificationType::dontSendNotification);
	irSampleStopLabel.setColour(juce::Label::textColourId, juce::Colours::white);
	irSampleStopLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(&irSampleStopLabel);

	sampleViewer.setCentre(getWidth() / 2, getHeight() / 2);
	openButton.setButtonText("Open File...");
	openButton.setEnabled(true);
	openButton.onClick = [&]()
	{
		loadFile();
		repaint();
	};
	addAndMakeVisible(openButton);

	thumbnail.addChangeListener(this);

	irComboBox.setSelectedId(1);
	loadFile(1);
}

IRDrawer::~IRDrawer()
{
	formatManager.clearFormats();
	delete formatReader;
}

void IRDrawer::paint(juce::Graphics& g)
{
	g.setColour(juce::Colour::fromRGBA(0, 0, 0, 60));

	IRPath.addRoundedRectangle(getLocalBounds(), 30);
	g.fillPath(IRPath);

	g.setColour(juce::Colours::white);
	g.drawRect(sampleViewer);

	g.setColour(juce::Colour::fromRGB(0, 0, 0));
	g.fillRect(sampleViewer);


	g.setColour(juce::Colours::white);
	g.setFont(30.0f);
	g.drawText("Impulse Response", 14, 14, 250, 28, juce::Justification::centred);

	/*
	g.setColour(juce::Colours::white);
	sampleStartLine.setStart(sampleViewer.getTopLeft().getX(), sampleViewer.getTopLeft().getY());
	sampleStartLine.setEnd(sampleViewer.getBottomLeft().getX(), sampleViewer.getBottomLeft().getY());
	g.drawLine(sampleStartLine, 3.0f);
	g.drawLine(sampleStopLine, 1.0f);
	*/


	if (thumbnail.getNumChannels() != 0)
	{
		thumbnail.drawChannels(g, sampleViewer.toNearestInt(), 0.0, thumbnail.getTotalLength(), 1.0f);
	}

}

void IRDrawer::resized()
{
	// This method is where you should set the bounds of any child
	// components that your component contains..

	openButton.setBounds(6 * getWidth() / 8, getHeight() / 16, 60, 30);

	sampleViewer.setBounds(getWidth() / 16, 4 * getHeight() / 16, 14 * getWidth() / 16, 155);

	irSampleStopSlider.setBounds(2 * getWidth() / 4, 12 * getHeight() / 16, 65, 65);
	irSampleStartSlider.setBounds(getWidth() / 4, 12 * getHeight() / 16, 65, 65);

	irComboBox.setBounds(getWidth()/3, 60, 150, 30);
}

bool IRDrawer::isInterestedInFileDrag(const juce::StringArray& files)
{
	for (auto file : files)
	{
		if (file.contains(".wav") || file.contains(".mp3") ||
			file.contains(".flac") || file.contains(".aif"))
			return true;
	}
	return false;
}

void IRDrawer::filesDropped(const juce::StringArray& files, int x, int y)
{
	for (auto file : files)
	{
		if (isInterestedInFileDrag(files))
		{
			loadFile(file);
		}
	}
	repaint();
}

void IRDrawer::loadFile()
{
	// Opens a dialog to ask the user to choose a sample to load
	juce::FileChooser chooser("Select a file to open...",
	                          juce::File::getSpecialLocation(juce::File::userMusicDirectory),
	                          "*.wav, *.mp3, *.flac, *.aac");

	if (chooser.browseForFileToOpen())
	{
		// Fetching the file
		try
		{
			const auto file = chooser.getResult();

			//Creates a reader for the loaded file
			formatReader = formatManager.createReaderFor(file);

			if (formatReader != nullptr)
			{
				const auto sampleLength = static_cast<int>(formatReader->lengthInSamples);
				waveForm.setSize(1, sampleLength);
				formatReader->read(&waveForm, 0, sampleLength, 0, true, false);
				proc.setIRWaveForm(getWaveForm());
				proc.setIRFile(file);

				if (proc.AB == 0) {
					
					proc.convolution.loadImpulseResponse(file, juce::dsp::Convolution::Stereo::yes,
						juce::dsp::Convolution::Trim::yes, 0,
						juce::dsp::Convolution::Normalise::yes);
				}
				else {
					proc.convolutionB.loadImpulseResponse(file, juce::dsp::Convolution::Stereo::yes,
						juce::dsp::Convolution::Trim::yes, 0,
						juce::dsp::Convolution::Normalise::yes);
				}
				
				thumbnail.setSource(new juce::FileInputSource(file));
			}
			else
			{
				throw std::runtime_error("Failed to create audio reader for selected file");
			}
		}
		catch (const std::exception& e)
		{
			juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
			                                       "Erreur lors de l'ouverture du fichier",
			                                       e.what(),
			                                       "OK");
			formatReader = nullptr;
		}
	}
	else
	{
		DBG("No file selected.");
	}
}

void IRDrawer::loadFile(const juce::String& path)
{
	const auto file = juce::File(path);
	formatReader = formatManager.createReaderFor(file); //Creates a reader for the loaded file

	const auto sampleLength = static_cast<int>(formatReader->lengthInSamples);

	waveForm.setSize(1, sampleLength);
	formatReader->read(&waveForm, 0, sampleLength, 0, true, false);

	proc.setIRWaveForm(getWaveForm());
	proc.setIRFile(file);
	if (proc.AB == 0) {

		proc.convolution.loadImpulseResponse(file, juce::dsp::Convolution::Stereo::yes,
			juce::dsp::Convolution::Trim::yes, 0,
			juce::dsp::Convolution::Normalise::yes);
	}
	else {
		proc.convolutionB.loadImpulseResponse(file, juce::dsp::Convolution::Stereo::yes,
			juce::dsp::Convolution::Trim::yes, 0,
			juce::dsp::Convolution::Normalise::yes);
	}
	thumbnail.setSource(new juce::FileInputSource(file));
}

void IRDrawer::loadFile(int row)
{
	const auto file = listBoxModel.getFile(row);
	formatReader = formatManager.createReaderFor(file); //Creates a reader for the loaded file

	const auto sampleLength = static_cast<int>(formatReader->lengthInSamples);

	waveForm.setSize(1, sampleLength);
	formatReader->read(&waveForm, 0, sampleLength, 0, true, false);

	proc.setIRWaveForm(getWaveForm());
	proc.setIRFile(file);
	if (proc.AB == 0) {

		proc.convolution.loadImpulseResponse(file, juce::dsp::Convolution::Stereo::yes,
			juce::dsp::Convolution::Trim::yes, 0,
			juce::dsp::Convolution::Normalise::yes);
	}
	else {
		proc.convolutionB.loadImpulseResponse(file, juce::dsp::Convolution::Stereo::yes,
			juce::dsp::Convolution::Trim::yes, 0,
			juce::dsp::Convolution::Normalise::yes);
	}
	thumbnail.setSource(new juce::FileInputSource(file));

}

void IRDrawer::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	repaint();
}
