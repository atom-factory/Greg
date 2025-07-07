#include "PluginProcessor.hpp"
#include "PluginEditor.hpp"
#include "BinaryData.h"

GregEditor::GregEditor(GregProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), mDriveIndicator(40.0f), mToneIndicator(16.0f), mMixIndicator(16.0f) {
    loadImages();
    updatePresetName();
    setupComponents();
    createAttachments();

    // Set the editor size
    setSize(700, 460);
}

GregEditor::~GregEditor() = default;

void GregEditor::paint(juce::Graphics& g) {
    g.drawImage(mBackgroundImage, getLocalBounds().toFloat());
}

void GregEditor::resized() {
    auto bounds = getLocalBounds();

    mPowerButton.setBounds(657, 17, 24, 24);
    mPresetLabel.setBounds(220, 19, 260, 21);
    mPresetLeftButton.setBounds(228, 22, 14, 14);
    mPresetRightButton.setBounds(457, 22, 14, 14);
    mDriveIndicator.setBounds(247, 131, 204, 204);
    mToneIndicator.setBounds(73, 183, 100, 100);
    mMixIndicator.setBounds(526, 183, 100, 100);
}

void GregEditor::updatePresetName() {
    juce::String currentPresetName = audioProcessor.getCurrentPresetName();
    mPresetLabel.setText(currentPresetName, juce::dontSendNotification);
}

void GregEditor::loadImages() {
    mBackgroundImage  = juce::ImageCache::getFromMemory(BinaryData::bg1x_png, BinaryData::bg1x_pngSize);
    mPowerButtonImage = juce::ImageCache::getFromMemory(BinaryData::power_btn1x_png, BinaryData::power_btn1x_pngSize);
    mRightArrowImage =
      juce::ImageCache::getFromMemory(BinaryData::right_arrow1x_png, BinaryData::right_arrow1x_pngSize);
    mLeftArrowImage = juce::ImageCache::getFromMemory(BinaryData::left_arrow1x_png, BinaryData::left_arrow1x_pngSize);
}

void GregEditor::setupComponents() {
    mPresetLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    mPresetLabel.setJustificationType(juce::Justification::centred);
    mPresetLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(mPresetLabel);

    mPowerButton.setImages(false,
                           true,
                           true,
                           mPowerButtonImage,
                           1.0f,
                           juce::Colours::transparentBlack,
                           juce::Image(),
                           1.0f,
                           juce::Colours::transparentBlack,
                           mPowerButtonImage,
                           1.0f,
                           juce::Colours::darkgrey);
    mPowerButton.setClickingTogglesState(true);
    mPowerButton.setToggleState(false, juce::dontSendNotification);
    // Callback isn't really necessary for this button, but will be needed for the preset nav buttons
    // mPowerButton.onClick = [this]() {};
    addAndMakeVisible(mPowerButton);

    mPresetLeftButton.setImages(false,
                                true,
                                true,
                                mLeftArrowImage,
                                1.0f,
                                juce::Colours::transparentWhite,
                                mLeftArrowImage,
                                0.78f,
                                juce::Colours::transparentWhite,
                                mLeftArrowImage,
                                0.56f,
                                juce::Colours::transparentWhite);
    addAndMakeVisible(mPresetLeftButton);

    mPresetRightButton.setImages(false,
                                 true,
                                 true,
                                 mRightArrowImage,
                                 1.0f,
                                 juce::Colours::transparentWhite,
                                 mRightArrowImage,
                                 0.78f,
                                 juce::Colours::transparentWhite,
                                 mRightArrowImage,
                                 0.56f,
                                 juce::Colours::transparentWhite);
    addAndMakeVisible(mPresetRightButton);

    mDriveIndicator.setRange(0.0, 30.0, 0.1);
    mDriveIndicator.setValue(0.0);
    addAndMakeVisible(mDriveIndicator);

    mToneIndicator.setRange(0.0, 100.0, 0.1);
    mToneIndicator.setValue(0.0);
    addAndMakeVisible(mToneIndicator);

    mMixIndicator.setRange(0.0, 100.0, 0.1);
    mMixIndicator.setValue(100.0);
    addAndMakeVisible(mMixIndicator);
}

void GregEditor::createAttachments() {
    mPowerButtonAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.mParameters,
                                                                             "bypass",
                                                                             mPowerButton);

    mDriveAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParameters,
                                                                             "drive",
                                                                             mDriveIndicator);
    mToneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParameters,
                                                                                             "tone",
                                                                                             mToneIndicator);
    mMixAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParameters,
                                                                                            "mix",
                                                                                            mMixIndicator);
    mOutputAttachment =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.mParameters,
                                                                             "output",
                                                                             mOutputSlider);
}