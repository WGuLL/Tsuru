
#pragma once

#include "JuceHeaderWrapper.h"

namespace MathUtils
{
template <typename T>
[[nodiscard]] NormalisableRange<T> frequencyRange(T min, T max) noexcept
{
    constexpr auto convertFrom0To1Func = [](auto min, auto max, auto value) {
        return juce::mapToLog10(value, min, max);
    };
    constexpr auto convertTo0To1Func = [](auto min, auto max, auto value) {
        return juce::mapFromLog10(value, min, max);
    };
    return {min, max, convertFrom0To1Func, convertTo0To1Func};
}
} // namespace MathUtils
