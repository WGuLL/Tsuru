/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>

/**
 */
class FunFilterEditor : public juce::AudioProcessorEditor {
public:
  FunFilterEditor(FunFilterAudioProcessor &);
  ~FunFilterEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  FunFilterAudioProcessor &audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FunFilterEditor)
};
