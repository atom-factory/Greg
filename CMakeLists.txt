cmake_minimum_required(VERSION 3.14)
project(Greg VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)

juce_add_binary_data(ImageResources
    SOURCES
    Resources/bg@1x.png
    Resources/left_arrow@1x.png
    Resources/right_arrow@1x.png
    Resources/power_btn@1x.png
    Resources/pre_off@1x.png
    Resources/pre_on@1x.png
    HEADER_NAME BinaryData.h
    NAMESPACE BinaryData
)

juce_add_plugin(Greg
    # Basic plugin info
    COMPANY_NAME "ATOM Factory"
    BUNDLE_ID "com.ATOMFactory.Greg"
    PLUGIN_MANUFACTURER_CODE ATOM  # 4 characters
    PLUGIN_CODE GREG               # 4 characters
    FORMATS VST3                   # Only VST3 format
    PRODUCT_NAME "Greg"

    # Plugin characteristics
    IS_SYNTH FALSE                 # Set to TRUE if it's a synthesizer
    NEEDS_MIDI_INPUT FALSE         # Set to TRUE if plugin needs MIDI
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE

    # Copy plugin to system folders for testing
    COPY_PLUGIN_AFTER_BUILD TRUE
    #    VST3_COPY_DIR "~/Library/Audio/Plug-Ins/VST3"  # macOS
    VST3_COPY_DIR "C:/Program Files/Common Files/VST3/ATOM Factory"  # Windows
)

target_sources(Greg PRIVATE
    Code/PluginProcessor.cpp
    Code/PluginProcessor.hpp
    Code/PluginEditor.cpp
    Code/PluginEditor.hpp
    Code/KnobIndicator.cpp
    Code/KnobIndicator.hpp
)

target_link_libraries(Greg PRIVATE
    ImageResources
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_graphics
    juce::juce_dsp
)

target_compile_definitions(Greg PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_include_directories(Greg PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/JUCE/modules
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Greg PRIVATE DEBUG=1 _DEBUG=1)
else ()
    target_compile_definitions(Greg PRIVATE NDEBUG=1)
endif ()