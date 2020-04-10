#include "CustomLookAndFeel.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

const Colour CustomLookAndFeel::backgroundColour = Colour(40, 44, 53);
const Colour CustomLookAndFeel::foregroundColour = Colours::whitesmoke;
const Colour CustomLookAndFeel::accentColour = Colours::transparentBlack;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

CustomLookAndFeel::CustomLookAndFeel()
{
	setColour(Label::backgroundColourId, backgroundColour);

	setupDefaultMeterColours();
  setColour(ff::LevelMeter::lmTextColour, foregroundColour);
  setColour(ff::LevelMeter::lmTextClipColour, foregroundColour);
  setColour(ff::LevelMeter::lmBackgroundColour, backgroundColour);
  setColour(ff::LevelMeter::lmMeterOutlineColour, foregroundColour);
  setColour(ff::LevelMeter::lmMeterBackgroundColour, findColour(TextEditor::backgroundColourId));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::setupDefaultMeterColours()
{
  setColour(ff::LevelMeter::lmTextColour, juce::Colours::lightgrey);
  setColour(ff::LevelMeter::lmTextClipColour, juce::Colours::white);
  setColour(ff::LevelMeter::lmTextDeactiveColour, juce::Colours::darkgrey);
  setColour(ff::LevelMeter::lmTicksColour, juce::Colours::orange);
  setColour(ff::LevelMeter::lmOutlineColour, juce::Colours::orange);
  setColour(ff::LevelMeter::lmBackgroundColour, juce::Colour(0xff050a29));
  setColour(ff::LevelMeter::lmBackgroundClipColour, juce::Colours::red);
  setColour(ff::LevelMeter::lmMeterForegroundColour, juce::Colours::green);
  setColour(ff::LevelMeter::lmMeterOutlineColour, juce::Colours::lightgrey);
  setColour(ff::LevelMeter::lmMeterBackgroundColour, juce::Colours::darkgrey);
  setColour(ff::LevelMeter::lmMeterMaxNormalColour, juce::Colours::lightgrey);
  setColour(ff::LevelMeter::lmMeterMaxWarnColour, juce::Colours::orange);
  setColour(ff::LevelMeter::lmMeterMaxOverColour, juce::Colours::darkred);
  setColour(ff::LevelMeter::lmMeterGradientLowColour, juce::Colours::green);
  setColour(ff::LevelMeter::lmMeterGradientMidColour, juce::Colours::lightgoldenrodyellow);
  setColour(ff::LevelMeter::lmMeterGradientMaxColour, juce::Colours::red);
  setColour(ff::LevelMeter::lmMeterReductionColour, juce::Colours::orange);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::updateMeterGradients()
{
  horizontalGradient.clearColours();
  verticalGradient.clearColours();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

juce::Rectangle<float> CustomLookAndFeel::getMeterInnerBounds(const juce::Rectangle<float> bounds,
  const ff::LevelMeter::MeterFlags meterType) const
{
  if (meterType & ff::LevelMeter::HasBorder)
  {
    const auto corner = std::min(bounds.getWidth(), bounds.getHeight()) * 0.01f;
    return bounds.reduced(3 + corner);
  }

  return bounds;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

juce::Rectangle<float> CustomLookAndFeel::getMeterBounds(const juce::Rectangle<float> bounds,
  const ff::LevelMeter::MeterFlags meterType,
  const int numChannels,
  const int channel) const
{
  if (meterType & ff::LevelMeter::SingleChannel) {
    return bounds;
  }
  else {
    if (meterType & ff::LevelMeter::Horizontal) {
      const float h = bounds.getHeight() / numChannels;
      return bounds.withHeight(h).withY(bounds.getY() + channel * h);
    }
    else {
      const float w = bounds.getWidth() / numChannels;
      return bounds.withWidth(w).withX(bounds.getX() + channel * w);
    }
  }
  return juce::Rectangle<float>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** Override this callback to define the placement of the actual meter bar. */
juce::Rectangle<float> CustomLookAndFeel::getMeterBarBounds(const juce::Rectangle<float> bounds,
  const ff::LevelMeter::MeterFlags meterType) const
{
  if (meterType & ff::LevelMeter::Minimal)
  {
    if (meterType & ff::LevelMeter::Horizontal)
    {
      const auto margin = bounds.getHeight() * 0.05f;
      const auto h = bounds.getHeight() - 2.0f * margin;
      const auto left = bounds.getX() + margin;
      const auto right = bounds.getRight() - (4.0f * margin + h);
      return juce::Rectangle<float>(bounds.getX() + margin,
        bounds.getY() + margin,
        right - left,
        h);
    }

    const auto margin = bounds.getWidth() * 0.05f;
    const auto top = bounds.getY() + 2.0f * margin + bounds.getWidth() * 0.5f;
    const auto bottom = (meterType & ff::LevelMeter::MaxNumber) ?
      bounds.getBottom() - (3.0f * margin + (bounds.getWidth() - margin * 2.0f))
      : bounds.getBottom() - margin;
    return juce::Rectangle<float>(bounds.getX() + margin, top,
      bounds.getWidth() - margin * 2.0f, bottom - top);
  }

  if (meterType & ff::LevelMeter::Vintage)
    return bounds;

  if (meterType & ff::LevelMeter::Horizontal)
  {
    const auto margin = bounds.getHeight() * 0.05f;
    const auto h = bounds.getHeight() * 0.5f - 2.0f * margin;
    const auto left = 60.0f + 3.0f * margin;
    const auto right = bounds.getRight() - (4.0f * margin + h * 0.5f);
    return juce::Rectangle<float>(bounds.getX() + left,
      bounds.getY() + margin,
      right - left,
      h);
  }

  const auto margin = bounds.getWidth() * 0.05f;
  auto w = bounds.getWidth() * 0.45f;
  if (meterType & ff::LevelMeter::Compact) w = bounds.getWidth() * 0.9f;
  const auto top = bounds.getY() + 6.0f * margin;
  const auto bottom = bounds.getBottom() - (2.0f * margin + 25.0f);
  return juce::Rectangle<float>(bounds.getX() + margin, top, w, bottom - top);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** Override this callback to define the placement of the tickmarks.
 To disable this feature return an empty rectangle. */
juce::Rectangle<float> CustomLookAndFeel::getMeterTickmarksBounds(const juce::Rectangle<float> bounds,
  const ff::LevelMeter::MeterFlags meterType) const
{
  if (meterType & ff::LevelMeter::Minimal)
  {
    if (meterType & ff::LevelMeter::Horizontal) {
      return getMeterBarBounds(bounds, meterType).reduced(0.0, 2.0);
    }
    else {
      return getMeterBarBounds(bounds, meterType).reduced(2.0, 0.0);
    }
  }

  if (meterType & ff::LevelMeter::Vintage)
    return bounds;

  if (meterType & ff::LevelMeter::Compact)
    return Rectangle<float>();

  if (meterType & ff::LevelMeter::Horizontal) {
    const auto margin = bounds.getHeight() * 0.05f;
    const auto h = bounds.getHeight() * 0.5f - 2.0f * margin;
    const auto left = 60.0f + 3.0f * margin;
    const auto right = bounds.getRight() - (4.0f * margin + h * 0.5f);
    return juce::Rectangle<float>(bounds.getX() + left,
      bounds.getCentreY() + margin,
      right - left,
      h);
  }
  else
  {
    const auto margin = bounds.getWidth() * 0.05f;
    const auto w = bounds.getWidth() * 0.45f;
    const auto top = bounds.getY() + 2.0f * margin + w * 0.5f + 2.0f;
    const auto bottom = bounds.getBottom() - (2.0f * margin + 25.0f + 2.0f);
    return juce::Rectangle<float>(bounds.getCentreX(), top, w, bottom - top);
  }

  return juce::Rectangle<float>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** Override this callback to define the placement of the clip indicator light.
 To disable this feature return an empty rectangle. */
juce::Rectangle<float> CustomLookAndFeel::getMeterClipIndicatorBounds(const juce::Rectangle<float> bounds,
  const ff::LevelMeter::MeterFlags meterType) const
{
  if (meterType & ff::LevelMeter::Minimal)
  {
    if (meterType & ff::LevelMeter::Horizontal)
    {
      const auto margin = bounds.getHeight() * 0.05f;
      const auto h = bounds.getHeight() - 2.0f * margin;
      return juce::Rectangle<float>(bounds.getRight() - (margin + h),
        bounds.getY() + margin,
        h,
        h);
    }
    else
    {
      const auto margin = bounds.getWidth() * 0.05f;
      const auto w = bounds.getWidth() - margin * 2.0f;
      return juce::Rectangle<float>(bounds.getX() + margin,
        bounds.getY() + margin,
        w,
        w * 0.5f);
    }
  }
  else if (meterType & ff::LevelMeter::Vintage)
  {
    return bounds;
  }
  else
  {
    if (meterType & ff::LevelMeter::Horizontal)
    {
      const auto margin = bounds.getHeight() * 0.05f;
      const auto h = bounds.getHeight() * 0.5f - 2.0f * margin;
      return juce::Rectangle<float>(bounds.getRight() - (margin + h * 0.5f),
        bounds.getY() + margin,
        h * 0.5f,
        h);
    }
    else
    {
      const auto margin = bounds.getWidth() * 0.05f;
      auto w = bounds.getWidth() * 0.45f;
      if (meterType & ff::LevelMeter::Compact) w = bounds.getWidth() * 0.9f;
      return juce::Rectangle<float>(bounds.getX() + margin, bounds.getY() + margin, w, 4.0f * margin);
    }
  }
  return juce::Rectangle<float>();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** Override this callback to define the placement of the max level.
 To disable this feature return an empty rectangle. */
juce::Rectangle<float> CustomLookAndFeel::getMeterMaxNumberBounds(const juce::Rectangle<float> bounds,
  const ff::LevelMeter::MeterFlags meterType) const
{
  if (meterType & ff::LevelMeter::Minimal)
  {
    if (meterType & ff::LevelMeter::MaxNumber)
    {
      if (meterType & ff::LevelMeter::Horizontal)
      {
        const auto margin = bounds.getHeight() * 0.05f;
        const auto h = bounds.getHeight() - 2.0f * margin;
        return juce::Rectangle<float>(bounds.getRight() - (margin + h),
          bounds.getY() + margin,
          h, h);
      }
      else
      {
        const auto margin = bounds.getWidth() * 0.05f;
        const auto w = bounds.getWidth() - margin * 2.0f;
        const auto h = w * 0.6f;
        return juce::Rectangle<float>(bounds.getX() + margin,
          bounds.getBottom() - (margin + h),
          w, h);
      }
    }
    else
    {
      return juce::Rectangle<float>();
    }
  }
  else if (meterType & ff::LevelMeter::Vintage) {
    return bounds;
  }
  else {
    if (meterType & ff::LevelMeter::Horizontal)
    {
      const auto margin = bounds.getHeight() * 0.05f;
      return juce::Rectangle<float>(bounds.getX() + margin,
        bounds.getCentreY() + margin,
        60,
        bounds.getHeight() * 0.5f - margin * 2.0f);
    }
    else
    {
      const auto margin = bounds.getWidth() * 0.05f;
      return juce::Rectangle<float>(bounds.getX() + margin,
        bounds.getBottom() - (margin + 25),
        bounds.getWidth() - 2 * margin,
        25.0);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

juce::Rectangle<float> CustomLookAndFeel::drawBackground(juce::Graphics& g,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds)
{
  g.setColour(findColour(ff::LevelMeter::lmBackgroundColour));
  if (meterType & ff::LevelMeter::HasBorder)
  {
    const auto corner = std::min(bounds.getWidth(), bounds.getHeight()) * 0.01f;
    g.fillRoundedRectangle(bounds, corner);
    g.setColour(findColour(ff::LevelMeter::lmOutlineColour));
    g.drawRoundedRectangle(bounds.reduced(3), corner, 2);
    return bounds.reduced(3 + corner);
  }
  else
  {
    g.fillRect(bounds);
    return bounds;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::drawMeterBars(juce::Graphics& g,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds,
  const ff::LevelMeterSource* source,
  const int fixedNumChannels = -1,
  const int selectedChannel = -1)
{
  if (source == nullptr)
    return;

  const juce::Rectangle<float> innerBounds = getMeterInnerBounds(bounds, meterType);
  const int numChannels = source->getNumChannels();
  if (meterType & ff::LevelMeter::Minimal)
  {
    if (meterType & ff::LevelMeter::Horizontal)
    {
      const float height = innerBounds.getHeight() / (2 * numChannels - 1);
      juce::Rectangle<float> meter = innerBounds.withHeight(height);
      for (int channel = 0; channel < numChannels; ++channel)
      {
        meter.setY(height * channel * 2);
        {
          juce::Rectangle<float> meterBarBounds = getMeterBarBounds(meter, meterType);
          drawMeterBar(g, meterType, meterBarBounds,
            source->getRMSLevel(channel),
            source->getMaxLevel(channel));
          const float reduction = source->getReductionLevel(channel);
          if (reduction < 1.0)
            drawMeterReduction(g, meterType,
              meterBarBounds.withBottom(meterBarBounds.getCentreY()),
              reduction);
        }

        juce::Rectangle<float> clip = getMeterClipIndicatorBounds(meter, meterType);
        if (!clip.isEmpty())
          drawClipIndicator(g, meterType, clip, source->getClipFlag(channel));
        juce::Rectangle<float> maxNum = getMeterMaxNumberBounds(meter, meterType);

        if (!maxNum.isEmpty())
          drawMaxNumber(g, meterType, maxNum, source->getMaxOverallLevel(channel));

        if (channel < numChannels - 1)
        {
          meter.setY(height * (channel * 2 + 1));
          juce::Rectangle<float> ticks = getMeterTickmarksBounds(meter, meterType);
          if (!ticks.isEmpty())
            drawTickMarks(g, meterType, ticks);
        }
      }
    }
    else
    {
      const float width = innerBounds.getWidth() / (2 * numChannels - 1);
      juce::Rectangle<float> meter = innerBounds.withWidth(width);
      for (int channel = 0; channel < numChannels; ++channel) {
        meter.setX(width * channel * 2);
        {
          juce::Rectangle<float> meterBarBounds = getMeterBarBounds(meter, meterType);
          drawMeterBar(g, meterType, getMeterBarBounds(meter, meterType),
            source->getRMSLevel(channel),
            source->getMaxLevel(channel));
          const float reduction = source->getReductionLevel(channel);
          if (reduction < 1.0)
            drawMeterReduction(g, meterType,
              meterBarBounds.withLeft(meterBarBounds.getCentreX()),
              reduction);
        }
        juce::Rectangle<float> clip = getMeterClipIndicatorBounds(meter, meterType);
        if (!clip.isEmpty())
          drawClipIndicator(g, meterType, clip, source->getClipFlag(channel));
        juce::Rectangle<float> maxNum = getMeterMaxNumberBounds(innerBounds.withWidth(innerBounds.getWidth() / numChannels).withX(innerBounds.getX() + channel * (innerBounds.getWidth() / numChannels)), meterType);
        if (!maxNum.isEmpty())
          drawMaxNumber(g, meterType, maxNum, source->getMaxOverallLevel(channel));
        if (channel < numChannels - 1) {
          meter.setX(width * (channel * 2 + 1));
          juce::Rectangle<float> ticks = getMeterTickmarksBounds(meter, meterType);
          if (!ticks.isEmpty())
            drawTickMarks(g, meterType, ticks);
        }
      }
    }
  }
  else if (meterType & ff::LevelMeter::SingleChannel)
  {
    if (selectedChannel >= 0)
      drawMeterChannel(g, meterType, innerBounds, source, selectedChannel);
  }
  else
  {
    const int numChannels = source->getNumChannels();
    const int numDrawnChannels = fixedNumChannels < 0 ? numChannels : fixedNumChannels;
    for (int channel = 0; channel < numChannels; ++channel)
      drawMeterChannel(g, meterType,
        getMeterBounds(innerBounds, meterType, numDrawnChannels, channel),
        source, channel);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::drawMeterBarsBackground(juce::Graphics& g,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds,
  const int numChannels,
  const int fixedNumChannels)
{
  const juce::Rectangle<float> innerBounds = getMeterInnerBounds(bounds, meterType);
  if (meterType & ff::LevelMeter::Minimal) {
    if (meterType & ff::LevelMeter::Horizontal) {
      const float height = innerBounds.getHeight() / (2 * numChannels - 1);
      juce::Rectangle<float> meter = innerBounds.withHeight(height);
      for (int channel = 0; channel < numChannels; ++channel) {
        meter.setY(height * channel * 2);
        drawMeterBarBackground(g, meterType, getMeterBarBounds(meter, meterType));
        juce::Rectangle<float> clip = getMeterClipIndicatorBounds(meter, meterType);
        if (!clip.isEmpty())
          drawClipIndicatorBackground(g, meterType, clip);
        if (channel < numChannels - 1) {
          meter.setY(height * (channel * 2 + 1));
          juce::Rectangle<float> ticks = getMeterTickmarksBounds(meter, meterType);
          if (!ticks.isEmpty())
            drawTickMarks(g, meterType, ticks);
        }
      }
    }
    else {
      const float width = innerBounds.getWidth() / (2 * numChannels - 1);
      juce::Rectangle<float> meter = innerBounds.withWidth(width);
      for (int channel = 0; channel < numChannels; ++channel) {
        meter.setX(width * channel * 2);
        drawMeterBarBackground(g, meterType, getMeterBarBounds(meter, meterType));
        juce::Rectangle<float> clip = getMeterClipIndicatorBounds(meter, meterType);
        if (!clip.isEmpty())
          drawClipIndicatorBackground(g, meterType, clip);
        if (channel < numChannels - 1) {
          meter.setX(width * (channel * 2 + 1));
          juce::Rectangle<float> ticks = getMeterTickmarksBounds(meter, meterType);
          if (!ticks.isEmpty())
            drawTickMarks(g, meterType, ticks);
        }
      }
    }
  }
  else if (meterType & ff::LevelMeter::SingleChannel) {
    drawMeterChannelBackground(g, meterType, innerBounds);
  }
  else {
    for (int channel = 0; channel < numChannels; ++channel) {
      drawMeterChannelBackground(g, meterType,
        getMeterBounds(innerBounds, meterType,
          fixedNumChannels < 0 ? numChannels : fixedNumChannels,
          channel));
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::drawMeterChannel(juce::Graphics& g,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds,
  const ff::LevelMeterSource* source,
  const int selectedChannel)
{
  if (source == nullptr)
    return;

  juce::Rectangle<float> meter = getMeterBarBounds(bounds, meterType);
  if (!meter.isEmpty())
  {
    if (meterType & ff::LevelMeter::Reduction)
    {
      drawMeterBar(g, meterType, meter,
        source->getReductionLevel(selectedChannel),
        0.0f);
    }
    else
    {
      drawMeterBar(g, meterType, meter,
        source->getRMSLevel(selectedChannel),
        source->getMaxLevel(selectedChannel));
      const float reduction = source->getReductionLevel(selectedChannel);
      if (reduction < 1.0)
      {
        if (meterType & ff::LevelMeter::Horizontal)
          drawMeterReduction(g, meterType,
            meter.withBottom(meter.getCentreY()),
            reduction);
        else
          drawMeterReduction(g, meterType,
            meter.withLeft(meter.getCentreX()),
            reduction);
      }
    }
  }

  if (source->getClipFlag(selectedChannel)) {
    juce::Rectangle<float> clip = getMeterClipIndicatorBounds(bounds, meterType);
    if (!clip.isEmpty())
      drawClipIndicator(g, meterType, clip, true);
  }

  juce::Rectangle<float> maxes = getMeterMaxNumberBounds(bounds, meterType);
  if (!maxes.isEmpty())
  {
    if (meterType & ff::LevelMeter::Reduction)
      drawMaxNumber(g, meterType, maxes, source->getReductionLevel(selectedChannel));
    else
      drawMaxNumber(g, meterType, maxes, source->getMaxOverallLevel(selectedChannel));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::drawMeterChannelBackground(juce::Graphics& g,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds)
{
  juce::Rectangle<float> meter = getMeterBarBounds(bounds, meterType);
  if (!meter.isEmpty())
    drawMeterBarBackground(g, meterType, meter);

  juce::Rectangle<float> clip = getMeterClipIndicatorBounds(bounds, meterType);
  if (!clip.isEmpty())
    drawClipIndicatorBackground(g, meterType, clip);

  juce::Rectangle<float> ticks = getMeterTickmarksBounds(bounds, meterType);
  if (!ticks.isEmpty())
    drawTickMarks(g, meterType, ticks);

  juce::Rectangle<float> maxes = getMeterMaxNumberBounds(bounds, meterType);
  if (!maxes.isEmpty()) {
    drawMaxNumberBackground(g, meterType, maxes);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::drawMeterBar(juce::Graphics& g,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds,
  const float rms, const float peak)
{
  const auto infinity = meterType & ff::LevelMeter::Reduction ? -30.0f : -80.0f;
  const auto rmsDb = juce::Decibels::gainToDecibels(rms, infinity);
  const auto peakDb = juce::Decibels::gainToDecibels(peak, infinity);

  const juce::Rectangle<float> floored(ceilf(bounds.getX()) + 1.0f, ceilf(bounds.getY()) + 1.0f,
    floorf(bounds.getRight()) - (ceilf(bounds.getX() + 2.0f)),
    floorf(bounds.getBottom()) - (ceilf(bounds.getY()) + 2.0f));

  if (meterType & ff::LevelMeter::Vintage) {
    // TODO
  }
  else if (meterType & ff::LevelMeter::Reduction)
  {
    const float limitDb = juce::Decibels::gainToDecibels(rms, infinity);
    g.setColour(findColour(ff::LevelMeter::lmMeterReductionColour));
    if (meterType & ff::LevelMeter::Horizontal)
      g.fillRect(floored.withLeft(floored.getX() + limitDb * floored.getWidth() / infinity));
    else
      g.fillRect(floored.withBottom(floored.getY() + limitDb * floored.getHeight() / infinity));
  }
  else
  {
    if (meterType & ff::LevelMeter::Horizontal)
    {
      if (horizontalGradient.getNumColours() < 2)
      {
        horizontalGradient = juce::ColourGradient(findColour(ff::LevelMeter::lmMeterGradientLowColour),
          floored.getX(), floored.getY(),
          findColour(ff::LevelMeter::lmMeterGradientMaxColour),
          floored.getRight(), floored.getY(), false);
        horizontalGradient.addColour(0.5, findColour(ff::LevelMeter::lmMeterGradientLowColour));
        horizontalGradient.addColour(0.75, findColour(ff::LevelMeter::lmMeterGradientMidColour));
      }
      g.setGradientFill(horizontalGradient);
      g.fillRect(floored.withRight(floored.getRight() - rmsDb * floored.getWidth() / infinity));

      if (peakDb > -49.0)
      {
        g.setColour(findColour((peakDb > -0.3f) ? ff::LevelMeter::lmMeterMaxOverColour :
          ((peakDb > -5.0) ? ff::LevelMeter::lmMeterMaxWarnColour :
            ff::LevelMeter::lmMeterMaxNormalColour)));
        g.drawVerticalLine(juce::roundToInt(floored.getRight() - juce::jmax(peakDb * floored.getWidth() / infinity, 0.0f)),
          floored.getY(), floored.getBottom());
      }
    }
    else
    {
      // vertical
      if (verticalGradient.getNumColours() < 2)
      {
        verticalGradient = juce::ColourGradient(findColour(ff::LevelMeter::lmMeterGradientLowColour),
          floored.getX(), floored.getBottom(),
          findColour(ff::LevelMeter::lmMeterGradientMaxColour),
          floored.getX(), floored.getY(), false);
        verticalGradient.addColour(0.5f, findColour(ff::LevelMeter::lmMeterGradientLowColour));
        verticalGradient.addColour(0.75f, findColour(ff::LevelMeter::lmMeterGradientMidColour));
      }
      g.setGradientFill(verticalGradient);
      g.fillRect(floored.withTop(floored.getY() + rmsDb * floored.getHeight() / infinity));

      if (peakDb > -49.0f) {
        g.setColour(findColour((peakDb > -0.3f) ? ff::LevelMeter::lmMeterMaxOverColour :
          ((peakDb > -5.0f) ? ff::LevelMeter::lmMeterMaxWarnColour :
            ff::LevelMeter::lmMeterMaxNormalColour)));
        g.drawHorizontalLine(juce::roundToInt(floored.getY() + juce::jmax(peakDb * floored.getHeight() / infinity, 0.0f)),
          floored.getX(), floored.getRight());
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::drawMeterReduction(juce::Graphics& g,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds,
  const float reduction)
{
  const auto infinity = -30.0f;

  const juce::Rectangle<float> floored(ceilf(bounds.getX()) + 1.0f, ceilf(bounds.getY()) + 1.0f,
    floorf(bounds.getRight()) - (ceilf(bounds.getX() + 2.0f)),
    floorf(bounds.getBottom()) - (ceilf(bounds.getY()) + 2.0f));

  const auto limitDb = juce::Decibels::gainToDecibels(reduction, infinity);
  g.setColour(findColour(ff::LevelMeter::lmMeterReductionColour));
  if (meterType & ff::LevelMeter::Horizontal) {
    g.fillRect(floored.withLeft(floored.getX() + limitDb * floored.getWidth() / infinity));
  }
  else {
    g.fillRect(floored.withBottom(floored.getY() + limitDb * floored.getHeight() / infinity));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::drawMeterBarBackground(juce::Graphics& g,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds)
{
  juce::ignoreUnused(meterType);
  g.setColour(findColour(ff::LevelMeter::lmMeterBackgroundColour));
  g.fillRect(bounds);

  g.setColour(findColour(ff::LevelMeter::lmMeterOutlineColour));
  g.drawRect(bounds, 1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::drawTickMarks(juce::Graphics& g,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds)
{
  const auto infinity = meterType & ff::LevelMeter::Reduction ? -30.0f : -80.0f;

  g.setColour(findColour(ff::LevelMeter::lmTicksColour));
  if (meterType & ff::LevelMeter::Minimal)
  {
    if (meterType & ff::LevelMeter::Horizontal)
    {
      for (int i = 0; i < 11; ++i)
        g.drawVerticalLine(juce::roundToInt(bounds.getX() + i * 0.1f * bounds.getWidth()),
          bounds.getY() + 4,
          bounds.getBottom() - 4);
    }
    else
    {
      const auto h = (bounds.getHeight() - 2.0f) * 0.1f;
      for (int i = 0; i < 11; ++i)
      {
        g.drawHorizontalLine(juce::roundToInt(bounds.getY() + i * h + 1),
          bounds.getX() + 4,
          bounds.getRight());
      }
      if (h > 10 && bounds.getWidth() > 20)
      {
        // don't print tiny numbers
        g.setFont(h * 0.5f);
        for (int i = 0; i < 10; ++i) {
          g.drawFittedText(juce::String(i * 0.1 * infinity),
            juce::roundToInt(bounds.getX()),
            juce::roundToInt(bounds.getY() + i * h + 2),
            juce::roundToInt(bounds.getWidth()),
            juce::roundToInt(h * 0.6f),
            juce::Justification::centredTop, 1);
        }
      }
    }
  }
  else if (meterType & ff::LevelMeter::Vintage)
  {
    // TODO
  }
  else
  {
    if (meterType & ff::LevelMeter::Horizontal)
    {
      for (int i = 0; i < 11; ++i)
        g.drawVerticalLine(juce::roundToInt(bounds.getX() + i * 0.1f * bounds.getWidth()),
          bounds.getY() + 4,
          bounds.getBottom() - 4);
    }
    else
    {
      const auto h = (bounds.getHeight() - 2.0f) * 0.05f;
      g.setFont(h * 0.8f);
      for (int i = 0; i < 21; ++i) {
        const auto y = bounds.getY() + i * h;
        if (i % 2 == 0)
        {
          g.drawHorizontalLine(juce::roundToInt(y + 1),
            bounds.getX() + 4,
            bounds.getRight());
          if (i < 20)
          {
            g.drawFittedText(juce::String(i * 0.05 * infinity),
              juce::roundToInt(bounds.getX()),
              juce::roundToInt(y + 4),
              juce::roundToInt(bounds.getWidth()),
              juce::roundToInt(h * 0.6f),
              juce::Justification::topRight, 1);
          }
        }
        else
        {
          g.drawHorizontalLine(juce::roundToInt(y + 2),
            bounds.getX() + 4,
            bounds.getCentreX());
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::drawClipIndicator(juce::Graphics& g,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds,
  const bool hasClipped)
{
  juce::ignoreUnused(meterType);

  g.setColour(findColour(hasClipped ? ff::LevelMeter::lmBackgroundClipColour : ff::LevelMeter::lmMeterBackgroundColour));
  g.fillRect(bounds);
  g.setColour(findColour(ff::LevelMeter::lmMeterOutlineColour));
  g.drawRect(bounds, 1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::drawClipIndicatorBackground(juce::Graphics& g,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds)
{
  juce::ignoreUnused(meterType);

  g.setColour(findColour(ff::LevelMeter::lmMeterBackgroundColour));
  g.fillRect(bounds);
  g.setColour(findColour(ff::LevelMeter::lmMeterOutlineColour));
  g.drawRect(bounds, 1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::drawMaxNumber(juce::Graphics& g,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds,
  const float maxGain)
{
  juce::ignoreUnused(meterType);

  g.setColour(findColour(ff::LevelMeter::lmMeterBackgroundColour));
  g.fillRect(bounds);
  const float maxDb = juce::Decibels::gainToDecibels(maxGain, -80.0f);
  g.setColour(findColour(maxDb > 0.0 ? ff::LevelMeter::lmTextClipColour : ff::LevelMeter::lmTextColour));
  g.setFont(bounds.getHeight() * 0.5f);
  g.drawFittedText(juce::String(maxDb, 1) + " dB",
    bounds.reduced(2.0).toNearestInt(),
    juce::Justification::centred, 1);
  g.setColour(findColour(ff::LevelMeter::lmMeterOutlineColour));
  g.drawRect(bounds, 1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void CustomLookAndFeel::drawMaxNumberBackground(juce::Graphics& g,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds)
{
  juce::ignoreUnused(meterType);

  g.setColour(findColour(ff::LevelMeter::lmMeterBackgroundColour));
  g.fillRect(bounds);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int CustomLookAndFeel::hitTestClipIndicator(const juce::Point<int> position,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds,
  const ff::LevelMeterSource* source) const
{
  if (source) {
    const int numChannels = source->getNumChannels();
    for (int i = 0; i < numChannels; ++i) {
      if (getMeterClipIndicatorBounds(getMeterBounds
      (bounds, meterType, source->getNumChannels(), i), meterType)
        .contains(position.toFloat())) {
        return i;
      }
    }
  }
  return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int CustomLookAndFeel::hitTestMaxNumber(const juce::Point<int> position,
  const ff::LevelMeter::MeterFlags meterType,
  const juce::Rectangle<float> bounds,
  const ff::LevelMeterSource* source) const
{
  if (source) {
    const int numChannels = source->getNumChannels();
    for (int i = 0; i < numChannels; ++i) {
      if (getMeterMaxNumberBounds(getMeterBounds
      (bounds, meterType, source->getNumChannels(), i), meterType)
        .contains(position.toFloat())) {
        return i;
      }
    }
  }
  return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
