
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ConvolutionReverbAudioProcessorEditor::ConvolutionReverbAudioProcessorEditor(ConvolutionReverbAudioProcessor& p)

    : AudioProcessorEditor(&p), audioProcessor(p), drawer(p), drawerB(p),
    filterDrawer(p, { "LowCut Freq", "HighCut Freq", "Peak Freq", "Peak Gain", "Peak Quality", "LowCut Slope", "HighCut Slope" }),
    filterDrawerB(p, { "LowCut FreqB", "HighCut FreqB", "Peak FreqB", "Peak GainB", "Peak QualityB", "LowCut SlopeB", "HighCut SlopeB" }),
    dryWetSliderAttachement(audioProcessor.apvts, "Dry Wet", dryWetSlider),
    blendSliderAttachement(audioProcessor.apvts, "Blend", blendSlider),
    gainSliderAttachement(audioProcessor.apvts, "Gain", gainSlider)
    

	//TODO
	/*,presetManagerComponent(p.getPresetManager())*/
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(900, 630);

    setLookAndFeel(&NLNF);

    //Elements du Header
    //TODO
    // addAndMakeVisible(&presetManagerComponent);

   //Elements sans rectangle
    dryWetSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    dryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(&dryWetSlider);

  /*  dryWetLabel.attachToComponent(&dryWetSlider,false);
    dryWetLabel.setText("Dry|Wet", juce::NotificationType::dontSendNotification);
    dryWetLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    dryWetLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&dryWetLabel);*/


    gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(&gainSlider);

   /* gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setText("Gain", juce::NotificationType::dontSendNotification);
    gainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    gainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(&gainLabel);*/
     

    //Elements du filtre
    addAndMakeVisible(filterDrawer);
    addAndMakeVisible(filterDrawerB);


    //Elements du Blend

    blendSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    blendSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    addAndMakeVisible(&blendSlider);



    addAndMakeVisible(drawer);
    addAndMakeVisible(drawerB);

    AB_button.addListener(this);
    
    gradient1.setColour(0, juce::Colours::aliceblue);
    gradient1.setColour(100, juce::Colours::beige);

    addAndMakeVisible(AB_button);
}

ConvolutionReverbAudioProcessorEditor::~ConvolutionReverbAudioProcessorEditor()
{
    
}


void ConvolutionReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    
        

    background = juce::ImageCache::getFromMemory(BinaryData::Background_png, BinaryData::Background_pngSize);
    g.drawImageWithin(background, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
        

    //Elements du Header
    g.setColour(colours[0]);
 

    g.setColour(juce::Colours::white);
    g.setFont(70.0f);
    g.drawText("Canyon", 325, 20, 250, 43, juce::Justification::centred);


    //Elements du Filtre

    g.setColour(juce::Colour::fromRGBA(0, 0, 0, 60));

    rectPath.addRoundedRectangle(filterRect, 30);
    g.fillPath(rectPath);


    g.setColour(juce::Colours::white);
    g.setFont(30.0F);
    g.drawText("EQ Filter", 585, 114, 120, 30, juce::Justification::centred);

        


    //Elements du Blend
    
    g.setColour(juce::Colour::fromRGBA(0, 0, 0, 60));

    bottomPath.addRoundedRectangle(bottomRect, 30);
    g.fillPath(bottomPath);
   

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawText("Blend", 260, 504, 58, 29, juce::Justification::centred);

    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("A", 159, 542, 13, 19, juce::Justification::centred);

    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("B", 399, 542, 19, 19, juce::Justification::centred);



    //Element du Vu meter

    g.setColour(juce::Colour::fromRGBA(0, 100, 100, 60));

    rectPath.addRoundedRectangle(vuRect, 30);
    g.fillPath(vuPath);
    
    juce::Point<float> x = vuRect.getPosition();
    x.addXY(20, 0);
    auto y = vuRect.getBottomLeft();
    y.addXY(20, 0);

    g.setColour(juce::Colours::white);
    g.drawLine(juce::Line(x, y));

    //Element footer

    g.setFont(24.0F);
    g.drawText("DRY|WET", 510, 531, 120, 29, juce::Justification::centred);

    g.drawText("GAIN", 651, 531, 70, 29, juce::Justification::centred);
}


void ConvolutionReverbAudioProcessorEditor::resized()
{

    //Elements du Header
    //TODO
    // presetManagerComponent.setBounds(0, 0, 400, 330);

    //Elements sans rectangle
    dryWetSlider.setBounds(431, 506, 79, 79);

    gainSlider.setBounds(719, 506, 79, 79);


    //Elements de l'IR

    drawer.setBounds(27, 94, 413, 380);
    drawerB.setBounds(0, 0, 0, 0);


    //Elements du filtre

    filterDrawer.setBounds(451, 94, 365, 380);
    filterDrawerB.setBounds(0,0,0,0);

    filterRect.setBounds(451, 94, 365, 380);



    //Elements du Blend
    blendSlider.setBounds(181, 528, 207, 50);

    bottomRect.setBounds(27, 489, 789, 107);

    AB_button.setBounds(53, 520, 76, 47);

    //Element du vu meter

    vuRect.setBounds(827, 94, 39, 500);

    

}


//================================== ROTARY ====================================
void  CustomLNF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{

    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);

    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = 3.5f;
    auto arcRadius = radius - lineW * 0.5f;

    juce::Path extArc;
    extArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true);

    g.setColour(juce::Colours::white);
    g.strokePath(extArc, juce::PathStrokeType(lineW+3, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));


    g.setColour(juce::Colour::fromRGB(37, 36, 39));
    juce::Path midCircle;
    int xCircle = x + bounds.getCentreX() - (radius)+lineW;
    int yCircle = x + bounds.getCentreY() - (radius)+lineW;
    int radiusCircle = (radius * 2) - (lineW * 2);
    midCircle.addRoundedRectangle(juce::Rectangle(xCircle, yCircle, radiusCircle+1, radiusCircle+1), 35);
    g.fillPath(midCircle);



    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        rotaryStartAngle,
        rotaryEndAngle,
        true);

    g.setColour(juce::Colours::black);
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
            bounds.getCentreY(),
            arcRadius,
            arcRadius,
            0.0f,
            rotaryStartAngle,
            toAngle,
            true);

        g.setColour(juce::Colour::fromRGB(151,10,194));
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    auto thumbWidth = lineW * 2.0f;
    juce::Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));

    g.setColour(juce::Colours::white);

    g.drawLine(thumbPoint.getX(), thumbPoint.getY(), bounds.getCentreX() + (arcRadius / 2) * std::cos(toAngle - juce::MathConstants<float>::halfPi),
        bounds.getCentreY() + (arcRadius / 2) * std::sin(toAngle - juce::MathConstants<float>::halfPi), 3.0f);

}


//================================== LINEAR ====================================

void CustomLNF::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos,
    float minSliderPos,
    float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (slider.isBar())
    {
        g.setColour(juce::Colours::blue);
        g.fillRect(slider.isHorizontal() ? juce::Rectangle<float>(static_cast<float> (x), (float)y + 0.5f, sliderPos - (float)x, (float)height - 1.0f)
            : juce::Rectangle<float>((float)x + 0.5f, sliderPos, (float)width - 1.0f, (float)y + ((float)height - sliderPos)));
    }
    else
    {
        auto isTwoVal = (style == juce::Slider::SliderStyle::TwoValueVertical || style == juce::Slider::SliderStyle::TwoValueHorizontal);
        auto isThreeVal = (style == juce::Slider::SliderStyle::ThreeValueVertical || style == juce::Slider::SliderStyle::ThreeValueHorizontal);

        auto trackWidth = juce::jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

        juce::Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
            slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

        juce::Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
            slider.isHorizontal() ? startPoint.y : (float)y);

        juce::Path backgroundTrack;
        backgroundTrack.startNewSubPath(startPoint);
        backgroundTrack.lineTo(endPoint);
        g.setColour(juce::Colours::blue);
        g.strokePath(backgroundTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

        juce::Path valueTrack;
        juce::Point<float> minPoint, maxPoint, thumbPoint;

        if (isTwoVal || isThreeVal)
        {
            minPoint = { slider.isHorizontal() ? minSliderPos : (float)width * 0.5f,
                         slider.isHorizontal() ? (float)height * 0.5f : minSliderPos };

            if (isThreeVal)
                thumbPoint = { slider.isHorizontal() ? sliderPos : (float)width * 0.5f,
                               slider.isHorizontal() ? (float)height * 0.5f : sliderPos };

            maxPoint = { slider.isHorizontal() ? maxSliderPos : (float)width * 0.5f,
                         slider.isHorizontal() ? (float)height * 0.5f : maxSliderPos };
        }
        else
        {
            auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
            auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

            minPoint = startPoint;
            maxPoint = { kx, ky };
        }

        auto thumbWidth = getSliderThumbRadius(slider);

        valueTrack.startNewSubPath(minPoint);
        valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
        g.setColour(slider.findColour(juce::Slider::trackColourId));
        g.strokePath(valueTrack, { trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded });

        if (!isTwoVal)
        {
            g.setColour(juce::Colours::white);
            g.fillEllipse(juce::Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre(isThreeVal ? thumbPoint : maxPoint));
        }

        if (isTwoVal || isThreeVal)
        {
            auto sr = juce::jmin(trackWidth, (slider.isHorizontal() ? (float)height : (float)width) * 0.4f);
            auto pointerColour = juce::Colours::white;

            if (slider.isHorizontal())
            {
                drawPointer(g, minSliderPos - sr,
                    juce::jmax(0.0f, (float)y + (float)height * 0.5f - trackWidth * 2.0f),
                    trackWidth * 2.0f, pointerColour, 2);

                drawPointer(g, maxSliderPos - trackWidth,
                    juce::jmin((float)(y + height) - trackWidth * 2.0f, (float)y + (float)height * 0.5f),
                    trackWidth * 2.0f, pointerColour, 4);
            }
            else
            {
                drawPointer(g, juce::jmax(0.0f, (float)x + (float)width * 0.5f - trackWidth * 2.0f),
                    minSliderPos - trackWidth,
                    trackWidth * 2.0f, pointerColour, 1);

                drawPointer(g, juce::jmin((float)(x + width) - trackWidth * 2.0f, (float)x + (float)width * 0.5f), maxSliderPos - sr,
                    trackWidth * 2.0f, pointerColour, 3);
            }
        }
    }
}


void ConvolutionReverbAudioProcessorEditor::buttonClicked(juce::Button*) {

    if (AB == 0) {
        filterDrawer.setBounds(0, 0, 0, 0);
        filterDrawerB.setBounds(451, 94, 365, 380);

        drawerB.setBounds(27, 94, 413, 380);
        drawer.setBounds(0, 0, 0, 0);

        AB_button.setButtonText("a | B");
        AB = 1;
        audioProcessor.AB = 1;
    }
    else {

        filterDrawer.setBounds(451, 94, 365, 380);
        filterDrawerB.setBounds(0, 0, 0, 0);

        drawer.setBounds(27, 94, 413, 380);
        drawerB.setBounds(0, 0, 0, 0);

        AB_button.setButtonText("A | b");
        AB = 0;
        audioProcessor.AB = 0;
    }
    
}