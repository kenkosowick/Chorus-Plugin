/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CoolChorusAudioProcessor::CoolChorusAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    //initilization
    addParameter(mDryWetParameter = new AudioParameterFloat("drywet",
                                                             "Dry Wet",
                                                             0.0,
                                                             1.0,
                                                            0.5));
    addParameter(mDepthParameter = new AudioParameterFloat("depth",
                                                           "Depth",
                                                           0.0,
                                                           1.0,
                                                           0.5));
    addParameter(mRateParameter = new AudioParameterFloat( "rate",
                                                            "Rate",
                                                            0.1f,
                                                            20.f,
                                                            10.f));
    addParameter(mPhaseOffsetParameter = new AudioParameterFloat( "phaseoffset",
                                                            "Phase Offset",
                                                            0.0f,
                                                            1.f,
                                                            0.f));
    addParameter(mFeedbackParameter = new AudioParameterFloat( "feedback",
                                                            "Feedback",
                                                            0,
                                                            0.98,
                                                            0.5));
    addParameter(mTypeParameter = new AudioParameterInt ("type",
                                                         "Type",
                                                         0,
                                                         1,
                                                         0));
                                    
    mCircularBufferLeft = nullptr;
    mCircularBufferRight= nullptr;
    mCircularBufferWriteHead = 0;
    mCircularBufferLength = 0;
    mDelayTimeInSamples = 0;
    mDelayReadHead = 0;
    mFeedbackLeft = 0;
    mFeedbackRight = 0;
    mDelayTimeSmoothed = 0;
    
    mLFOPhase = 0;
}

CoolChorusAudioProcessor::~CoolChorusAudioProcessor()
{
    //Free memory once App Closes
    if (mCircularBufferLeft != nullptr) {
        delete [] mCircularBufferLeft;
        mCircularBufferLeft = nullptr;
    }
    
    if (mCircularBufferRight == nullptr) {
        delete [] mCircularBufferRight;
        mCircularBufferRight = nullptr;
    }
}

//==============================================================================
const juce::String CoolChorusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CoolChorusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CoolChorusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CoolChorusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CoolChorusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CoolChorusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CoolChorusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CoolChorusAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CoolChorusAudioProcessor::getProgramName (int index)
{
    return {};
}

void CoolChorusAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
namespace myfunc
{
    float lin_interp ( float sample_x, float sample_x1, float inPhase)
    { //InPhase == time
        return (1 - inPhase) * sample_x + inPhase * sample_x1;
    }
}
//==============================================================================
void CoolChorusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mDelayTimeInSamples = 1;
    mLFOPhase = 0;
    
    mCircularBufferLength = sampleRate * MAX_DELAY_TIME;
    
    //Determining Circ Buffer Length using Sample Rate
    if (mCircularBufferLeft == nullptr)
    {
        mCircularBufferLeft = new float[mCircularBufferLength];
    }
    
        
    juce::zeromem(mCircularBufferLeft, mCircularBufferLength * sizeof(float));
        
    if (mCircularBufferRight == nullptr)
    {
        mCircularBufferRight = new float[mCircularBufferLength];
    }
    
    juce::zeromem(mCircularBufferRight, mCircularBufferLength * sizeof(float));

    mCircularBufferWriteHead = 0;
    
}

void CoolChorusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CoolChorusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CoolChorusAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);


    // Main samples loop
    for ( int i =0; i < buffer.getNumSamples(); i++ )
    {
        //LFO calculations for Delay time-------------------------------------------------------------------
        float lfoOutLeft = sin(2*M_PI * mLFOPhase);
        lfoOutLeft *= *mDepthParameter;
        
        
        float lfoPhaseRight = mLFOPhase + *mPhaseOffsetParameter;
        if (lfoPhaseRight > 1)
        {
            lfoPhaseRight -= 1;
        }
        
        float lfoOutRight = sin(2*M_PI * lfoPhaseRight);
        lfoOutRight *= *mDepthParameter;
        
        // Determine whether type is chorus or flanger
        float lfoOutMappedLeft = 0;
        float lfoOutMappedRight = 0;
        
        if (*mTypeParameter == 1) //Flanger
        {
            lfoOutMappedLeft = jmap(lfoOutLeft, -1.f, 1.f, 0.001f, 0.005f); // Delay time range
            lfoOutMappedRight = jmap(lfoOutRight, -1.f, 1.f, 0.001f, 0.005f);
        }
        else // Chorus
        {
            lfoOutMappedLeft = jmap(lfoOutLeft, -1.f, 1.f, 0.005f, 0.03f);
            lfoOutMappedRight = jmap(lfoOutRight, -1.f, 1.f, 0.005f, 0.03f);
        }

        
        mLFOPhase += *mRateParameter / getSampleRate(); //frequency/sr
        
        if (mLFOPhase > 1)
        {
            mLFOPhase -= 1;
        }
        
        
        mDelayTimeSmoothed = mDelayTimeSmoothed - 0.001 * (mDelayTimeSmoothed - lfoOutMappedLeft); //smoothing helps when lfo is using a saw tooth or other types of waveforms to prevents clicks. (Previously 0.0001)
        
        //(mDelayTimeSmoothed - *mDelayTimeParameter); //Smoothing Formula
        
        mDelayTimeInSamples = getSampleRate() * mDelayTimeSmoothed; //left
        float delayTimeSamplesRight = getSampleRate() * lfoOutMappedRight; //right
        //-------------------------------------------------------------------------------------
        
        mCircularBufferLeft[mCircularBufferWriteHead] = leftChannel[i] + mFeedbackLeft;
        mCircularBufferRight[mCircularBufferWriteHead]= rightChannel[i] + mFeedbackRight;
        
        //Setting delay readhead----------------------------------------------------------------
        mDelayReadHead = mCircularBufferWriteHead - mDelayTimeInSamples;
        float delayReadHeadRight = mCircularBufferWriteHead - delayTimeSamplesRight;
        
        if ( mDelayReadHead < 0 )
        {
            mDelayReadHead += mCircularBufferLength;
        }
        
        int readHead_L = (int)mDelayReadHead;
        int readHead_L1 = readHead_L + 1; // The sample one index ahead of the read head
        int readHead_R = (int)delayReadHeadRight;
        int readHead_R1 = readHead_R + 1;
        
        //checking to see if readHead_x1 is wrapping around
        if ( readHead_L1 >= mCircularBufferLength)
        {
            readHead_L1 -= mCircularBufferLength;
        }
        if ( readHead_R1 >= mCircularBufferLength)
        {
            readHead_R1 -= mCircularBufferLength;
        }
        float readHeadFloatL = mDelayReadHead - readHead_L; //the difference between the two readheads
        float readHeadFloatR = delayReadHeadRight - readHead_R;
        
        float delay_sample_left = myfunc::lin_interp(mCircularBufferLeft[readHead_L], mCircularBufferLeft[readHead_L1], readHeadFloatL);
        float delay_sample_right = myfunc::lin_interp(mCircularBufferRight[readHead_R], mCircularBufferRight[readHead_R1], readHeadFloatR);
        
        mFeedbackLeft = delay_sample_left * *mFeedbackParameter;
        mFeedbackRight = delay_sample_right * *mFeedbackParameter;

        //adding the delayed signal to the dry signal
        buffer.setSample( 0, i, buffer.getSample(0, i) * (1 - *mDryWetParameter) + delay_sample_left * *mDryWetParameter );
        buffer.setSample( 1, i, buffer.getSample(1, i) * (1 - *mDryWetParameter)+ delay_sample_right * *mDryWetParameter );

        //Wrapping around ReadHead is index is less than zero

        mCircularBufferWriteHead++;
        
        
        if ( mCircularBufferWriteHead >= mCircularBufferLength) {
            mCircularBufferWriteHead = 0;
        }
    }
}

//==============================================================================
bool CoolChorusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CoolChorusAudioProcessor::createEditor()
{
    return new CoolChorusAudioProcessorEditor (*this);
}

//==============================================================================
void CoolChorusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CoolChorusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CoolChorusAudioProcessor();
}
