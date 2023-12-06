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
class CoolChorusAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CoolChorusAudioProcessorEditor (CoolChorusAudioProcessor&);
    ~CoolChorusAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void InitializeSliders();
    void InitializeLabels(); 

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CoolChorusAudioProcessor& audioProcessor;
    
    juce::Slider mDryWetSlider;
    juce::Slider mFeedbackSlider;
    juce::Slider mDelayTimeSlider;
    
    juce::Label mDryWetLabel;
    juce::Label mFeedbackLabel;
    juce::Label mDelayTimeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoolChorusAudioProcessorEditor)
};
