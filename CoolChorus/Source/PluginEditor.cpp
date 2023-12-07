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
    InitializeSliders();
    InitializeLabels();
}

CoolChorusAudioProcessorEditor::~CoolChorusAudioProcessorEditor()
{
}

//==============================================================================
void CoolChorusAudioProcessorEditor::InitializeSliders()
{

    auto& params = processor.getParameters();
    
    // Dry Wet parameter
    juce::AudioParameterFloat* dryWetParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
    mDryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 15);
    mDryWetSlider.setNumDecimalPlacesToDisplay(3);
    
    mDryWetSlider.setRange(dryWetParameter->range.start, dryWetParameter->range.end);
    mDryWetSlider.setValue(*dryWetParameter);
    addAndMakeVisible(mDryWetSlider);
    mDryWetSlider.onValueChange = [this, dryWetParameter] { *dryWetParameter = mDryWetSlider.getValue(); };
    mDryWetSlider.onDragStart = [dryWetParameter] { dryWetParameter->beginChangeGesture(); };
    mDryWetSlider.onDragEnd = [dryWetParameter] { dryWetParameter->endChangeGesture(); };
    
    
    // Depth Parameter
    juce::AudioParameterFloat* depthParameter = (juce::AudioParameterFloat*)params.getUnchecked(1);
    mDepthSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mDepthSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 15);
    mDepthSlider.setNumDecimalPlacesToDisplay(3);
    
    mDepthSlider.setRange(depthParameter->range.start, depthParameter->range.end);
    mDepthSlider.setValue(*depthParameter);
    addAndMakeVisible(mDepthSlider);
    mDepthSlider.onValueChange = [this, depthParameter] { *depthParameter = mDepthSlider.getValue(); };
    mDepthSlider.onDragStart = [depthParameter] { depthParameter->beginChangeGesture(); };
    mDepthSlider.onDragEnd = [depthParameter] { depthParameter->endChangeGesture(); };
    
    
    // Rate Parameter
    juce::AudioParameterFloat* rateParameter = (juce::AudioParameterFloat*)params.getUnchecked(2);
    mRateSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mRateSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 15);
    mRateSlider.setNumDecimalPlacesToDisplay(3);
    
    mRateSlider.setRange(rateParameter->range.start, rateParameter->range.end);
    mRateSlider.setValue(*rateParameter);
    addAndMakeVisible(mRateSlider);
    mRateSlider.onValueChange = [this, rateParameter] { *rateParameter = mRateSlider.getValue(); };
    mRateSlider.onDragStart = [rateParameter] { rateParameter->beginChangeGesture(); };
    mRateSlider.onDragEnd = [rateParameter] { rateParameter->endChangeGesture(); };
    
    
    // Phase Offset Parameter
    juce::AudioParameterFloat* phaseOffsetParameter = (juce::AudioParameterFloat*)params.getUnchecked(3);
    mPhaseOffsetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mPhaseOffsetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 15);
    mPhaseOffsetSlider.setNumDecimalPlacesToDisplay(3);
    
    mPhaseOffsetSlider.setRange(phaseOffsetParameter->range.start, phaseOffsetParameter->range.end);
    mPhaseOffsetSlider.setValue(*phaseOffsetParameter);
    addAndMakeVisible(mPhaseOffsetSlider);
    mPhaseOffsetSlider.onValueChange = [this, phaseOffsetParameter] { *phaseOffsetParameter = mPhaseOffsetSlider.getValue(); };
    mPhaseOffsetSlider.onDragStart = [phaseOffsetParameter] { phaseOffsetParameter->beginChangeGesture(); };
    mPhaseOffsetSlider.onDragEnd = [phaseOffsetParameter] { phaseOffsetParameter->endChangeGesture(); };
    
    
    //Feedback parameter
    //Feedback is the second paramater in the processor (4)
    juce::AudioParameterFloat* feedbackParameter = (juce::AudioParameterFloat*)params.getUnchecked(4);
    mFeedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    mFeedbackSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 15);
    mFeedbackSlider.setNumDecimalPlacesToDisplay(3);

    mFeedbackSlider.setRange(feedbackParameter->range.start, feedbackParameter->range.end);
    mFeedbackSlider.setValue(*feedbackParameter);
    addAndMakeVisible(mFeedbackSlider);
    mFeedbackSlider.onValueChange = [this, feedbackParameter] { *feedbackParameter = mFeedbackSlider.getValue(); };
    mFeedbackSlider.onDragStart = [feedbackParameter] { feedbackParameter->beginChangeGesture(); };
    mFeedbackSlider.onDragEnd = [feedbackParameter] { feedbackParameter->endChangeGesture(); };

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
