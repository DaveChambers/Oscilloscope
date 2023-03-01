/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OscilloscopeAudioProcessorEditor::OscilloscopeAudioProcessorEditor (OscilloscopeAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    oscilloscopeComponent.reset(new UntriggeredOscilloscope(audioProcessor, audioProcessor.getSampleRate()));

    addAndMakeVisible(oscilloscopeComponent.get());
    addAndMakeVisible(drawGrid);
    addAndMakeVisible(bufferLength);
    drawGrid.setButtonText("Grid");
    
    setSize(1400, 700);

    auto area = getLocalBounds();
    drawGrid.setSize(100, 20);
    oscilloscopeComponent->setTopLeftPosition(0, 0);
    oscilloscopeComponent->setSize(area.getWidth(), area.getHeight() - 30);
    drawGrid.setTopLeftPosition(10, area.getHeight() - 25);
    drawGrid.onStateChange = [this]() {
        auto draw = drawGrid.getToggleState();
        oscilloscopeComponent->setGridCheck(draw);
    };

    // Get a pointer to the AudioParameterBool
    //auto* myToggleParam = dynamic_cast<juce::AudioParameterBool*>(processor.getParameters().getUnchecked(0));
    auto drawGridValue = audioProcessor.getTreeState()->getParameter("drawGrid")->getValue();
    drawGrid.setToggleState(drawGridValue, juce::sendNotification);

    // Attach the ToggleButton to the AudioParameterBool
    gridAttachment = new juce::AudioProcessorValueTreeState::ButtonAttachment(*audioProcessor.getTreeState(), juce::String("drawGrid"), drawGrid);

    bufferLength.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    bufferLength.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    auto bufferLengthValue = audioProcessor.getTreeState()->getParameter("bufferLength")->getValue();
    bufferLength.setValue((double)bufferLengthValue, juce::sendNotification);
    bufferLengthAttachment = new juce::AudioProcessorValueTreeState::SliderAttachment(*audioProcessor.getTreeState(), juce::String("bufferLength"), bufferLength);
    bufferLength.setTopLeftPosition(1000, area.getHeight() - 25);
    bufferLength.setSize(350, 20);
    
    setResizable(true, true);
    setResizeLimits(256, 256, 1920, 1080);
    resized(); // need to be called in constructor since rescaling logic makes oscilloscope invisible on creation
    
}

OscilloscopeAudioProcessorEditor::~OscilloscopeAudioProcessorEditor()
{
    delete gridAttachment;
    delete bufferLengthAttachment;
}

//==============================================================================
void OscilloscopeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colour(95, 2, 31).darker(0.8));

}

void OscilloscopeAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds();
    float yDiff = (float)(area.getHeight() - 30) - oscilloscopeComponent->getHeight();

    // rescale oscilloscope
    float x_sc = (float)area.getWidth() / oscilloscopeComponent->getWidth();
    float y_sc = (float)(area.getHeight() - 30) / oscilloscopeComponent->getHeight();
    oscilloscopeComponent->setTransform(juce::AffineTransform::scale(x_sc, y_sc));
    drawGrid.setTransform(juce::AffineTransform::translation(0, yDiff));
    
}


