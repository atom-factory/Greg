#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "KnobIndicator.hpp"
#include "PluginProcessor.hpp"

class GregEditor : public juce::AudioProcessorEditor {
public:
    GregEditor(GregProcessor&);
    ~GregEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void updatePresetName();

private:
    void loadImages();

    GregProcessor& audioProcessor;

    juce::Image mBackgroundImage;
    juce::Image mPowerButtonImage;
    juce::Image mRightArrowImage;
    juce::Image mLeftArrowImage;

    KnobIndicator mDriveIndicator;
    KnobIndicator mToneIndicator;
    KnobIndicator mMixIndicator;
    juce::Slider mOutputSlider;

    juce::ImageButton mPowerButton;
    juce::ImageButton mPresetLeftButton;
    juce::ImageButton mPresetRightButton;

    juce::Label mDriveLabel;
    juce::Label mToneLabel;
    juce::Label mMixLabel;
    juce::Label mOutputLabel;
    juce::Label mPresetLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mDriveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mToneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mOutputAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mPowerButtonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GregEditor)
};