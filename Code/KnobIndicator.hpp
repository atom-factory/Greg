// Author: Jake Rieger
// Created: 7/7/2025.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class KnobIndicatorLookAndFeel : public juce::LookAndFeel_V4 {
public:
    void drawRotarySlider(juce::Graphics& g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPosProportional,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider& slider) override;

    void setInset(float inset) {
        mInset = inset;
    }

private:
    float mInset {4.0f};
};

class KnobIndicator : public juce::Slider {
public:
    KnobIndicator(float inset);
    ~KnobIndicator() final;

private:
    KnobIndicatorLookAndFeel mLookAndFeel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobIndicator)
};