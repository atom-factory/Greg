#include "PluginProcessor.hpp"
#include "PluginEditor.hpp"

GregProcessor::GregProcessor()
    : AudioProcessor(BusesProperties()
                       .withInput("Input", juce::AudioChannelSet::stereo(), true)
                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      mParameters(*this, nullptr, "Parameters", createParameterLayout()) {}

GregProcessor::~GregProcessor() {}

const juce::String GregProcessor::getName() const {
    return JucePlugin_Name;
}

bool GregProcessor::acceptsMidi() const {
    return false;
}

bool GregProcessor::producesMidi() const {
    return false;
}

bool GregProcessor::isMidiEffect() const {
    return false;
}

double GregProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int GregProcessor::getNumPrograms() {
    return 1;
}

int GregProcessor::getCurrentProgram() {
    return 0;
}

void GregProcessor::setCurrentProgram(int index) {}

const juce::String GregProcessor::getProgramName(int index) {
    return {};
}

void GregProcessor::changeProgramName(int index, const juce::String& newName) {}

void GregProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    mSampleRate = sampleRate;

    // Initialize smoothed parameters
    double oversampledRate = sampleRate * kOversampleOrder;

    mDriveSmoothed.reset(oversampledRate, 0.01);
    mToneSmoothed.reset(oversampledRate, 0.01);
    mMixSmoothed.reset(oversampledRate, 0.01);
    mOutputSmoothed.reset(oversampledRate, 0.01);

    // Initialize with current parameter values
    mDriveSmoothed.setCurrentAndTargetValue(mParameters.getRawParameterValue("drive")->load());
    mToneSmoothed.setCurrentAndTargetValue(mParameters.getRawParameterValue("tone")->load());
    mMixSmoothed.setCurrentAndTargetValue(mParameters.getRawParameterValue("mix")->load());
    mOutputSmoothed.setCurrentAndTargetValue(mParameters.getRawParameterValue("output")->load());

    mOversampling =
      std::make_unique<juce::dsp::Oversampling<float>>(getTotalNumInputChannels(),
                                                       kOversampleOrder,
                                                       juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple,
                                                       true);

    juce::dsp::ProcessSpec spec {};
    spec.sampleRate       = oversampledRate;
    spec.maximumBlockSize = samplesPerBlock * kOversampleOrder;
    spec.numChannels      = getTotalNumOutputChannels();
    mOversampling->initProcessing(spec.maximumBlockSize);
}

void GregProcessor::releaseResources() {
    // Clean up resources here
}

bool GregProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    // Support stereo input and output
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) return false;

    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo()) return false;

    return true;
}

void GregProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ScopedNoDenormals noDenormals;

    const bool isBypassed = mParameters.getRawParameterValue("bypass")->load() > 0.5f;
    if (isBypassed) return;

    const bool isFilterPre = mParameters.getRawParameterValue("pre")->load() < 0.5f;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Get current parameter values
    float currentDrive  = mParameters.getRawParameterValue("drive")->load();
    float currentTone   = mParameters.getRawParameterValue("tone")->load();
    float currentMix    = mParameters.getRawParameterValue("mix")->load();
    float currentOutput = mParameters.getRawParameterValue("output")->load();

    // Update smoothed values if parameters changed
    if (currentDrive != mPreviousDrive) {
        mDriveSmoothed.setTargetValue(currentDrive);
        mPreviousDrive = currentDrive;
    }
    if (currentTone != mPreviousTone) {
        mToneSmoothed.setTargetValue(currentTone);
        mPreviousTone = currentTone;
    }
    if (currentMix != mPreviousMix) {
        mMixSmoothed.setTargetValue(currentMix);
        mPreviousMix = currentMix;
    }
    if (currentOutput != mPreviousOutput) {
        mOutputSmoothed.setTargetValue(currentOutput);
        mPreviousOutput = currentOutput;
    }

    // Store dry signal for mixing
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::AudioBlock<float> dryBlock(dryBuffer);

    // Get oversampled block
    auto oversampledBlock = mOversampling->processSamplesUp(block);

    // Process each sample with smoothed parameters
    for (int sample = 0; sample < oversampledBlock.getNumSamples(); ++sample) {
        // Get smoothed parameter values for this sample
        float smoothedDrive  = mDriveSmoothed.getNextValue();
        float smoothedTone   = mToneSmoothed.getNextValue();
        float smoothedMix    = mMixSmoothed.getNextValue();
        float smoothedOutput = mOutputSmoothed.getNextValue();

        float driveLinear  = juce::Decibels::decibelsToGain(smoothedDrive);
        float outputLinear = juce::Decibels::decibelsToGain(smoothedOutput);
        float mixRatio     = smoothedMix * 0.01f;  // Convert percentage to 0-1

        for (int channel = 0; channel < oversampledBlock.getNumChannels(); ++channel) {
            auto* channelData = oversampledBlock.getChannelPointer(channel);

            // Apply saturation
            float processedSample = applySaturation(channelData[sample], driveLinear);

            // TODO: Tone filter

            // Apply output gain
            processedSample *= outputLinear;

            channelData[sample] = processedSample;
        }
    }

    // Downsample back to original rate
    mOversampling->processSamplesDown(block);

    // Apply dry/wet mixing at original sample rate
    float mixRatio = mMixSmoothed.getCurrentValue() * 0.01f;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto* wetData = buffer.getWritePointer(channel);
        auto* dryData = dryBuffer.getReadPointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            wetData[sample] = wetData[sample] * mixRatio + dryData[sample] * (1.0f - mixRatio);
        }
    }
}

bool GregProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor* GregProcessor::createEditor() {
    return new GregEditor(*this);
}

void GregProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // Save plugin state here
    const auto state = mParameters.copyState();
    const std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GregProcessor::setStateInformation(const void* data, int sizeInBytes) {
    // Restore plugin state here
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr) {
        if (xmlState->hasTagName(mParameters.state.getType())) {
            mParameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

void GregProcessor::setCurrentPresetName(const juce::String& name) {
    mCurrentPresetName = name;
    if (auto* editor = getActiveEditor()) {
        if (auto* greg = dynamic_cast<GregEditor*>(editor)) { greg->updatePresetName(); }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout GregProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterBool>("bypass", "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("pre", "Pre", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("drive",
                                                                 "Drive",
                                                                 juce::NormalisableRange<float>(0.0f, 30.0f, 0.1f),
                                                                 0.0f,
                                                                 "dB"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("tone",
                                                                 "Tone",
                                                                 juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
                                                                 100.0f,
                                                                 "%"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix",
                                                                 "Mix",
                                                                 juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
                                                                 100.0f,
                                                                 "%"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("output",
                                                                 "Output",
                                                                 juce::NormalisableRange<float>(-30.0f, 30.0f, 0.1f),
                                                                 0.0f,
                                                                 "dB"));

    return {params.begin(), params.end()};
}

float GregProcessor::applySaturation(float input, float drive) {
    float fundamental = input;
    float harmonic    = std::sin(input * drive * 2.0f);
    return std::tanh((fundamental + harmonic * 0.3f) * drive);
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new GregProcessor();
}