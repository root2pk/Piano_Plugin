/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "JuceHeader.h"

//==============================================================================
PluginAudioProcessor::PluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
parameters(*this, nullptr, "ParamTree", {
// Parameter layout 
// id, description, min val, max val, default val
std::make_unique<juce::AudioParameterFloat>("gain","Gain",0.5f, 200.0f, 50.0f),
std::make_unique<juce::AudioParameterFloat>("choice","Struck or Plucked(0 or 1)",0.0f,1.0f,0.1f),
std::make_unique<juce::AudioParameterFloat>("T60time","T60(s)",1.0f, 10.0f, 5.0f),
std::make_unique<juce::AudioParameterFloat>("interval","Interval(milliseconds)",0.0f, 1000.0f, 20.0f),
std::make_unique<juce::AudioParameterFloat>("freqParam","Frequency Random Parameter",0.0f, 10.0f, 1.0f),
std::make_unique<juce::AudioParameterFloat>("baseVel","Strike force(N)",0.0f, 100.0f, 15.0f),
std::make_unique<juce::AudioParameterFloat>("velCurve","Velocity Curve",0.0f, 15.0f, 5.0f),
std::make_unique<juce::AudioParameterFloat>("youngsModulus","Young's Modulus(GPa)",10.0f, 1000.0f, 190.0f),
std::make_unique<juce::AudioParameterFloat>("density","Density (kg/m^3)",1000.0f, 20000.0f, 8000.0f),
std::make_unique<juce::AudioParameterFloat>("xi","Striking Point",0.01f, 0.99f, 0.3f),
std::make_unique<juce::AudioParameterFloat>("xo","Microphone Position", 0.01f, 0.99f, 0.5f),
std::make_unique<juce::AudioParameterFloat>("lengthParam","Length",0.1f, 10.0f, 1.0f),
std::make_unique<juce::AudioParameterFloat>("radiusParam","Radius",0.1f, 10.0f, 1.0f),
std::make_unique<juce::AudioParameterFloat>("lim1","MIDI limit for 1 string note", 1, 127, 14),
std::make_unique<juce::AudioParameterFloat>("lim2","MIDI limit for 2 string note", 1, 127, 30),
std::make_unique<juce::AudioParameterFloat>("attack","Attack(s)",0.01f, 2.0f, 0.05f),
std::make_unique<juce::AudioParameterFloat>("decay","Decay(s)",0.01f, 2.0f, 0.05f),
std::make_unique<juce::AudioParameterFloat>("sustain","Sustain(level)",0.1f, 1.0f, 0.5f),
std::make_unique<juce::AudioParameterFloat>("release","Release(s)",0.01f, 5.0f, 0.2f),
})

{   // Constructor

     /// Audio Processor Value Parameters
    gain = parameters.getRawParameterValue("gain");
    choice = parameters.getRawParameterValue("choice");
    T60time = parameters.getRawParameterValue("T60time");

    attack = parameters.getRawParameterValue("attack");
    decay = parameters.getRawParameterValue("decay");
    sustain = parameters.getRawParameterValue("sustain");
    release = parameters.getRawParameterValue("release");
    
    interval = parameters.getRawParameterValue("interval");
    freqParam = parameters.getRawParameterValue("freqParam");

    velCurve = parameters.getRawParameterValue("velCurve");
    baseVel = parameters.getRawParameterValue("baseVel");

    youngsModulus = parameters.getRawParameterValue("youngsModulus");
    density = parameters.getRawParameterValue("density");

    xi = parameters.getRawParameterValue("xi");
    xo = parameters.getRawParameterValue("xo");

    lengthParam = parameters.getRawParameterValue("lengthParam");
    radiusParam = parameters.getRawParameterValue("radiusParam");

    lim1 = parameters.getRawParameterValue("lim1");
    lim2 = parameters.getRawParameterValue("lim2");

    // Adding Synth voices
    for (int i = 0; i < voiceCount; i++) {
        synth.addVoice(new SynthVoice());
    }
    // Adding Synth sound
    synth.addSound(new SynthSound());

    // Variable Parameters
    for (int i = 0; i < voiceCount; i++) {
        SynthVoice* v = dynamic_cast<SynthVoice*>(synth.getVoice(i));
        v->setParamPointers(T60time, gain, velCurve, baseVel, choice, youngsModulus, density);
        v->setNotePointers(interval, freqParam, xi, xo, lengthParam, radiusParam, lim1, lim2);
        v->setADSRPointers(attack, decay, sustain, release);
    }
}

PluginAudioProcessor::~PluginAudioProcessor()
{
}

//==============================================================================
const juce::String PluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void PluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================


void PluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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


//==============================================================================

void PluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    synth.setCurrentPlaybackSampleRate(sampleRate);             // Set sample rate for synthesiser

    for (int i = 0; i < voiceCount; i++) {
        SynthVoice* v = dynamic_cast<SynthVoice*>(synth.getVoice(i));
        v->init(sampleRate);
    }
}

void PluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // Calling render block for synth
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool PluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
} 

//==============================================================================
void PluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginAudioProcessor();
}
