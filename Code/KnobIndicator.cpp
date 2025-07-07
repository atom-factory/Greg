// Author: Jake Rieger
// Created: 7/7/2025.
//

#include "KnobIndicator.hpp"

void KnobIndicatorLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                                int x,
                                                int y,
                                                int width,
                                                int height,
                                                float sliderPosProportional,
                                                float rotaryStartAngle,
                                                float rotaryEndAngle,
                                                juce::Slider& slider) {
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
    auto centre = bounds.getCentre();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;

    // Calculate current angle
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // TODO: Also make these parameters
    auto indicatorLength    = 20.0f;
    auto indicatorThickness = 2.0f;

    // Start point inset from the edge
    auto indicatorStart =
      centre.translated((radius - mInset - indicatorLength) * std::cos(angle - juce::MathConstants<float>::halfPi),
                        (radius - mInset - indicatorLength) * std::sin(angle - juce::MathConstants<float>::halfPi));

    // End point inset from the edge
    auto indicatorEnd = centre.translated((radius - mInset) * std::cos(angle - juce::MathConstants<float>::halfPi),
                                          (radius - mInset) * std::sin(angle - juce::MathConstants<float>::halfPi));

    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.drawLine(indicatorStart.x, indicatorStart.y, indicatorEnd.x, indicatorEnd.y, indicatorThickness);
}

KnobIndicator::KnobIndicator(float inset) {
    mLookAndFeel.setInset(inset);
    setSliderStyle(RotaryHorizontalVerticalDrag);
    setTextBoxStyle(NoTextBox, false, 0, 0);
    setLookAndFeel(&mLookAndFeel);
    setRotaryParameters(juce::MathConstants<float>::pi * 1.2f, juce::MathConstants<float>::pi * 2.8f, true);
}

KnobIndicator::~KnobIndicator() {
    setLookAndFeel(nullptr);
}