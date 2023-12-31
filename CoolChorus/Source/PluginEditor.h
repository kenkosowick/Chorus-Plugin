/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
using namespace juce;

class CoolChorusAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CoolChorusAudioProcessorEditor (CoolChorusAudioProcessor&);
    ~CoolChorusAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void InitializeUIElements();
    void InitializeSlider(Slider* slider, int parameterIndex);
    void InitializeLabel(Label* label, const String& labelText);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CoolChorusAudioProcessor& audioProcessor;
    
    juce::Slider mDryWetSlider;
    juce::Slider mFeedbackSlider;
    //juce::Slider mDelayTimeSlider;
    Slider mDepthSlider;
    Slider mRateSlider;
    Slider mPhaseOffsetSlider;
    
    ComboBox mTypeBox;
    
    juce::Label mDryWetLabel;
    juce::Label mFeedbackLabel;
    //juce::Label mDelayTimeLabel;
    Label mDepthLabel;
    Label mRateLabel;
    Label mPhaseOffsetLabel;
    Label mTypeLabel; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoolChorusAudioProcessorEditor)
};
