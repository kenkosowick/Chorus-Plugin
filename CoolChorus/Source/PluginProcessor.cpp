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
    addParameter(mDryWetParameter = new juce::AudioParameterFloat("drywet",
                                                             "Dry Wet",
                                                             0.0,
                                                             1.0,
                                                            0.5));
    addParameter(mFeedbackParameter = new juce::AudioParameterFloat("feedback",
                                                           "Feedback",
                                                           0.0,
                                                           0.98,
                                                           0.5));
    addParameter(mDelayTimeParameter = new juce::AudioParameterFloat( "delaytime",
                                                            "Delay Time",
                                                            0.01,
                                                            MAX_DELAY_TIME,
                                                            0.5));
                                    
    mCircularBufferLeft = nullptr;
    mCircularBufferRight= nullptr;
    mCircularBufferWriteHead = 0;
    mCircularBufferLength = 0;
    mDelayTimeInSamples = 0;
    mDelayReadHead = 0;
    mFeedbackLeft = 0;
    mFeedbackRight = 0;
    mDelayTimeSmoothed = 0;
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
    mDelayTimeInSamples = sampleRate * *mDelayTimeParameter;
    
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
    
    mDelayTimeSmoothed = *mDelayTimeParameter;
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

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    /*
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
    */
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);

    for ( int i =0; i < buffer.getNumSamples(); i++ ) {
        
        mDelayTimeSmoothed = mDelayTimeSmoothed - 0.0001 * (mDelayTimeSmoothed - *mDelayTimeParameter); //Smoothing Formula
        mDelayTimeInSamples = getSampleRate() * mDelayTimeSmoothed;

        mCircularBufferLeft[mCircularBufferWriteHead] = leftChannel[i] + mFeedbackLeft;
        mCircularBufferRight[mCircularBufferWriteHead]= rightChannel[i] + mFeedbackRight;
        
        //Setting delay readhead
        mDelayReadHead = mCircularBufferWriteHead - mDelayTimeInSamples;
        if ( mDelayReadHead < 0 ) {
            mDelayReadHead += mCircularBufferLength;
        }
        
        int readHead_x = (int)mDelayReadHead;
        int readHead_x1 = readHead_x + 1;
        if ( readHead_x1 >= mCircularBufferLength) { //checking to see if readHead is wrapping around
            readHead_x1 -= mCircularBufferLength;
        }
        float readHeadFloat = mDelayReadHead - readHead_x; //the difference between the two readheads
        
        
        
        float delay_sample_left = myfunc::lin_interp(mCircularBufferLeft[readHead_x], mCircularBufferLeft[readHead_x1], readHeadFloat);
        float delay_sample_right = myfunc::lin_interp(mCircularBufferRight[readHead_x], mCircularBufferRight[readHead_x1], readHeadFloat);
        
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