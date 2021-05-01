/*
  ==============================================================================

    Synth.h
    Created: 30 Apr 2021 2:28:16pm
    Author:  Ruthu

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Note.h"

// ===========================
// ===========================
// SOUND
class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    //--------------------------------------------------------------------------
    bool appliesToChannel(int) override { return true; }
};




// =================================
// =================================
// Synthesiser Voice - Synth code goes in here

/*!
 @class SynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple SynthVoice objects will be created by the Synthesiser so that it can be played polyphicially

 @namespace none
 @updated 2019-06-18
 */
class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice() {}

    void init(float sampleRate) {
        note.setSampleRate(sampleRate);
        note.setSampleRate(sampleRate);
    }
    //--------------------------------------------------------------------------
    /**
     What should be done when a note starts

     @param midiNoteNumber
     @param velocity
     @param SynthesiserSound unused variable
     @param / unused variable
     */
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        playing = true;
        int lim1 = 14;
        int lim2 = 30;
        if (midiNoteNumber < lim1) {
            note.setNumStrings(1);
        }
        else if(midiNoteNumber < lim2) {
            note.setNumStrings(2);
        }
        else {
            note.setNumStrings(3);
        }
        float length = (-0.019196429) * float(midiNoteNumber) + 1.815625;
        float radius = (-2.08333e-03) * float(midiNoteNumber) + 0.62875;
        note.setStringParams(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber), length, radius, 5.0f);
        note.setForceParameters(1.2, 5.0f);
        note.setInterval(20);

    }
    //--------------------------------------------------------------------------
    /// Called when a MIDI noteOff message is received
    /**
     What should be done when a note stops

     @param / unused variable
     @param allowTailOff bool to decie if the should be any volume decay
     */
    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        clearCurrentNote();
        playing = false;
    }

    //--------------------------------------------------------------------------
    /**
     The Main DSP Block: Put your DSP code in here

     If the sound that the voice is playing finishes during the course of this rendered block, it must call clearCurrentNote(), to tell the synthesiser that it has finished

     @param outputBuffer pointer to output
     @param startSample position of first sample in buffer
     @param numSamples number of smaples in output buffer
     */
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {   
        gain = 2000.0f;
        if (playing) // check to see if this voice should be playing
        {
            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample; sampleIndex < (startSample + numSamples); sampleIndex++)
            {
                // your sample-by-sample DSP code here!
                // An example white noise generater as a placeholder - replace with your own code
                float currentSample = gain*note.process();

                // for each channel, write the currentSample float to the output
                for (int chan = 0; chan < outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample(chan, sampleIndex, currentSample);
                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void pitchWheelMoved(int) override {}
    //--------------------------------------------------------------------------
    void controllerMoved(int, int) override {}
    //--------------------------------------------------------------------------
    /**
     Can this voice play a sound. I wouldn't worry about this for the time being

     @param sound a juce::SynthesiserSound* base class pointer
     @return sound cast as a pointer to an instance of SynthSound
     */
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SynthSound*> (sound) != nullptr;
    }
    //--------------------------------------------------------------------------
private:
    //--------------------------------------------------------------------------
    // Set up any necessary variables here
    /// Should the voice be playing?
    bool playing = false;

    /// Random Object
    juce::Random random;

    /// Note object
    Note note;

    /// Gain
    float gain;

};