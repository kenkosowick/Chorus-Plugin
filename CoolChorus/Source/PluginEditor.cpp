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
    
    // Getting the parameter info by reference
        auto& params = processor.getParameters();
        
        juce::AudioParameterFloat* dryWetParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);
        
        mDryWetSlider.setBounds(0, 0, 100, 100);
        mDryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        mDryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        mDryWetSlider.setRange(dryWetParameter->range.start, dryWetParameter->range.end);
        mDryWetSlider.setValue(*dryWetParameter);
        addAndMakeVisible(mDryWetSlider);
        
        //Lambda function, passing in its current state with "this"
        mDryWetSlider.onValueChange = [this, dryWetParameter] { *dryWetParameter = mDryWetSlider.getValue(); };
        mDryWetSlider.onDragStart = [dryWetParameter] { dryWetParameter->beginChangeGesture(); };
        mDryWetSlider.onDragEnd = [dryWetParameter] { dryWetParameter->endChangeGesture(); };

        //Feedback is the second paramater in the processor (1)
        juce::AudioParameterFloat* feedbackParameter = (juce::AudioParameterFloat*)params.getUnchecked(1);
        
        mFeedbackSlider.setBounds(100,0,100,100);
        mFeedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        mFeedbackSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        mFeedbackSlider.setRange(feedbackParameter->range.start, feedbackParameter->range.end);
        mFeedbackSlider.setValue(*feedbackParameter);
        addAndMakeVisible(mFeedbackSlider);
        
        //Lambda function, passing in its current state with "this"
        mFeedbackSlider.onValueChange = [this, feedbackParameter] { *feedbackParameter = mFeedbackSlider.getValue(); };
        mFeedbackSlider.onDragStart = [feedbackParameter] { feedbackParameter->beginChangeGesture(); };
        mFeedbackSlider.onDragEnd = [feedbackParameter] { feedbackParameter->endChangeGesture(); };
        
        //Delay Time
        juce::AudioParameterFloat* delayTimeParameter = (juce::AudioParameterFloat*)params.getUnchecked(2);
        
        mDelayTimeSlider.setBounds(200,0,100,100);
        mDelayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        mDelayTimeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        mDelayTimeSlider.setRange(delayTimeParameter->range.start, delayTimeParameter->range.end);
        mDelayTimeSlider.setValue(*delayTimeParameter);
        addAndMakeVisible(mDelayTimeSlider);
        
        //Lambda function, passing in its current state with "this"
        mDelayTimeSlider.onValueChange = [this, delayTimeParameter] { *delayTimeParameter = mDelayTimeSlider.getValue(); };
        mDelayTimeSlider.onDragStart = [delayTimeParameter] { delayTimeParameter->beginChangeGesture(); };
        mDelayTimeSlider.onDragEnd = [delayTimeParameter] { delayTimeParameter->endChangeGesture(); };

}

CoolChorusAudioProcessorEditor::~CoolChorusAudioProcessorEditor()
{
}

//==============================================================================
void CoolChorusAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void CoolChorusAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
