#ifndef CUSTOMLOOKANDFEEL_H_INCLUDED
#define CUSTOMLOOKANDFEEL_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../ff_meters/ff_meters.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class CustomLookAndFeel :
	public LookAndFeel_V4,
	public ff::LevelMeter::LookAndFeelMethods
{
	public:
	
		CustomLookAndFeel();
		~CustomLookAndFeel() {};

		static const Colour backgroundColour;
		static const Colour foregroundColour;
		static const Colour accentColour;

    // LevelMeter
    virtual void setupDefaultMeterColours() override;
    virtual void updateMeterGradients() override;
    virtual Rectangle<float> getMeterInnerBounds(const Rectangle<float> bounds, const ff::LevelMeter::MeterFlags meterType) const override;
    virtual Rectangle<float> getMeterBounds(const Rectangle<float> bounds, const ff::LevelMeter::MeterFlags meterType, const int numChannels, const int channel) const override;
    virtual Rectangle<float> getMeterBarBounds(const Rectangle<float> bounds, const ff::LevelMeter::MeterFlags meterType) const override;
    virtual Rectangle<float> getMeterTickmarksBounds(const Rectangle<float> bounds, const ff::LevelMeter::MeterFlags meterType) const override;
    virtual Rectangle<float> getMeterClipIndicatorBounds(const Rectangle<float> bounds, const ff::LevelMeter::MeterFlags meterType) const override;
    virtual Rectangle<float> drawBackground(Graphics&, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds) override;
    virtual void drawMeterBars(Graphics&, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds, const ff::LevelMeterSource* source, const int fixedNumChannels, const int selectedChannel) override;
    virtual void drawMeterBarsBackground(Graphics&, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds,  const int numChannels, const int fixedNumChannels) override;
    virtual void drawMeterChannel(Graphics&, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds, const ff::LevelMeterSource* source, const int selectedChannel) override;
    virtual void drawMeterChannelBackground(Graphics&, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds) override;
    virtual void drawMeterBar(Graphics&, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds, const float rms, const float peak) override;
    virtual void drawMeterReduction(Graphics& g, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds, const float reduction) override;
    virtual void drawMeterBarBackground(Graphics&, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds) override;
    virtual void drawTickMarks(Graphics&, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds) override;
    virtual void drawClipIndicator(Graphics&, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds, const bool hasClipped) override;
    virtual void drawClipIndicatorBackground(Graphics&, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds) override;
    virtual Rectangle<float> getMeterMaxNumberBounds(const Rectangle<float> bounds, const ff::LevelMeter::MeterFlags meterType) const override;
    virtual void drawMaxNumber(Graphics&, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds, const float maxGain) override;
    virtual void drawMaxNumberBackground(Graphics&, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds) override;
    virtual int hitTestClipIndicator(const Point<int> position, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds, const ff::LevelMeterSource* source) const override;
    virtual int hitTestMaxNumber(const Point<int> position, const ff::LevelMeter::MeterFlags meterType, const Rectangle<float> bounds, const ff::LevelMeterSource* source) const override;

  private:

    ColourGradient horizontalGradient;
    ColourGradient verticalGradient;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CUSTOMLOOKANDFEEL_H_INCLUDED