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
    setSize (400, 300);
    InitializeSliders();
    InitializeLabels();
}

CoolChorusAudioProcessorEditor::~CoolChorusAudioProcessorEditor()
{
}

//==============================================================================
void CoolChorusAudioProcessorEditor::InitializeSliders()
{
    // Getting the parameter info by reference

    auto& params = processor.getParameters();
    
    juce::AudioParameterFloat* dryWetParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
    
    mDryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 15);
    mDryWetSlider.setNumDecimalPlacesToDisplay(3);
    
    mDryWetSlider.setRange(dryWetParameter->range.start, dryWetParameter->range.end);
    mDryWetSlider.setValue(*dryWetParameter);
    addAndMakeVisible(mDryWetSlider);
    
    //Lambda function, passing in its current state with "this"
    mDryWetSlider.onValueChange = [this, dryWetParameter] { *dryWetParameter = mDryWetSlider.getValue(); };
    mDryWetSlider.onDragStart = [dryWetParameter] { dryWetParameter->beginChangeGesture(); };
    mDryWetSlider.onDragEnd = [dryWetParameter] { dryWetParameter->endChangeGesture(); };

    //Feedback is the second paramater in the processor (1)
    juce::AudioParameterFloat* feedbackParameter = (juce::AudioParameterFloat*)params.getUnchecked(1);
    
    
    mFeedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mFeedbackSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 15);
    mFeedbackSlider.setNumDecimalPlacesToDisplay(3);

    mFeedbackSlider.setRange(feedbackParameter->range.start, feedbackParameter->range.end);
    mFeedbackSlider.setValue(*feedbackParameter);
    addAndMakeVisible(mFeedbackSlider);
    
    //Lambda function, passing in its current state with "this"
    mFeedbackSlider.onValueChange = [this, feedbackParameter] { *feedbackParameter = mFeedbackSlider.getValue(); };
    mFeedbackSlider.onDragStart = [feedbackParameter] { feedbackParameter->beginChangeGesture(); };
    mFeedbackSlider.onDragEnd = [feedbackParameter] { feedbackParameter->endChangeGesture(); };

}

void CoolChorusAudioProcessorEditor::InitializeLabels()
{
    mDryWetLabel.setText("Mix", juce::NotificationType::dontSendNotification);
    mDryWetLabel.setJustificationType(juce::Justification::centred);
    mFeedbackLabel.setText("Feedback", juce::NotificationType::dontSendNotification);
    mFeedbackLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(mDryWetLabel);
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
    
    mDryWetSlider.setBounds(centerX - 150, centerY - 70, 100, 100);
    mFeedbackSlider.setBounds(centerX - 50,centerY - 70,100,100);
    
    mDryWetLabel.setBounds(centerX - 150, centerY + 40, 100, 30);
    mFeedbackLabel.setBounds(centerX - 50, centerY + 40, 100, 30);

}
