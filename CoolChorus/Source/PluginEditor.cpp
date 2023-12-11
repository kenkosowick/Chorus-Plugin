/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CoolChorusAudioProcessorEditor::CoolChorusAudioProcessorEditor (CoolChorusAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 250);
    InitializeUIElements();
}

CoolChorusAudioProcessorEditor::~CoolChorusAudioProcessorEditor()
{
}

//==============================================================================
void CoolChorusAudioProcessorEditor::InitializeUIElements()
{
    InitializeSlider(&mDryWetSlider, 0);
    InitializeSlider(&mDepthSlider, 1);
    InitializeSlider(&mRateSlider, 2);
    InitializeSlider(&mPhaseOffsetSlider, 3);
    InitializeSlider(&mFeedbackSlider, 4);
    
    InitializeLabel(&mDryWetLabel, "Mix");
    InitializeLabel(&mDepthLabel, "Depth");
    InitializeLabel(&mRateLabel, "Rate");
    InitializeLabel(&mPhaseOffsetLabel, "Phase Offset");
    InitializeLabel(&mFeedbackLabel, "Feedback");

    auto& params = processor.getParameters();
    //Type Parameter
    AudioParameterInt* typeParameter = (AudioParameterInt*)params.getUnchecked(5);
    mTypeBox.addItem("Chorus", 1);
    mTypeBox.addItem("Flanger", 2);
    addAndMakeVisible(mTypeBox);
    
    mTypeBox.onChange = [this, typeParameter]
    {
        typeParameter->beginChangeGesture();
        *typeParameter = mTypeBox.getSelectedItemIndex();
        typeParameter->endChangeGesture();
    };
    
    mTypeBox.setSelectedItemIndex(*typeParameter);
}

void CoolChorusAudioProcessorEditor::InitializeSlider(Slider* slider, int paramIndex)
{
    auto& params = processor.getParameters();
    jassert(paramIndex >= 0 && paramIndex < params.size());
    AudioParameterFloat* parameter = (AudioParameterFloat*)params.getUnchecked(paramIndex);
    
    jassert(slider != nullptr);
    slider->setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    slider->setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 15);
    slider->setNumDecimalPlacesToDisplay(3);
    
    slider->setRange(parameter->range.start, parameter->range.end);
    slider->setValue(*parameter);
    addAndMakeVisible(slider);
    slider->onValueChange = [this, parameter, slider] { *parameter = slider->getValue(); };
    slider->onDragStart = [parameter] { parameter->beginChangeGesture(); };
    slider->onDragEnd = [parameter] { parameter->endChangeGesture(); };
}

void CoolChorusAudioProcessorEditor::InitializeLabel(Label* label, const String& labelText)
{
    label->setText(labelText, NotificationType::dontSendNotification);
    label->setJustificationType(Justification::centred);
    addAndMakeVisible(label);
}

void CoolChorusAudioProcessorEditor::InitializeLabels()
{
    mDryWetLabel.setText("Mix", juce::NotificationType::dontSendNotification);
    mDryWetLabel.setJustificationType(juce::Justification::centred);
    
    mDepthLabel.setText("Depth", juce::NotificationType::dontSendNotification);
    mDepthLabel.setJustificationType(juce::Justification::centred);
    
    mRateLabel.setText("Rate", juce::NotificationType::dontSendNotification);
    mRateLabel.setJustificationType(juce::Justification::centred);
    
    mPhaseOffsetLabel.setText("Phase Offset", juce::NotificationType::dontSendNotification);
    mPhaseOffsetLabel.setJustificationType(juce::Justification::centred);
    
    mFeedbackLabel.setText("Feedback", juce::NotificationType::dontSendNotification);
    mFeedbackLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(mDryWetLabel);
    addAndMakeVisible(mDepthLabel);
    addAndMakeVisible(mRateLabel);
    addAndMakeVisible(mPhaseOffsetLabel);
    addAndMakeVisible(mFeedbackLabel);
}

//==============================================================================
void CoolChorusAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    /*
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
     */
}

void CoolChorusAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor...
    const int centerX = getWidth()/2;
    const int centerY = getHeight()/2;
    const int compWidth = 100;
    
    mDryWetSlider.setBounds(centerX - 50 - compWidth*2, centerY - 70, compWidth, 100);
    mDepthSlider.setBounds(centerX - 50 - compWidth, centerY - 70,compWidth,100);
    mRateSlider.setBounds(centerX - 50, centerY - 70, compWidth, 100);
    mPhaseOffsetSlider.setBounds(centerX - 50 + compWidth,centerY - 70,compWidth,100);
    mFeedbackSlider.setBounds(centerX - 50 + compWidth*2,centerY - 70,compWidth,100);
    
    mDryWetLabel.setBounds(centerX - 50 - compWidth*2, centerY + 40, compWidth, 30);
    mDepthLabel.setBounds(centerX - 50 - compWidth, centerY + 40, compWidth, 30);
    mRateLabel.setBounds(centerX - 50, centerY + 40, compWidth, 30);
    mPhaseOffsetLabel.setBounds(centerX - 50 + compWidth, centerY + 40, compWidth, 30);
    mFeedbackLabel.setBounds(centerX - 50 + compWidth*2, centerY + 40, compWidth, 30);
    
    mTypeBox.setBounds(centerX - 50, centerY - 110, compWidth, 20);

}
