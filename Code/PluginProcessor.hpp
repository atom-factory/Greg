#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

class GregProcessor : public juce::AudioProcessor {
public:
    GregProcessor();
    ~GregProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::String getCurrentPresetName() const {
        return mCurrentPresetName;
    }

    void setCurrentPresetName(const juce::String& name);

    juce::AudioProcessorValueTreeState mParameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    static constexpr size_t kOversampleOrder = 4;

    juce::String mCurrentPresetName {"Init"};

    // Add your parameters here
    float mGain        = 1.0f;
    double mSampleRate = 44100;
    std::unique_ptr<juce::dsp::Oversampling<float>> mOversampling;

    // Smoothed parameters
    juce::SmoothedValue<float> mDriveSmoothed;
    juce::SmoothedValue<float> mToneSmoothed;
    juce::SmoothedValue<float> mMixSmoothed;
    juce::SmoothedValue<float> mOutputSmoothed;

    // Previous parameter values for change detection
    float mPreviousDrive  = 0.0f;
    float mPreviousTone   = 0.0f;
    float mPreviousMix    = 100.0f;
    float mPreviousOutput = 0.0f;

    static float applySaturation(float input, float drive);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GregProcessor)
};